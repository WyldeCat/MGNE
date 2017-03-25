//
// packet_queue.hpp
//

#ifndef _PACKET_QUEUE_HPP_
#define _PACKET_QUEUE_HPP_

#include <boost/thread.hpp>

#include <mgne/packet.hpp>
#include <mgne/pattern/thread_job_queue.hpp>

namespace mgne {
class PacketQueue : public pattern::ThreadJobQueue<mgne::Packet> {
public:
  PacketQueue(size_t num_threads, void (*packet_handler)(Packet&)) 
    : num_threads_(num_threads)
    , packet_handler_(packet_handler)
  {
     
  }

  void StartProcessing(boost::thread_group& thread_group)
  {
    for (int i = 0; i < num_threads_; i++) {
      thread_group.create_thread(boost::bind(&PacketQueue::loop, this));
    }
  }

private:
  void loop()
  {
    Packet p;
    while (1) {
      while (!Pop(p)); 
      if (packet_handler_ != nullptr) packet_handler_(p);
    }
  }

  size_t num_threads_;
  void (*packet_handler_)(Packet&);
};
}

#endif
