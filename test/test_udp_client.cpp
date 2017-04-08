#include <iostream>

#include <mgne/protocol.hpp>
#include <boost/asio.hpp>

int main( )
{
  boost::asio::io_service io_service;
  boost::asio::ip::udp::endpoint endpoint1(
    boost::asio::ip::address::from_string("127.0.0.1"), 4007);
  boost::asio::ip::udp::endpoint endpoint2(
    boost::asio::ip::address::from_string("127.0.0.1"), 4000);
  boost::asio::ip::udp::socket socket(io_service, endpoint1); 

  char send_buffer[1024]={0,};
  UDP_PACKET_HEADER *header = (UDP_PACKET_HEADER*)send_buffer;
  header->packet_id = PACKET_ADMIT_REQ;
  header->packet_size = sizeof(UDP_PACKET_HEADER) + sizeof(short);
  *((short*)(send_buffer+sizeof(UDP_PACKET_HEADER))) = 4001;

  socket.send_to(boost::asio::buffer((char*)header, header->packet_size),
    endpoint2);
  return 0;
}
