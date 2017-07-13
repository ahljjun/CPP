
#include <cstdio>
#include <set>
#include <thread>
#include <mutex>
#include <chrono>

class Request;

class Inventory
{
public:
    void add(Request *req)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_requests.insert(req);
    }

    void remove(Request *req)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_requests.erase(req);
    }

    void printAll() const;
    
private:
    //using RequestList=std::set<Request*>;
    //using RequestListPtr=std::shared_ptr<RequestList>;
    mutable std::mutex m_mutex;
    std::set<Request*> m_requests;
    //RequestListPtr m_requestsPtr;
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
        g_inventory.remove(this);
    }

    void process() 
    {
        std::lock_guard<std::mutex> lk(m_mutex);
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
    std::lock_guard<std::mutex> lk(m_mutex);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for(auto it = m_requests.begin(); it!=m_requests.end(); ++it)
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


