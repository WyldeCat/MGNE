#include <stdio.h>

#include "logger.hpp"

int main( )
{
  FILE *f = fopen("test.txt","w");
  mgne::log::Logger::SetFile(f);
  mgne::log::Logger::Log("Hello, world!");
  return 0;
}
