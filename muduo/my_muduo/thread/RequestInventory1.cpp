
#include <cstdio>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>
#include <cstdio>

class Request;

class Inventory
{
public:
    Inventory() : m_requestsPtr(new RequestList){}
    void add(Request *req)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if(!m_requestsPtr.unique()){
            m_requestsPtr.reset(new RequestList(*m_requestsPtr));
            printf("Inventory::add() copy the whole list\n");
        }
        assert(m_requestsPtr.unique());
        m_requestsPtr->insert(req);
    }

    void remove(Request *req)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if(!m_requestsPtr.unique()){
            m_requestsPtr.reset(new RequestList(*m_requestsPtr));
            printf("Inventory::remove() copy the whole list\n");
        }
        assert(m_requestsPtr.unique());
        m_requestsPtr->erase(req);
    }

    void printAll() const;
    
private:
    using RequestList=std::set<Request*>;
    using RequestListPtr=std::shared_ptr<RequestList>;
    mutable std::mutex m_mutex;
    RequestListPtr m_requestsPtr;
};

Inventory g_inventory;

class Request
{
public:
    Request() : m_val(0){}

    ~Request() 
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        printf("~Request\n");
        g_inventory.remove(this);
    }

    void process() 
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        printf("process: add\n");
        g_inventory.add(this);
    }

    void print() const __attribute__ ((noinline))
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        printf("print Request %p x=%d\n", this, m_val);
    }

private:
    mutable std::mutex m_mutex;
    int m_val ;
};


void Inventory::printAll() const
{
    RequestListPtr requests;
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        requests = m_requestsPtr;
        assert(!requests.unique());
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    for(auto it = m_requestsPtr->begin(); it!=m_requestsPtr->end(); ++it)
        (*it)->print();
}

void threadFunc()
{
    Request *req = new Request;
    req->process();
    delete req;
}

int main()
{
    std::thread thr(threadFunc);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    g_inventory.printAll(); 
    thr.join();
}


