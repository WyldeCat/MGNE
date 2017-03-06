
template <class T>
class Singleton {
 protected:
  static T* instance;
  Singleton() { }
  ~Singleton() { }
 
 public:
  void get_instance() {
    return instance;
  }
  virtual void init() = 0;
  virtual void release() = 0;
};
