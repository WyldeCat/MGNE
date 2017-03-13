#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

namespace mgne::pattern {
template <class T>
class Singleton {
protected:
  static T* instance;
  Singleton() { }
  ~Singleton() { }
 
public:
  T* get_instance() {
    return instance;
  }
  virtual void init(void* data) = 0;
  virtual void release() = 0;
};

}
template <class T>
T* mgne::pattern::Singleton<T>::instance;

#endif
