//
// thread_safe.hpp
//

#ifndef _THREAD_SAFE_HPP_
#define _THREAD_SAFE_HPP_

#include <mutex>

namespace mgne::pattern {
class ThreadSafe {
protected:
  std::mutex mutex_;
  void Lock()
  {
    mutex_.lock();
  }
  void Unlock()
  {
    mutex_.unlock();
  }
};
}

#endif
