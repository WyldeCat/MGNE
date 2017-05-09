//
// thread_safe.hpp
//

#ifndef _MGNE_THREAD_SAFE_HPP_
#define _MGNE_THREAD_SAFE_HPP_

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
  bool TryLock()
  {
    return mutex_.try_lock();
  }
};
}

#endif
