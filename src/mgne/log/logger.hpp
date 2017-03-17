#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <mutex>

namespace mgne::log {
class Logger {
public:
  static void Log(char* str)
  {
    mutex_.lock();
    fprintf(file_,"%s\n",str);
    mutex_.unlock();
  }
  static bool SetFile(FILE* file)
  {
    mutex_.lock();
    //FIXME
    file_ = file;
    mutex_.unlock();
    return true;
  }

private:
  static FILE* file_;
  static std::mutex mutex_;
  Logger() { }
  ~Logger() { }
};

FILE* Logger::file_;
std::mutex Logger::mutex_;
}

#endif
