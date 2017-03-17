//
// session_manager.hpp
//

#ifndef _SESSION_MANAGER_HPP_
#define _SESSION_MANAGER_HPP_

#include <vector>
#include <deque>
#include <unordered_map>

#include <mgne/server.hpp>
#include <mgne/session.hpp>
#include <mgne/thread_manager.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace mgne::tcp {
class SessionManager {
public:
  SessionManager(size_t capacity, Server& server)
    : server_(server)
    , capacity_(capacity) 
  {
    for (int i = 0; i < capacity_; i++) {
      sessions_.push_back(new Session(i, this)); 
      available_sessions.push(i);
    }
  }

  ~SessionManager() 
  {
    for (int i = 0; i < capacity_; i++) {
      delete sessions_[i];
    }
    for (int i = 0; i < io_services_.size(); i++) {
      delete io_services_[i];
    }
    delete acceptor_;
  }

  void StartAccepting(size_t num_threads, ThreadManager& thread_manager)
  {
    io_services_.push_back(new boost::asio::io_service);
    acceptor_ = new boost::asio::ip::tcp::acceptor(*io_service_back(),
      server_.GetEndPoint());
    ::accept();
    for (int i = 0; i < num_threads; i++) {
      thread_manager.CreateThread(io_services_back()->run());
    }
  }

  boost::asio::io_service& GetIOService(int id)
  {
    return io_services[id%num_threads_];
  }

  const Server& GetServer()
  {
    return server_; 
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

  Server& server_;
  boost::asio::ip::tcp::acceptor* acceptor_;
  std::vector<Session*> sessions_;
  std::vector<boost::asio::io_service*> io_services_;
  std::queue<int> available_sessions;

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
