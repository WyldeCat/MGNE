#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include <deque>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

class Server;

namespace mgne::tcp {
class Session {
public:
  Session(int session_id, boost::asio::io_service& io_service,
      Server* server) : session_id_(session_id), io_service_(io_service),
      server_(server)
  {
  }

  ~Session()
  {
    while (send_data_queue_.empty() == false) {
      delete[] send_data_queue_.front();
      send_data_queue_.pop_front();
    }
  }

  void Init() 
  {
    packet_buffer_mark_ = 0;
  }

  int SessionID() { return session_id; }

  void PostReceive()
  {
    socket_.async_read_some(boost::asio::buffer(receive_buffer_),
        boost::bind(&Session::handle_receive, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  }

  void PostSend(const bool immediately, const int packet_size,
      char* packet_data)
  {
    char* send_data = nullptr;

    if (immediately == false) {
      send_data = new char[packet_size];
      memcpy(send_data, packet_data, packet_size);
      send_data_queue_.push_back(send_data);
    } else {
      send_data = packet_data;
    }

    if (immediately == false && send_data_queue_.size() > 1) return;

    boost::asio::async_write(socket_, boost::asio::buffer(send_data,
        packet_size), boost::bind(&Session::handle_write, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
        
  }

private:
  void handle_write(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    delete[] send_data_queue_.front();
    send_data_queue_.pop_front();

    if (send_data_queue_.empty() == false) {
      // HACK
      char* packet_data = send_data_queue_.front();
      PostSend(true, strlen(packet_data), packet_data);
    }
  }

  void handle_receive(const boost:;system::error_code& error,
      size_t bytes_transferred)
  {
    if (error) {
      // TODO
      if (error == boost::asio::error::eof) {
      } else {
      }
      server_->CloseSession(session_id_);
    } else {
      memcpy(&packet_buffer_[packet_buffer_mark_], receive_buffer_.data(),
          bytes_transferred);

      int packet_size = packet_buffer_mark_ + bytes_transferred;
      int readed_data = 0;

      while (packet_size > 0) {
        // TODO
      }

      if (packet_size > 0) {
        char temp_buffer[MAX_RECEIVE_BUFFER_LEN] = {0,};
        memcpy(&temp_buffer[0], &packet_buffer_[readed_data],packet_size);
        memcpy(&packet_buffer_[0], &temp_buffer[0], packet_size);
      }
      packet_buffer_mark_ = packet_size;
      PostReceive();
    }
  }

  int session_id_;
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::socket socket_;

  std::deque<char*> send_data_queue_;
  std::array<char, MAX_RECEIVE_BUFFER_LEN> receive_buffer_;
  int packet_buffer_mark_;
  char packet_buffer_[MAX_RECEIVE_BUFFER_LEN*2];
};

}

#endif
