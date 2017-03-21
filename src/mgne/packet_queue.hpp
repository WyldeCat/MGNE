//
// packet_queue.hpp
//

#ifndef _PACKET_QUEUE_HPP_
#define _PACKET_QUEUE_HPP_

#include <mgne/pattern/thread_job_queue.hpp>
#include <mgne/server.hpp>
#include <mgne/packet.hpp>

namespace mgne {
class PacketQueue : public pattern::ThreadJobQueue<Packet> {
public:
  PacketQueue(size_t num_threads, BasicServer& server)
  {
  }

private:
  int num_threads_;
  BasicServer
};
}

#endif
