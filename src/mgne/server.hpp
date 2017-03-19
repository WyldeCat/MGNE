//
// server.hpp
//

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <boost/asio.hpp>
#include <mgne/session_manager.hpp>
#include <mgne/packet_queue.hpp>

namespace mgne::tcp {
class Server {
public:
  Server(boost::asio::ip::tcp::endpoint endpoint,
    size_t capacity, size_t num_io_threads, size_t num_pq_threads)
    : address_(address)
    , capacity_(capacity)
    , session_manager_(capacity_, num_io_threads, *this)
    , packet_queue_(num_pq_threads, *this)
  {
  }

  ~Server()
  {
  }
  
  boost::asio::io_service& GetIOService()
  {
    return io_service_;
  }

  const boost::asio::ip::tcp::endpoint& GetEndpoint()
  {
    return endpoint_;
  }

private:
  PacketQueue packet_queue_;
  SessionManager session_manager_;

  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::io_service io_service_;
  size_t capacity_;
};
}

namespace mgne::udp {
class Server {
public:
  Server(boost::asio::ip::address, size_t capacity)
    : address_(address)
    , capacity_(capacity)
  {
  }
  ~Server()
  {
  }
private:
  boost::asio::ip::address address_;
  boost::asio::io_service io_service_;
  size_t capacity_;
};
}

#endif
