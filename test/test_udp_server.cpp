#include <stdio.h>

#include <mgne/mgne.hpp>
#include <boost/asio.hpp>

int admit_handler(mgne::Packet p)
{
  std::cout << "receiving admit req" << std::endl;
  return 1;
}

int main()
{
  boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), 4000); 
  mgne::udp::Server server(endpoint, 500, 3, 3, nullptr, admit_handler);
  server.Run();
  return 0;
}
