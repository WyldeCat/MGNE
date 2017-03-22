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
  PacketQueue(size_t num_threads) : num_threads_(num_threads)
  {
    
  }

  void StartProcessing(boost::thread_group& thread_group)
  {
  }

private:
  size_t num_threads_;
};
}

#endif
