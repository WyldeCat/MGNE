//
// server.hpp
//

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <boost/asio.hpp>
#include <mgne/session_manager.hpp>

namespace mgne::tcp {
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
