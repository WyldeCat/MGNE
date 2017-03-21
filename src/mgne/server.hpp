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

#include <mgne/session_manager.hpp>
#include <mgne/packet_queue.hpp>

namespace mgne {
class BasicServer {
public:
  BasicServer(boost::asio::ip::tcp::endpoint endpoint, size_t capacity,
    size_t num_pq_threads) 
    : endpoint_(endpoint)
    , capacity_(capacity)
    , packet_queue_(num_pq_threads, *this)
  {
  }

  ~BasicServer()
  {
  }

  const boost::asio::ip::tcp::endpoint& GetEndpoint()
  {
    return endpoint_;
  }
  
  virtual void Run() = 0;
  virtual void Stop() = 0;

private:
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::thread::thread_group thread_group_;
  PacketQueue packet_queue_;
  size_t capacity_;

};
}

namespace mgne::tcp {
class Server : public BasicServer {
public:
  Server(boost::asio::ip::tcp::endpoint endpoint,
    size_t capacity, size_t num_io_threads, size_t num_pq_threads)
    : BasicServer(endpoint, capacity, num_pq_threads)
    , session_manager_(capacity_, num_io_threads, endpoint_, packet_queue_)
  {
  }

  ~Server()
  {
  }
  
  void Run() 
  {
    StartAccepting(thread_group_); 
  }
  
  void Stop()
  {
    // DO I really need to implement this?

  }

private:
  SessionManager session_manager_;
};
}

namespace mgne::udp {
class Server : public BasicServer {
public:
  Server(boost::asio::ip::tcp::endpoint endpoint,
    size_t capacity, size_t num_io_threads, size_t num_pq_threads)
    : BasicServer(endpoint, capacity, num_pq_threads)
    , session_manager_(capacity_, num_io_threads, *this)
  {
  }

  ~Server()
  {
  }

  void Run()
  {
  }

  void Stop()
  {
  }

private:
  SessionManager session_manager_;
};
}

#endif
