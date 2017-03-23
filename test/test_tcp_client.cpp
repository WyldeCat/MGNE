#include <iostream>
#include <mgne/mgne.hpp>
#include <boost/asio.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 4000;

int main()
{
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::endpoint endpoint(
    boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);
  boost::system::error_code connect_error;
  boost::asio::ip::tcp::socket socket(io_service);

  socket.connect(endpoint, connect_error);

  if (connect_error) {
    std::cout << "Connection fail." << std::endl;
    std::cout << connect_error << std::endl;
  }

  return 0;
}
