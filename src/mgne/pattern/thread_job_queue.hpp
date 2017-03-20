//
// thread_job_queue.hpp
//
/*
Is std::mutex fair?
*/

#ifndef _THREAD_JOB_QUEUE_
#define _THREAD_JOB_QUEUE_

#include <queue>
#include <mutex>

namespace mgne::pattern {
template <typename T>
class ThreadJobQueue {
public:
  void Push(const T& t)
  {
    wqueue_mutex_.lock();
    write_queue_->push(t);
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
    t = read_queue_->front();
    read_queue_->pop();
    rqueue_size_--;
    return true;
  }

protected:
  ThreadJobQueue()
  {
    wqueue_size_ = 0;
    rqueue_size_ = 0;
    write_queue_ = &queues_[0]; 
    read_queue_ = &queues_[1];
  }

  ~ThreadJobQueue()
  {
    write_queue_->clear();
    read_queue_->clear();
  }

  void swap()
  { // It is certain that this thread obtains rqueue_mutex_,
    // when enter this function
    std::lock_guard<std::mutex> lock(wqueue_mutex_);
    if (write_queue_ == &queues_[0]) {
      write_queue_ = &queues_[1];
      read_queue_ = &queues_[0];
    } else {
      write_queue_ = &queues_[0];
      read_queue_ = &queues_[1];
    }
  }


  std::queue<T>* write_queue_;
  std::queue<T>* read_queue_;

private:
  std::queue<T> queues_[2];
  std::mutex rqueue_mutex_;
  std::mutex wqueue_mutex_;
  size_t rqueue_size_;
  size_t wqueue_size_;
};
}

#endif
