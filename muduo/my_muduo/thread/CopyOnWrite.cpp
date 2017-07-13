
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <cassert>

using namespace std;

class Foo
{
public:
    void doit() const;
};

using FooList=std::vector<Foo>;
using FooListPtr=std::shared_ptr<FooList>;
FooListPtr g_foos;
std::mutex m_mutex;


void post(const Foo& f)
{
    cout << "post\n";
    std::lock_guard<std::mutex> lk(m_mutex);
    if ( !g_foos.unique() ){
        g_foos.reset(new FooList(*g_foos));
        cout<<"copy the whole list\n";
    }

    assert(g_foos.unique());

    g_foos->push_back(f);
}

void traverse()
{
    FooListPtr foos;
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        foos=g_foos;
    }
    
    assert(!g_foos.unique());
    for(auto it=foos->begin(); it != foos->end(); it++)
        it->doit();
}

void Foo::doit() const
{
    Foo f;
    post(f);
}

int main()
{
   g_foos.reset(new FooList);
   Foo f;
   post(f);

   traverse();
}










