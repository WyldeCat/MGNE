#include <iostream>

#include "singleton.hpp"

class A: public mgne::pattern::Singleton<A> {
public:
  int n_;
  A() { }
  A(int n):n_(n) { }
  void init(void* data)
  {
    int k = *(int*)data;
    instance = new A(k);
  }
  void release()
  {
  }
};

int main()
{
  A a, b;
  int k = 3;
  a.init((void*)&k);

  std::cout << b.get_instance()->n_ << std::endl;

  return 0;
}
