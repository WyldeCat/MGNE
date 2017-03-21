//
// session.hpp
//

#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include <mgne/packet.hpp>
#include <mgne/session_manager.hpp>
#include <mgne/packet_analyzer.hpp>

namespace mgne {
class BasicSession {
public:
  virtual void Send(const bool immediately, Packet* packet) = 0;

protected:
  BasicSession(int id) : id_(id) { }
  ~BasicSession() { }
};
}

namespace mgne::tcp {
class Session : public BasicSession {
public:
  Session(int id, PacketQueue& packet_queue,
    boost::asio::io_service& io_service)
    : BasicSession(id) 
    , socket_(io_service, packet_queue)
  { 
  }
  ~Session() { }

  virtual void Send(const bool immediately, Packet& packet)
  {
    PacketAnalyzer::Encrypt(packet);
    socket_.Send(immediately, packet.packet_size, packet.data_); 
    PacketAnalyzer::Decrypt(packet);
  }
  void Close() { /* TODO */ }
  void Receive() { socket_.Receive(); }

private:
  SessionManager& session_manager_;
  Socket socket_;
};
}

namespace mgne::udp {
class Session : public BasicSession {
public:
  Session(int id, SessionManager& session_manager) 
    : BasicSession(id)
    , session_manager_(session_manager)
    , server_socket_(session_manager.GetServer().GetServerSocket())
  { 
  }
  ~Session() { }

  virtual void Send(const bool immediately, Packet& packet)
  {
    PacketAnalyzer::Encrypt(packet);
    server_socket_.Send(immediately, packet.packet_size, packet.data_);
    PacketAnalyzer::Decrypt(packet);
  }

private:
  SessionManager& session_manager_;
  Socket& server_socket_;
};
}

#endif
