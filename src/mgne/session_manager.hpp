//
// session_manager.hpp
//

#ifndef _SESSION_MANAGER_HPP_
#define _SESSION_MANAGER_HPP_

#include <vector>
#include <deque>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <mgne/server.hpp>
#include <mgne/session.hpp>
#include <mgne/thread_manager.hpp>
#include <mgne/packet_queue.hpp>

namespace mgne::tcp {
class SessionManager {
public:
  SessionManager(size_t capacity, size_t num_threads,
    boost::asio::ip::tcp::endpoint& endpoint, PacketQueue& packet_queue)
    : capacity_(capacity) 
    , num_threads_(num_threads)
    , io_services_(num_threads)
    , packet_queue_(packet_queue)
  {
    for (int i = 0; i < capacity_; i++) {
      sessions_.push_back(nullptr); 
      available_sessions_.push(i);
    }
  }

  ~SessionManager() 
  {
    for (int i = 0; i < capacity_; i++) delete sessions_[i];
    if (acceptor_ != NULL) delete acceptor_;
  }

  void StartAccepting(boost::thread::thread_group& thread_group)
  {
    acceptor_ = new boost::asio::ip::tcp::acceptor(io_services.front(), 
      endpoint_);
    ::accept();
    for (int i = 0; i < num_threads_; i++) {
      thread_group.create_thread([]{ io_services[i].run(); });
    }
  }

  void CloseSession(int session_id)
  {
    delete sessions_[session_id];  
    available_sessions_.push(session_id);
  }

private:
  bool accept()
  {
    if (available_sessions.empty()) {
      is_accepting = false;
      return false;
    }
    is_accepting = true;
    // TODO : thread safe queue
    int session_id = available_sessions.front();
    available_sessions.pop();
    sessions_[session_id] = new Session(i, packet_queue_,
      num_threads_ == 1 ? io_services_[0] : io_services_[id%(num_threads_-1)+1]);
    //
    acceptor_.async_accept(sessions_[session_id].GetSocket().get_socket(),
      boost::bind(&SessionManager::handle_accept, this, sessions_[session_id],
      boost::asio::placeholders);

    return true;
  }

  void handle_accept(Session* session, const boost::system::error_code& error)
  {
    if (!error) {
      // TODO Call Event Listener
      session->Receive();
      ::accept();
    } else {
      // TODO Log and Assert
    }
  }

  size_t capacity_;
  size_t num_threads_;
  bool is_accepting;

  boost::asio::ip::tcp::acceptor* acceptor_;
  boost::asio::ip::tcp::endpoint& endpoint_;
  PacketQueue& packet_queue_;
  std::vector<Session*> sessions_;
  std::vector<boost::asio::io_service> io_services_;
  std::queue<int> available_sessions_;

};
}

namespace mgne::udp {
class SessionManager {
public:
  SessionManager(size_t capacity, Server& server)
    : server_(server)
  {
  }

  ~SessionManager() { }

  void StartAccepting(size_t num_threads, ThreadManager& thread_manager)
  {
  }

  void GetServer()
  {
  }

private:
  size_t capacity_;
  Server& server_;
  std::unordered_map<int, Session> sessions_;
};
}

#endif
