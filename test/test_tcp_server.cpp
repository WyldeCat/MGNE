#include <iostream>

#include <mgne/mgne.hpp>

void packet_handler(mgne::Packet &p)
{
  std::cout << "I received Packet!!" << std::endl;
}

int main()
{
  FILE* f = fopen("test_log.txt","w");
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 4000);
  mgne::tcp::Server server(endpoint, 1000, 3, 3, packet_handler);
  mgne::log::Logger::SetFile(f);
  server.Run();

  return 0;
}
