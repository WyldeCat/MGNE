//
// session.hpp
//

#ifndef _MGNE_SESSION_HPP_
#define _MGNE_SESSION_HPP_

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
  Session(int id, PacketQueue& packet_queue, void (*error_handler)(int),
    boost::asio::io_service& io_service,
    pattern::ThreadSafeQueue<int>& available_sessions)
    : BasicSession(id) 
    , socket_(io_service, packet_queue, error_handler, available_sessions, id_)
  { 
  }

  ~Session() 
  {
    // Close Socket
  }

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
  Session(int id, boost::asio::ip::udp::endpoint& endpoint,
    Socket* socket)
    : BasicSession(id)
    , endpoint_(endpoint)
    , socket_(socket) 
  {
    // How about using 8 byte in id? 
  }
  ~Session() { }

  void Send(const bool immediately, Packet& packet)
  {
    socket_->Send(immediately, packet.packet_size_, packet.packet_id_,
      packet.data_->data(), endpoint_);
  }

  void Close() { socket_->Close(); }
  void Receive() { socket_->Receive(); }
  Socket* GetSocket() { return socket_; }

private:
  boost::asio::ip::udp::endpoint endpoint_;
  Socket* socket_;
};
}

#endif
