//
// thread_job_queue.hpp
//
/*
Is std::mutex fair?
*/

#ifndef _MGNE_THREAD_JOB_QUEUE_
#define _MGNE_THREAD_JOB_QUEUE_

#include <iostream>

#include <deque>
#include <mutex>

namespace mgne::pattern {
template <typename T>
class ThreadJobQueue {
public:
  void Push(const T& t)
  {
    wqueue_mutex_.lock();
    wqueue_->push_front(t);
    wqueue_size_++;
    wqueue_mutex_.unlock();
  }

  bool Pop(T& t)
  {
    std::lock_guard<std::mutex> lock(rqueue_mutex_);
    if (wqueue_size_ == 0 && rqueue_size_ == 0) {
      return false;
    } else if (rqueue_size_ == 0) {
      swap();
    }
    t = rqueue_->front();
    rqueue_->pop_back();
    rqueue_size_--;
    return true;
  }

  int Rsize()
  {
    std::lock_guard<std::mutex> lock(rqueue_mutex_);
    if (rqueue_size_ == 0 && wqueue_size_ != 0) swap();
    return rqueue_size_;
  }

  bool Erase(T& t)
  {
    rqueue_mutex_.lock();
    wqueue_mutex_.lock();

    bool ret = (std::find(wqueue_->begin(), wqueue_->end(), t) != wqueue_->end())
      || (std::find(rqueue_->begin(), rqueue_->end(), t) != rqueue_->end());
    wqueue_->erase(std::remove(wqueue_->begin(), wqueue_->end(), t),
      wqueue_->end());
    rqueue_->erase(std::remove(rqueue_->begin(), rqueue_->end(), t),
      rqueue_->end());
    rqueue_mutex_.lock();
    wqueue_mutex_.lock();

    return ret;
  }

  ThreadJobQueue()
  {
    wqueue_size_ = 0;
    rqueue_size_ = 0;
    wqueue_ = &queues_[0]; 
    rqueue_ = &queues_[1];
  }

  ~ThreadJobQueue()
  {
  }

  void swap()
  { // It is certain that this thread obtains rqueue_mutex_,
    // when entering this function
    std::lock_guard<std::mutex> lock(wqueue_mutex_);
    if (wqueue_ == &queues_[0]) {
      wqueue_ = &queues_[1];
      rqueue_ = &queues_[0];
    } else {
      wqueue_ = &queues_[0];
      rqueue_ = &queues_[1];
    }
    rqueue_size_ = wqueue_size_;
    wqueue_size_ = 0;
  }

  std::deque<T>* wqueue_;
  std::deque<T>* rqueue_;

private:
  std::deque<T> queues_[2];
  std::mutex rqueue_mutex_;
  std::mutex wqueue_mutex_;
  size_t rqueue_size_;
  size_t wqueue_size_;
};
}

#endif
