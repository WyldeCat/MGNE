#include <iostream>
#include <mgne/mgne.hpp>
#include <boost/asio.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 4000;

int main()
{
  TCP_PACKET_HEADER header;
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::endpoint endpoint(
    boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);
  boost::system::error_code connect_error;
  boost::asio::ip::tcp::socket socket(io_service);

  socket.connect(endpoint, connect_error);
  header.packet_size = sizeof(TCP_PACKET_HEADER) + 1;
  header.packet_id = 99;

  socket.write_some(boost::asio::buffer((char*)&header,sizeof(TCP_PACKET_HEADER)));
  socket.write_some(boost::asio::buffer("a",1));

  if (connect_error) {
    std::cout << "Connection fail." << std::endl;
    std::cout << connect_error << std::endl;
  }

  return 0;
}
