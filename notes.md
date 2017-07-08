# c++ primer notes



## 类的设计


1. 使用class和struct 定义类唯一的区别就是默认的访问权限

2. 类可以允许其他类或者函数访问它的非公有成员，方法是令其他类或者
   函数成为它的友元

3. 使用=default告诉编译器合成默认构造函数

4. mutable,可变数据成员，即使const成员函数也可以改变一个可变成员的值

5. 类内初始值: 必须以符号=或者花括号表示

   ```c++
    class Windows_mgr{
      private:
     std::vector<Screen> screens{Screen(24,80,' ')} ;
    };

   struct Sales_data{
     std::string bookNo ;
     unsigned units_sold = 0;
     double revenue = 0.0 ;
   }
   ```

   ​

6. 前向声明：把函数的声明和定义分开，只能在有限条件下使用，可以定义指向这种类型的指针或引用； 或者 声明 以不完全类型作为参数或者返回类型的函数。

7. 作用域： 

   ```c++
   int height;
   class Screen{
     public:
        using pos = std::string::size_type;
        void dummy_fcn(pos height){
          cursor = width * height // 使用的是参数height;
        }
     
        void dummy_fcn1(pos height)
          {
           // 强制使用成员变量 
            cursor = width * this->height; 
            // or
            // cursor = width * Screen::height; 
          }
     private:
        pos cursor = 0;
        pos height=0, width=0;
   }

   void Screen::dummy_fcn2(pos height)
     {
       cursor = width * ::height; // 使用全局height
     }
   ```

8. 构造函数：

   8.1 如果成员是const , 引用，或者某种未提供默认构造函数的类类型，必须通过构造函数初始值列表为这些成员提供初值。

   ```c++
   class ConstRef{
     public:
     ConstRef(int i);
     private:
     int i;
     const int ci;
     int &ri;
   };

   ConstRef::ConstRef(int ii): i(ii), ci(ii), ri(ii) {}
   ```

   8.2 成员初始化顺序与它们在类定义中的出现顺序一致。

   8.3 委托构造函数(delegating constructor)：

   8.4 抑制构造函数定义的隐形转换

   explicit: 将构造函数声明为explicit加以阻止

   只对一个实参的构造函数有用，需要多个实参的构造函数不能执行隐形转换，

   无需对这些构造函数指定为explicit

   ```c++
   class Sales_data{
    public:
        Sales_data() = default; 
        Sales_data(const std::string &s, unsigned n): bookNo(s),  
          units_sold(n){}
        explicit Sales_data(const std::string &s) : bookNo(s){}
        //
        ...
   }; 
   ```




9.静态成员的初始化

10. 一个类通过定义五种特殊的成员函数来控制这些操作中：

-  拷贝构造函数(copy constructor)

- 拷贝赋值构造函数(copy-assignment constructor)

- 移动构造函数(move constructor)

- 移动赋值构造函数(move-assignment constructor)

- 析构函数

   析构函数有一个函数体和一个析构部分。首先执行函数体，然后销毁成员，成员按初始化顺序的逆序销毁

11. 使用=default

    在类内用=default修饰成员的声明时，合成的函数将隐式地声明为inline

    ，如果不想inline，应该只对成员的类外定义使用=default

    ```c++
    class Sales_data{
     public:
      Sales_data() = default;
      Sales_data(const Sales_data&) = default;
      Sales_data& opertator=(const Sales_data&);
      ~Sales_data() = default;
    };

    Sales_data& Sales_data::operator=(const Sales_data&) =default;
    ```

    ​

12. 使用=delete

    通过将拷贝构造函数和拷贝赋值构造函数运算符定义为删除的函数(deleted function)来阻止拷贝

    或者

    声明为private 来阻止拷贝

    ```c++
    struct NoCopy
      {
        NoCopy() = default; 
        NoCopy(const NoCopy &) = delete; //阻止拷贝
        NoCopy& operator=(const NoCopy&) = delete; //阻止赋值
      };
    ```

13. 移动构造函数，移动赋值运算符

    完成资源移动，还要保证移后原对象处于销毁且无害的状态。一旦资源完成，原对象必须不再指向被移动的资源，这些资源的所有权意境归属新创建的对象。

    ```c++
    StrVec::StrVec(StrVec &&s) noexcept // 移动操作不应跑出任何异常
      : elements(s.elements), first_free(s.first_free)
        {
          s.elements = s.first_free = nullptr; ---> no self-assignment check needed
        }
      
    ```

    ```
    StrVec &StrVec::operator=(StrVec &&rhs) noexcept
      {
        if (this != &rhs){     -----> 需要做self-assignment 检查
          free() ; // 释放已由元素
          elements = rhs.elements;
          first_free = rhs.elements;
          cap = rhs.cap;
          //将rhs置于可析构状态
          rhs.elements = rhs.first_free = rhs.cap = nullptr;
          return *this;
        }
     }
    ```

    移动后，原对象必须可析构

    - 合成移动构造函数，移动赋值函数

      只有当一个类没有定义任何自己版本的拷贝控制成员，且类的每个非static数据成员都可以移动时，编译器才会为它合成移动构造函数或移动赋值运算符.

    - 移动操作永远不会隐式定义为删除的函数

    ​        当显式要求编译器生成=default移动操作，且编译器不能移动所有成员，则编译器会将移动操作定义为删除的函数。

    - 定义了一个移动构造函数或移动赋值运算符的类必须也定义自己的拷贝构造操作。否则成员默认地定义为删除的。

    ​

    函数重载

    区分移动和拷贝的重载函数通常由有一个版本接受一个const T&，另一个版本接受T&&。

    ```c++
    class StrVec{
      public:
      void push_back(const std::string&) ; // 拷贝元素
      void push_back(std::string&&) ; //移动元素
      ...
    }；
      
    void StrVec::push_back(const string& s){
      chk_n_alloc();
      alloc.construct(first_free++, s);
    }

    void StrVec::push_back(string &&s)
      {
        chk_n_alloc();
        alloc.construct(first_free++, std::move(s));
      }
    ```

​      引用限定符(reference qualifier)

引用限定符可以是&或&&，分别指出this可以指向一个左值或右值。类似const限定符，引用限定符只能用于成员函数，且必须同时出现在函数的声明和定义中。

```
class Foo{
 public:
  Foo sorted() &&; //可用于可改变的右值
  Foo sorted() const &; //可用于任何类型的Foo
  ...
 private:
   vector<int> data;
};

//本对象是右值，原地址排序
Foo Foo::sorted() && 
{
  sort(data.begin(), data.end());
  return *this;
}

//本对象时const或是一个左值
Foo Foo::sorted() &
{
  Foo ret(*this);
  sort(ret.data.begin(), ret.data.end());
  return ret;
}
```



14. 使用拷贝和交换的 赋值运算符

    ```c++
    //通常赋值运算符定义如下
    a) 
      HasPtr& HasPtr::operator=(const HasPtr &rhs)
      {
        if(this != &rhs){
          auto newp = new string(*rhs.ps); ===> 放在前面，异常安全！
          delete ps;
          ps = newp;
          i = rhs.i;
        }
        return *this;
      }

    //使用 swap-and-copy 技术
    //自动异常安全的，并切能够处理异常
    b)
      HasPtr& HasPtr::operator=(HasPtr rhs) ----> note: param pass by value
      {
        //交换左侧运算符对象和局部变量rhs的内容
        swap(*this, rhs);
        return *this;
      }
    ```


    note: b)这种不能可以和 移动赋值操作符 一起声明，因为 rhs 会产生 temp object, 会被当成rvalue. 
      造成overload 冲突。
      C++ ambiguous overload for ‘operator=
    ​```





15. 运算符重载

    - 下标运算符

    - 递增递减

      ```c++

      class StrBlobPtr{
      public:
        StrBlobPtr& operator++();  //前置运算符
        StrBlobPtr& operator--();
        StrBlobPtr operator++(int); //后置运算符
        StrBlobPtr operator--(int); 
      };
      ```

      显式调用后置预算符：

      ```c++
       StrBlobPtr p(a1);
       p.operator++(0); //后置
       p.operator++(); // 前置
      ```

      ​


    - 如果一个类包含下标运算符，则通常会定义两个版本。一个返回普通引用，另一个是类的常量成员并返回常量引用

    - ```c++
      class StrVec{
      public:
        std::string& operator[](std::size_t n){return elements[n];}
        const std::string& operator[](std::size_t n){return elements[n];}
      private:
        std::string *elements;
      }；         
      ```
    
    - 函数调用运算符: 如果定义了调用运算符，该类的对象称作函数对象。
    
      ```c++
      struct absInt{
        int operator() (int value) const {
          return value < 0 ? -value : value ;
        }
      };
      ```



16. 可调用对象

    1） 函数

    2） 函数指针

    3） lambda表达式

    4）bind创建的对象

    5）重载了函数调用运算符的类

    =====> 如何统一 ？ 接口调用，使用std::function 类型！====> 泛化函数对象

17. 类型转换运算符

    将一个类类型的值转换成其他类型

    operator type() const;

    ```c++
    class SmallInt{
    public:
        SmallInt(int i=0): val(i)
        {
          
        }
        
        operator int() const{return val;}
        
    private:
        int val;
    };
    ```

    ​


## 面向对象

1. final -> 防止继承

   ```c++
   class NoDerived final {}; 

   class Base {};
   class Last final: public Base{} ;// Last不能做基类
   class Bad: NoDerived{};
   class Bad2: Last {};
   ```

2. 存在继承关系的类型之间的转换规则：

   - 从派生类向基类的类型转换只对指针或引用有效
   - 基类向派生类不存在隐式类型转换，可以使用dynamic_cast 或者 
   - 和任何其他成员一样，派生类向基类的类型转换也可能会由于访问受限而变得不可能



3. 虚函数

   一旦某个函数被申明为虚函数，则在所有派生类中它都是虚函数

   final & override

   ```c++
   struct B{
     virtual void f1(int) const;
     virtual void f2();
     void f3();
   };

   struct D2: public B{
     void f1(int) const override;  // correct, f1 与基类中的f1匹配
     void f2(int) override; // error: 基类中没有f2(int)型
   }；
     
   struct D3 : public B {
     void f1(int) const final;//不允许后续的其他类覆盖f1(int)
   };

   struct D4: D3 {
     void f2(); // correct, from B
     void f1(int); //incorrect, f2 could not be overrided
   };
   ```



4. 抽象基类

   含有（未经覆盖直接继承）纯虚函数的类是抽象基类。抽象基类负责定义接口， 不能直接创建一个抽象基类的对象。

1) 访问控制与继承

公有继承，私有集成，受保护继承：公有继承接口，私有继承实现。

```c++
class Base{
  public:
      void pub_mem();
  protected:
      int prot_mem;
  private:
      char priv_mem;
};

struct Pub_Derv: public Base{
  int f() {return prot_mem;}
  char g() {return priv_mem;}
};

struct Priv_Derv: private Base{
  int f1() const {return prot_mem;}
};
```

​       

2) 友元与继承：

友元关系不能继承，基类的友元访问派生类成员时不具特殊性，派生类的友元不能随意访问基类的成员

3）改变个别成员的可访问性： 使用using

```c++
class Base{
  public:
     std::size_t size() const {return n;}
  protected:
     std::size_t n;
};

class Derived: private Base{
  public:
     using Base::size ; // 保持size() 的访问权限
  protected:
     using Base::n;
};
```

4）继承中的类作用域



​       



## 右值引用

右值引用： 只能绑定到一个将要销毁的对象。因此，可以自由地将一个右值引用的资源“移动”到另一个对象中。

左值持久，右值短暂：

- 右值所引用的对象将要被销毁
- 该对象没有没有其他用户





## 模版

- 为了生成一个实例化版本，编译器需要掌握函数模版或类模版成员函数的定义。因此，模版的头文件通常包含声明和定义



- c++11中允许将模版类型参数声明为友元

```c++
template<typename Type> class Bar
  {
    friend Type; 
     ....
  };

Sales_data 将成为 Bar<Sales_data>的友元

```



- 类作用域

  当处于一个类模版作用域中时，编译器处理模版自身引用时就好像我们已经提供了与模版参数匹配的实参一样。

  Note: 在类外仍然需要使用类模版名

  ```c++
  template <typename T> class BlobPtr{
   public:
    BlobPtr(): curr(0){}
    BlobPtr(BlobPtr<T>& a, size_t sz=0): wptr(a.data), curr(sz){}
    T& operator*() const {
      auto p = check(curr, "dereference past end");
      return (*p)[curr];
    }
    BlobPtr& operator++();//前置
    BlobPtr& operator--();
   private:
    std::shared_ptr<std::vector<T>> check(std::size_t, const std::string&) const;
    // 保存一个weak_ptr表示底层vector可能被销毁
    std::weak_ptr<std::vector<T>> wptr;
    std::size_t curr ; // 
  };
  ```


  template <typename T>
    BlobPtr<T> BlobPtr<T>::operator++(int){
      BlobPtr ret = *this; 
      ++*this;
      return ret;
    }
  ```

  ​

- c++11中类型别名：

c++11 为类模版定义一个类型别名

```c++
template<typename T> using Twin = pair<T,T>;
twin<string> authors;

template<typename T> using partNo = pair<T, unsigned>;
partNo<string> books;
partNo<Student> kids;
```


## Overview of C++11 from Scott Meyers



## C++ 11 特性（深入理解）

- 类型推导

  auto 

  decltype:

  - 规则1：exp是标示符，类访问表达式，decltype(exp)和exp的类型一致
  - 规则2:  exp是函数调用，decltype(exp)和返回值的类型一致
  - 规则3:   若exp是一个左值，decltype(exp)是exp类型的左值引用，否则与exp类型一致。

  auto + decltype : 返回类型后置(trailing-return type, 跟踪返回类型) :

  返回类型后置，是为了解决函数返回值类型推导依赖于参数而导致难以确定返回值类型的问题。

  ```c++
  template <typename T, typename U>
    auto add(T t, U u) -> decltype(t+u)
    {
      return t+u;
    }
  ```


  int & foo(int& i);
  float  foo(float& f);

  template<typename T>
    auto func(T& val) -> decltype(foo(val))
    {
      return foo(val);
    }
  ```

- 模版别名:

  定义一个固定以std::string为key的map:

  ```c++
  c++98/03:

  template <typename Val>
    struct str_map
      {
        typedef std::map<std::string, Val> type;
        
      };

     //...
  str_map<int>::type map1; 


  c++11:
  template<typename Val>
  using str_map_t = std::map<std::string, Val>;
  //...
  str_map_t<int> map2；
    
  ```

  

- using vs typedef

  using 别名语法覆盖了typdef的全部功能。

  ```c++
  // 类型别名
  typdef unsigned int uint_t; 
  using uint_t = unsigned int;

  // map
  typedef std::map<std::string, int> map_int_t; 
  using map_int_t = std::map<std::stirng, int>;

  //函数指针
  typedef void (*func_t)(int, int);
  using func_t = void(*)(int, int);

  //模版别名
  /*c++ 98/03*/
  template<typename T>
    struct funct_t
      {
        typedef void(*type)(T, T);
      };

  fun_t<int>::type xx_1;
  ```


  /*c+=11*/
  template<typename T>
  using func_t = void (*) (T, T);
  func_t<int>  xx_2;
  ```

- 列表初始化／统一初始化

  统一各种对象的初始化方式

  聚合类型 vs 初始化列表

  聚合类型：

  1） 类型是一个普通数组

  2） 类型是一个类(class, struct, union)  且

         无用户自定义的构造函数

  	无私有或保护的非静态数据成员

  	无基类

  	无虚函数

  	不能有{}和= 直接初始化的非静态数据成员.

  ```c++
  struct Foo{
    int x;
    double y;
    int z;
    Foo(int, int){}
  };
  Foo foo{1, 2.5, 3}; // error,试图用构造函数初始化，参数不符
  ```

  3) 任意长度的初始化列表

  使用 std::initializer_list 构造函数，拥有任意长度初始化的能力

  ```c++
  class Foo
    {
      public:
    Foo(std::initializer_list<int>) {}
    };

  Foo foo={1,2,3,4,5};
  ```

  std::initializer_list 特点：

-   一个轻量级的容器类型，内部定义了iterator等容器必需的 概念
  - std::initializer_list<T>而言，可以接收人意长度的初始化列表，但要求元素必需是同种类型T(或转换为T)
  - 三个成员接口： size(), begin(),  end()
  - 只能被整体初始化或赋值。

  ```c++
  void func(std::initializer_list<int> l)
    {
      for(auto it = l.begin(); it != l.end(); ++it)
        std::cout<<*it<<std::endl;
    }

  int main()
    {
      func({});
      func({{1,2,3,4,5}});
      return 0;
    }
  ```

  - 使用列表初始化来防止类型收窄

    ```c++
    int a = 1.1; //ok
    int b = {1.1}; //error
    ```

    ​

- Range For

  ​

  ```c++
  vector<int> IntVec{1,2,3,4,5,6};
  for(auto n : IntVec)
    {
      std::cout<< n << std::endl;
    }

  // modify
  for(auto &n : IntVec)
    {
      n++;
    }

  // access with reference but not modification
  for(const auto &n : IntVec)
    {
      std::cout<< n << std::endl;
    }
  ```

  Note: 同普通的for循环一样，在迭代时修改容器可能引起迭代器失效。

  ```c++
  #include <iostream>
  #include <vector>

  int main()
  {
      std::vector<int> arr{1,2,3,4,5};
      for(auto n : arr)
      {
          std::cout<< n <<std::endl;
          arr.push_back(0); // 可能导致迭代器失效
          arr.push_back(0); // 可能导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
          arr.push_back(0); // 导致迭代器失效
      }
  ```

  ​

- std::function 和 std::bind

  可调用对象(Callable Objects)

  - 一个函数指针

    void func(){ … }

  - 一个具有operator()成员函数的类对象

    struct Foo{

      void operator() (void) { …. } 

    }

  - 一个可被转换为函数指针的类对象

  - 一个类成员(函数)指针

  std::function - 可调用对象的包装器， 类模版，通过指定模版参数，可以用统一的方式处理函数，函数对象，函数指针，并允许保存和延迟执行。

  用途：

  1. 函数包装器

  2. std::functino可以取代函数指针，可以保存函数延迟执行，可以用作回调函数

  3. 作用函数入参

     ```c++
     #include <iostream>
     #include <functional>

     void func(void)
     {
         std::cout<< __FUNCTION__ << std::endl;
     }

     class Foo{
     public:
         static int foo_func(int a)
         {
             std::cout<< __FUNCTION__ << std::endl;
             return a;
         }
     };
     ```


     class Bar
     {
     public:
         int operator()(int a)
         {
             std::cout<< __FUNCTION__ << std::endl;
             return a;
         }
    
     };
    
     class A
     {
         std::function<void(void)> callback;
     public:
         // error here :
         //function_test.cpp:35:14: note: candidate constructor not viable:
         //  no known conversion from 'T' to 'std::function<void ()> &'
         //  for 1st argument
         //explicit A(std::function<void(void)> &f): callback(f) {}
         // OK: pass value by const reference
         // explicit A(const std::function<void(void)> &f): callback(f) {}
         explicit A(std::function<void(void)> f): callback(f) {}
    
         void notify(){
             callback();
         }
     };
    
     class T
     {
     public:
         void operator()(void)
         {
             std::cout<< "T "<<__FUNCTION__ <<std::endl;
         }
     };
    
     void call_when_even(int x, const std::function<void(int)> &f)
     {
         if (!(x&1))
         {
             f(x);
         }
     }
    
     void printvalue(int x)
     {
         std::cout<< x << " ";
     }


     int main(void)
     {
         std::function<void(void)> f = func ;
         f();
    
         std::function<int(int)> f2 = Foo::foo_func; 
         std::cout << f2(2) << std::endl;


         Bar bar;
         std::function<int(int)> f3 = bar;
         std::cout<< f3(3) << std::endl;


         /// as the callback
         T t;
         A aa(t);
         aa.notify();
    
        // as the param
         for(int x=0; x < 10; x++)
         {
             call_when_even(x, printvalue);
         }
     }
     ```

  std::bind 用来将可调用对象与其参数一起进行绑定。绑定后的结果使用std::function进行保存，并延迟到需要的时候调用。

  **std::function和std::bind的配合，所有可调用对象有了统一的操作方法

  作用：

  1） 将可调用对象与其参数一起绑定成一个仿函数

  2） 将多元（参数为n, n>1)可调用对象转成一元或者(n-1)元可调用对象，即只绑定部分参数。

  3)   结合 std::placeholders::_1, std::placeholders::_2, ..., std::placeholders::_N , 来绑定参数

  ```c++
  #include <iostream>
  #include <functional>

  class A
  {
  public:
      int i_ = 0;

      void output(int x, int y)
      {
          std::cout<< x << " " << y << std::endl;
      }
  };

  int main(void)
  {
      A aa;
      const int val = 1000;
      std::function<void(int)> f = std::bind(&A::output, &aa, std::placeholders::_1, 
              val);
      f(1);
  }
  ```

  ​


- lambda表达式

  lambda表达式语法：

  [capature] {param} opt -> ret { body ;} ;

  capature: 捕获列表；

  params: 参数表

  opt: 函数选项

  ret: 返回值类型

  body: 函数体 

  ```c++
  auto f = [](int a) -> int {return a + 1;};
  ```

  - [] 不捕获任何变量

  - [&] 捕获外部作用域中所有变量，并作为引用在函数体中使用(按引用捕获)

  - [=] 捕获外部作用域中所有变量，并作为副本在函数体重使用(按值捕获)

  - [=, &foo] 按值捕获外部作用域中所有变量，并按引用捕获foo变量

  - [bar] 按值捕获bar变量，同时不捕获其他变量

  - [this] 捕获当前类中的this指针，让lambda表达式拥有和当前类成员函数同样的访问权限，

    如果使用了 & 或 = , 就默认添加此项。 捕获this的目的是可以在lambda中使用当前类的成员和

    成员变量。

    ```c++
    #include <iostream>
    #include <vector>
    #include <functional>
    ```



    int main()
    {
        std::vector<int> vec= {1,2,3,4,5,6,7,8,9,10};
        int even_count = 0;
    
        // lambda as the callable object
        auto f = [&even_count](int val){ if (!(val&1)){even_count++;}};
    
        // lambda as the function
        std::function<void(int)> f1 = [&even_count](int val) { if (!(val&1)){even_count++;}};
    
        // lambda as the function pointer
        using func_t = void(*)(int);
        func_t f2 = [](int a){std::cout<<"a="<<a<<std::endl;};
    
        for(auto v: vec)
        {
            f1(v);
        }
        std::cout<< even_count << std::endl;
    
        f2(even_count);
    }
    ​```
    
    ​

- 右值引用 & 移动语义 & 完美转发

  - 左值 vs 右值

    左值：表达式结束时依然存在的持久对象

    右值：表达式结束时不在存在的临时对象

    区分方法：对表达式用&符取地址，则为左值；否则为右值

  - 右值引用， &&的特性

    对右值进行引用的类型, 因为右值不具名，我们只能通过引用方式找到它。

    临时对象，通过右值引用，使得生命周期被延长，与右值引用类型变量的生命周期一样。

    **常量左值引用是一个万能引用类型，可以接受  左值／右值／常量左值／常量右值**

    T&& : 并不一定表示右值

    ```c++
    template<typename T>
    void f(T&& param);

    f(10); // 10是右值
    int x = 10;
    f(x); // x是左值
    ```

    **universal reference**:  只有当发生类型推断时，&&才是一个universal reference. 

    auto&&或函数参数类型自动推导的T&&是一个未定的引用类型，称为universal reference. 它可能是

    左值引用也可能是右值引用类型，取决于初始化的值类型。

    ```c++
    template<typename T>
    void f(T&& param); //这里的T需要推导，所以&&是一个universal references

    tempalte<typename T>
    class Test{
      ...
      Test(Test&& rhs); // 已经定义了一个特定类型， &&是一个右值引用
    };
    ```

    ​

  - 移动构造函数 & 移动赋值函数

  - move 语义

    std::move方法唯一的功能是将左值强制转换为右值引用，方便应用移动语义.

    std::forward: 不管参数是T&&这种未定的引用还是明确的左值引用或者右值引用，都会

    按照参数本来的类型转发

    ```c++
    template<typename T>
    void forwardValue(T&& val)
      {
        processVal(val); //右值变成左值
      }

    template<typename T>
    void forwardValue(const T& val)
      {
        processVal(val); //参数都变成常量左值引用
      }

    solution:
    template<typename T>
      void forwardVal(T&& val)
      {
        processVal(std::forward<T>(val));
      }
    ```

    ​

  - 右值引用 + 完美转发  + 可变参数模版 ， 可以构造万能函数包装器

    ```c++
    template<class Function, class... Args>

    inline auto FuncWrapper(Function && f, Args &&… args) -> decltype(f(std::forward<Args>(args)...))
      {
        return f(std::forward<Args>(args)...)
      }
    ```

    ​

- 智能指针： shared_ptr/ unique_ptr/ weak_ptr

  - shared_ptr: 共享的智能指针

    1. 初始化： 构造函数／ std::make_shared<T>  辅助函数/ reset 

       ```c++
       //shared_ptr初始化
       std::shared_ptr<int> p(new int);
       std::shared_ptr<int> p2 = p;
       std::shared_ptr<int> ptr ;
       ptr.reset(new int(1));

       if(ptr)
         {
           std::cout<<"ptr is not null"<<std::endl;
         }
       ```

    2. 获取原始指针:  get 

       ```c++
       std::shared<int> ptr(new int(1));
       int *p = ptr.get();
       ```

    3. 指定删除器

       ```c++
       std::shared_ptr<int> p(new int, [](int *p){delete p;});

       //当用shared_ptr管理动态数组时，需要指定删除器，因为std::shared_ptr默认删除器不支持数组对象
       std::shared_ptr<int> p(new int[10], [](int *p){delete []p;});

       //封装make_shared_array
       template<typename T>
         shared_ptr<T> make_shared_array(size_t size)
         {
           return shared_ptr<T>(new T[size], default_delete<T[]>());
         }

       ```

    4. 不要做的。。。

       1）不要一个原始指针初始化多个shared_ptr

       2).  不要在函数实参中创建shared_ptr

       ```c++
       //c++的函数参数的计算顺序在不同编译器不同的调用约定下可能不一样，一般是从右到左，
       //也可能是从左到右。可能的过程是先 new int, 然后调用 g(); 如果g()抛出异常，而
       // shared_ptr<int> 还没有创建，则会发生内存泄漏
       function (shared_ptr<int>(new int), g());

       //正确写法
       shared_ptr<int> p(new int);
       f(p,g());
       ```

       3) 通过shared_from_this()返回指针，不要将this指针作为shared_ptr返回出来

       正确做法是：让目标类通过派生std::enable_shared_from_this<T>类，然后使用基类的成员函数shared_from_this来返回this的shared_ptr. 

       ```c++
       class A: public std::enable_shared_from_this<A>
         {
           std::shared_ptr<A> GetSelf()
             {
               return shared_from_this();
             }
         };

       std::shared_ptr<A> spy(new A); // enable_shared_from_this只能用于堆对象
       std::shared_ptr<A> p = spy->GetSelf(); // OK.
       ```

       4) 避免循环引用

       ```c++
       #include <iostream>
       #include <memory>
       struct B;
       struct C;
       struct B{
           std::shared_ptr<C> cptr;
           ~B(){std::cout<<"~B"<<std::endl;}
       };

       struct C{
           std::shared_ptr<B> bptr;
           ~C() {std::cout<<"~C"<<std::endl;}
       };

       int main()
       {
       // circular reference
          std::shared_ptr<B> bp(new B);
          std::shared_ptr<C> cp(new C);
          bp->cptr = cp;
          cp->bptr = bp; 
         
         return 0;
       }
       // destructors never called
       ```

  - unique_ptr独占的智能指针

    独占型指针，unique_ptr指定删除器的类型 

    ```c++
    std::shared_ptr<int> ptr(new int[1], [](int *p){delete p;}); //正确
    std::unique_ptr<int> ptr(new int[1], [](int *p){delete p;}); //错误

    std::unique_ptr<int, void(*)(int*)> ptr(new int[1], [](int *p){delete p;});//正确
    ```

    ​

    ```c++
    #include <iostream>
    #include <functional>
    #include <memory>

    struct MyDeleter
    {
      void operator()(int)
        {
          std::cout<<"delete"<<std::endl;
          delete p;
        }
    };

    int main()
    {
        std::unique_ptr<int, MyDeleter> p(new int);
        return 0;
    }
    ```

    ​

  - weak_ptr弱引用的智能指针，是shared_ptr的助手，只是监视shared_ptr管理的资源是否释放，本身并不操作或者管理资源。用于解决shared_ptr循环引用或返回this指针的问题。

- 多线程

  - 创建

    ```c++
    #include <iostream>
    #include <thread>
    #include <vector>

    std::vector<std::thread> g_list;
    std::vector<std::shared_ptr<std::thread>> g_list2;

    int i = 0;

    void func(int i)
    {
        std::cout<<"id: "<< i  << std::endl;
    }
    void CreateThreads()
    {
        std::thread t(func, i++);
        g_list.push_back(std::move(t));

        g_list2.push_back(std::make_shared<std::thread>(func, i++));
    }
    ```


    int main()
    {
        CreateThreads();
    
        for(auto &t : g_list)
        {
            t.join();
        }
    
        for(auto& t : g_list2)
        {
            t->join();
        }
    
        std::thread t1([](int a, int b){std::cout<<"a: "<<a<<" b:"<<b<<std::endl;}, 10, 20);
        t1.join();
    
        std::thread t2(std::bind(func, 2000));
        t2.join();
    
        return 0;
    }
    ​```
    
    ​

-   互斥量 & 条件变量

    std::mutex: 独占互斥量

    std::mutex m;

    std::lock_guard<std::mutex> lk(m);  // RAII 

    //condtion variable

    ```c++
    ／／队列是否为空
    std::unique_lock<std::mutex> lk(m_mutex);
    while(IsEmpty())
    {
      m_notEmpty.wait(m_mutex);
    }

    //等价写法
    std::unique_lock<std::mutex> lk(m_mutex);
    m_notEmpty.wait([this]{return !IsEmtpy();});
    ```

    ​​

- STL:

    vector: 

    set:

    map:

    stack<int>:

    queue<int>:

    priority_queue<int>:  push/pop/top

