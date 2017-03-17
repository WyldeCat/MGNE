#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <deque>
#include <array>

#include <boost/asio.hpp>

#include <mgne/session.hpp>
#include <mgne/packet_queue.hpp>

namespace mgne::tcp {
class Socket {
public:
  Socket(boost::asio::io_service& io_service,
    const boost::asio::ip::address& address_, PacketQueue& packet_queue_,
    Session& session)
    : 
  {
  }
private:
  boost::asio::ip::tcp::socket socket_;
  mgne::tcp::Session& session_;
  mgne::tcp::PacketQueue& packet_queue_;
  std::array<char, 128> receive_buffer_;
  std::deque<char*> send_data_queue_;
  int packet_buffer_mark_;
  char packet_buffer_[128*2];

};
}
#endif
