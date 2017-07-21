
#include <cstdio>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>
#include <cstdio>
using namespace std;

class Request;
using RequestPtr=std::shared_ptr<Request>;

class Inventory
{
public:
    Inventory() : m_requestsPtr(new RequestList){}
    void add(const RequestPtr& req)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if(!m_requestsPtr.unique()){
            m_requestsPtr.reset(new RequestList(*m_requestsPtr));
            printf("Inventory::add() copy the whole list\n");
        }
        assert(m_requestsPtr.unique());
        m_requestsPtr->insert(req);
    }

    void remove(const RequestPtr &req)
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
    using RequestList=std::set<RequestPtr>;
    using RequestListPtr=std::shared_ptr<RequestList>;
    mutable std::mutex m_mutex;
    RequestListPtr m_requestsPtr;
};

Inventory g_inventory;

class Request : public std::enable_shared_from_this<Request>
{
public:
    Request() : m_val(0){}

    ~Request() 
    {
        m_val = -1;
    }

    void cancel() __attribute__((noinline))
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_val = 1;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        printf("cancel\n");
        g_inventory.remove(shared_from_this());
    }

    void process() 
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        printf("process: add\n");
        g_inventory.add(shared_from_this());
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
    for(auto it = requests->begin(); it!=requests->end(); ++it)
        (*it)->print();
}

void threadFunc()
{
    RequestPtr req =std::make_shared<Request>();
    req->process();
    req->cancel();
}

int main()
{
    std::thread thr(threadFunc);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    g_inventory.printAll(); 
    thr.join();
}


