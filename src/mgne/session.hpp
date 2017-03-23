//
// session.hpp
//

#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include <mgne/packet_analyzer.hpp>
#include <mgne/packet.hpp>
#include <mgne/socket.hpp>
#include <mgne/packet_queue.hpp>
#include <mgne/pattern/thread_safe_queue.hpp>

#include <boost/asio.hpp>

namespace mgne {
class BasicSession {
public:
  virtual void Send(const bool immediately, Packet& packet) = 0;

protected:
  BasicSession(int id) : id_(id) { }
  ~BasicSession() { }
  int id_;
};
}

namespace mgne::tcp {
class Session : public BasicSession {
public:
  Session(int id, PacketQueue& packet_queue,
    boost::asio::io_service& io_service,
    pattern::ThreadSafeQueue<int>& thread_safe_queue)
    : BasicSession(id) 
    , socket_(io_service, packet_queue, thread_safe_queue)
  { 
  }
  ~Session() { }

  void Send(const bool immediately, Packet& packet)
  {
    PacketAnalyzer::Encrypt(packet);
    socket_.Send(immediately, packet.packet_size_, packet.data_); 
    PacketAnalyzer::Decrypt(packet);
  }
  void Close() { /* TODO */ }
  void Receive() { socket_.Receive(); }

private:
  Socket socket_;
};
}

namespace mgne::udp {
class Session : public BasicSession {
public:
private:
};
}

#endif
