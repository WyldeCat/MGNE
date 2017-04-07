//
// server.hpp
//
/*
io에 대해선, thread pool이 필요 없지만,
packet queue에서의 worker thread들이 동작하는데에 있어선,
thread pool이 필요하다.
*/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <mgne/session_manager.hpp>
#include <mgne/packet.hpp>
#include <mgne/packet_queue.hpp>

namespace mgne {
class BasicServer {
public:
  BasicServer(size_t capacity, size_t num_pq_threads,
    void (*packet_handler)(Packet&))
    : capacity_(capacity)
    , packet_queue_(num_pq_threads, packet_handler)
  {
  }

  ~BasicServer()
  {
  }

  virtual void Run() = 0;
  virtual void Stop() = 0;

protected:
  // HACK
  boost::thread_group thread_group_;
  PacketQueue packet_queue_;
  size_t capacity_;

  // Handler

};
}

namespace mgne::tcp {
class Server : public BasicServer {
public:
  Server(boost::asio::ip::tcp::endpoint endpoint,
    size_t capacity, size_t num_io_threads, size_t num_pq_threads,
    void (*packet_handler)(Packet&))
    : BasicServer(capacity, num_pq_threads, packet_handler)
    , endpoint_(endpoint)
    , session_manager_(capacity_, num_io_threads, endpoint_, packet_queue_)
  {
  }

  ~Server()
  {
  }
  
  void Run() 
  {
    session_manager_.StartAccepting(thread_group_); 
    packet_queue_.StartProcessing(thread_group_);
    thread_group_.join_all();
  }
  
  void Stop()
  {
    // DO I really need to implement this?
  }

  const SessionManager& GetSessionManager() { return session_manager_; }

private:
  boost::asio::ip::tcp::endpoint endpoint_;
  SessionManager session_manager_;
};
}

namespace mgne::udp {
class Server : public BasicServer {
public:
  Server(boost::asio::ip::udp::endpoint endpoint,
    size_t capacity, size_t num_io_threads, size_t num_pq_threads,
    void (*packet_handler)(Packet&), int (*admit_handler)(Packet))
    : BasicServer(capacity, num_pq_threads, packet_handler)
    , endpoint_(endpoint)
    , session_manager_(capacity_, num_io_threads, endpoint_, packet_queue_,
      admit_handler)
  {
  }

  void Run()
  {
    session_manager_.StartAccepting(thread_group_);
    packet_queue_.StartProcessing(thread_group_);
    thread_group_.join_all();
  }

  void Stop()
  {
  }

private:
  boost::asio::ip::udp::endpoint endpoint_;
  SessionManager session_manager_;
};
}

#endif
