//
// thread_safe_queue.hpp
//

#ifndef _THREAD_SAFE_QUEUE_
#define _THREAD_SAFE_QUEUE_

#include <mutex>
#include <queue>

namespace mgne::pattern {
template <typename T>
class ThreadSafeQueue {
public:
  ThreadSafeQueue() { }
  ~ThreadSafeQueue() { }
  void Push(const T& t)
  {
    mutex_.lock();
    queue_.push(t);
    mutex_.unlock();
  }

  bool Pop(T& t)
  {
    mutex_.lock();
    if (queue_.empty()) return false;
    t = queue_.front();
    queue_.pop(); 
    mutex_.unlock();
    return true;
  }


  bool Empty()
  {
    // FIXME
    std::lock_guard<std::mutex> lock(mutex_);  
    return queue_.empty();
  }

  T Front() 
  { 
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.front(); 
  }

private:
  std::queue<T> queue_; 
  std::mutex mutex_;
};
}

#endif
