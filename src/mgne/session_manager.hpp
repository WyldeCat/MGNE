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

#include <mgne/session.hpp>
#include <mgne/packet_queue.hpp>
#include <mgne/pattern/thread_safe_queue.hpp>

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

private:
  bool accept()
  {
    if (available_sessions_.Empty()) {
      is_accepting = false;
      return false;
    }
    is_accepting = true;
    // TODO : thread safe queue
    int session_id = available_sessions_.front();
    available_sessions_.Pop();
    if (sessions_[session_id] != nullptr) delete sessions_[session_id];
    sessions_[session_id] = new Session(session_id, packet_queue_,
      num_threads_ == 1 ? 
      io_services_[0] : io_services_[session_id % (num_threads_ - 1) + 1],
      available_sessions_);
    //
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
private:
};
}

#endif
