#include <iostream>

#include <mgne/mgne.hpp>

int main()
{
  FILE* f = fopen("test_log.txt","w");
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 4000);
  mgne::tcp::Server server(endpoint, 1000, 3, 3);
  mgne::log::Logger::SetFile(f);
  server.Run();

  return 0;
}
