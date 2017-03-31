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
    pattern::ThreadSafeQueue<int>& available_sessions)
    : BasicSession(id) 
    , socket_(io_service, packet_queue, available_sessions, id_)
  { 
  }
  ~Session() { }

  void Send(const bool immediately, Packet& packet)
  {
    socket_.Send(immediately, packet.packet_size_, packet.packet_id_,
      packet.data_->data()); 
  }

  void Close() { socket_.Close(); }
  void Receive() { socket_.Receive(); }
  Socket& GetSocket() { return socket_; }

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
