//
// session_manager.hpp
//

#ifndef _SESSION_MANAGER_HPP_
#define _SESSION_MANAGER_HPP_

#include <vector>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <mgne/session.hpp>
#include <mgne/socket.hpp>
#include <mgne/packet_queue.hpp>
#include <mgne/pattern/thread_safe_queue.hpp>
#include <mgne/protocol.hpp>

namespace mgne::tcp {
class SessionManager {
public:
  SessionManager(size_t capacity, size_t num_threads,
    boost::asio::ip::tcp::endpoint& endpoint, PacketQueue& packet_queue)
    : capacity_(capacity) 
    , endpoint_(endpoint)
    , num_threads_(num_threads)
    , io_services_(num_threads)
    , packet_queue_(packet_queue)
  {
    for (int i = 0; i < capacity_; i++) {
      sessions_.push_back(nullptr); 
      available_sessions_.Push(i);
    }
    for (auto& io_service : io_services_) {
      works_.push_back(boost::asio::io_service::work(io_service)); 
    }
  }

  ~SessionManager() 
  {
    for (int i = 0; i < capacity_; i++) delete sessions_[i];
    if (acceptor_ != NULL) delete acceptor_;
  }

  void StartAccepting(boost::thread_group& thread_group)
  {
    acceptor_ = new boost::asio::ip::tcp::acceptor(io_services_.front(), 
      endpoint_);
    SessionManager::accept();
    for (int i = 0; i < num_threads_; i++) {
      thread_group.create_thread(boost::bind(&boost::asio::io_service::run,
        &io_services_[i]));
    }
  } 

  void Send(const int session_id, Packet &packet) const {
    sessions_[session_id]->Send(false, packet);
  }

private:
  bool accept()
  {
    if (available_sessions_.Empty()) {
      is_accepting = false;
      return false;
    }
    is_accepting = true;
    int session_id;
    available_sessions_.Pop(session_id);
    if (sessions_[session_id] != nullptr) delete sessions_[session_id];
    sessions_[session_id] = new Session(session_id, packet_queue_,
      num_threads_ == 1 ? 
      io_services_[0] : io_services_[session_id % (num_threads_ - 1) + 1],
      available_sessions_);
    acceptor_->async_accept(sessions_[session_id]->GetSocket().get_socket(),
      boost::bind(&SessionManager::handle_accept, this, sessions_[session_id],
      boost::asio::placeholders::error));
    return true;
  }

  void handle_accept(Session* session, const boost::system::error_code& error)
  {
    if (!error) {
      // TODO Call Event Listener
      session->Receive();
      SessionManager::accept();
    } else {
      // TODO Log and Assert
    }
  }

  size_t capacity_;
  size_t num_threads_;
  bool is_accepting;

  boost::asio::ip::tcp::acceptor* acceptor_;
  boost::asio::ip::tcp::endpoint& endpoint_;
  std::vector<Session*> sessions_;
  std::vector<boost::asio::io_service> io_services_;
  std::vector<boost::asio::io_service::work> works_;
  pattern::ThreadSafeQueue<int> available_sessions_;
  PacketQueue& packet_queue_;

};
}

namespace mgne::udp {
class SessionManager {
public:
  SessionManager(size_t capacity, size_t num_threads,
    boost::asio::ip::udp::endpoint& endpoint, PacketQueue& packet_queue,
    int (*admit_handler)(Packet))
    : capacity_(capacity)
    , endpoint_(endpoint)
    , packet_queue_(packet_queue)
    , io_services_(num_threads)
    , num_threads_(num_threads)
    , admit_handler_(admit_handler)
    , accepting_socket_(io_services_[0], endpoint)
  {
    for (int i = 0; i < capacity_; i++) {
      sessions_.push_back(nullptr);
      available_sessions_.Push(i);
    }
    for (auto& io_service : io_services_) {
      works_.push_back(boost::asio::io_service::work(io_service));
    }
  }

  ~SessionManager()
  {
    for (int i = 0; i < capacity_; i++)
      if (sessions_[i] != nullptr) delete sessions_[i];
  }

  void StartAccepting(boost::thread_group& thread_group)
  {
    for (int i = 0; i < num_threads_; i++) {
      thread_group.create_thread(boost::bind(&boost::asio::io_service::run,
        &io_services_[0]));
    }
    accept();
  }

  void Send(const int session_id, Packet &packet) const {
    sessions_[session_id]->Send(false, packet);
  }

private:
  long long ep2str(boost::asio::ip::udp::endpoint& endpoint)
  {
    long long ret = endpoint.port();
    ret += (*(short*)(endpoint.address().to_v4().to_bytes().data()) << 2);
    return ret; 
  }

  void accept()
  {
    memset(&recv_buffer_, 0, sizeof(recv_buffer_));
    accepting_socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
      remote_endpoint_,
      boost::bind(&SessionManager::handle_accept, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
  }

  void handle_accept(const boost::system::error_code& error,
    size_t bytes_transferred)
  {
    if (error) {
      return;
    } else {
      int& session_id = endpoint_map_[ep2str(remote_endpoint_)];
      if (session_id != 0) {
        accept();
        return;
      }
      if (available_sessions_.Empty()) {
        accept();
        return;
      }

      available_sessions_.Pop(session_id);

      UDP_PACKET_HEADER* header = (UDP_PACKET_HEADER*)recv_buffer_.data();
      if (header->packet_id != PACKET_ADMIT_REQ) return;
      if (header->packet_size < sizeof(UDP_PACKET_HEADER) + sizeof(short)) return;

      Packet p((char*)(header + sizeof(UDP_PACKET_HEADER)),
        header->packet_size - sizeof(UDP_PACKET_HEADER), header->packet_id,
        0, Packet::PACKET_UDP);

      int socket_id = admit_handler_(p);
      if (socket_id == -1) return;
      Socket* socket = socket_map_[socket_id];
      if (socket == nullptr) {
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), 0);
        socket = socket_map_[socket_id] =
          new Socket(io_services_[socket_id % (num_threads_ - 1) + 1], endpoint,
          packet_queue_);
      } else {
        socket = socket_map_[socket_id]; 
      }

      if (sessions_[session_id] != nullptr) delete sessions_[session_id];
      sessions_[session_id] = new Session(session_id, remote_endpoint_, socket);
      header->packet_id = PACKET_ADMIT_ANS;
      header->packet_size = sizeof(UDP_PACKET_HEADER) + sizeof(short);
      *((short*)(header + sizeof(UDP_PACKET_HEADER))) = socket->GetPort();
      sessions_[session_id]->GetSocket()->Send(false, header->packet_size,
        header->packet_id, (char*)header, remote_endpoint_);
    }
  }

  size_t capacity_;
  size_t num_threads_;

  PacketQueue& packet_queue_;

  std::vector<boost::asio::io_service> io_services_;
  std::vector<boost::asio::io_service::work> works_;
  boost::asio::ip::udp::endpoint& endpoint_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  boost::asio::ip::udp::socket accepting_socket_;

  std::vector<Session*> sessions_;
  std::array<char, 128> recv_buffer_;
  std::unordered_map<long long, int> endpoint_map_;
  std::unordered_map<int, Socket*> socket_map_;
  pattern::ThreadSafeQueue<int> available_sessions_;

  int (*admit_handler_)(Packet);

};
}

#endif
