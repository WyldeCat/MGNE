//
// socket.hpp
//

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <string.h>

#include <deque>
#include <array>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <mgne/packet_queue.hpp>
#include <mgne/pattern/thread_safe_queue.hpp>
#include <mgne/log/logger.hpp>
#include <mgne/protocol.hpp>

namespace mgne::tcp {
class Socket {
public:
  Socket(boost::asio::io_service& io_service, PacketQueue& packet_queue,
    pattern::ThreadSafeQueue<int>& available_sessions, int session_id)
    : socket_(io_service)
    , packet_queue_(packet_queue)
    , available_sessions_(available_sessions)
    , session_id_(session_id)
  {
    packet_buffer_mark_ = 0;
  }

  ~Socket() { socket_.close(); }

  void Send(const bool immediately, const short packet_size,
    const short packet_id, char* packet_data)
  {
    char* data = nullptr;
    if (immediately == false) {
      data = new char[packet_size];
      memcpy(data, packet_data, packet_size);
      send_data_queue_.push_back(data);
    } else {
      data = packet_data;
    }
    if (immediately == false && send_data_queue_.size() > 1) return;
    boost::asio::async_write(socket_, boost::asio::buffer(data, packet_size),
      boost::bind(&Socket::handle_write, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
  }

  void Receive() 
  { 
    memset(&receive_buffer_, 0, sizeof(receive_buffer_));
    socket_.async_read_some(boost::asio::buffer(receive_buffer_),
      boost::bind(&Socket::handle_read, this,
      boost::asio::placeholders::error, 
      boost::asio::placeholders::bytes_transferred));
  }

  void Close() { close(); }
  boost::asio::ip::tcp::socket& get_socket() { return socket_; }

private:
  void close()
  {
    socket_.close();
    available_sessions_.Push(session_id_);
  }

  void handle_write(const boost::system::error_code& error,
    size_t bytes_transferred) 
  { 
    delete[] send_data_queue_.front();
    send_data_queue_.pop_front();

    if (send_data_queue_.empty() == false) {
      char* data = send_data_queue_.front();
      TCP_PACKET_HEADER* header = (TCP_PACKET_HEADER*)data;
      Send(true, header->packet_size, header->packet_id, data);
    }
  }

  void handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
  {
    if (error) {
      if (error == boost::asio::error::eof) {
        // Close session
        // Add handler
      } else {
        // Log
        // TODO
      }
      close();
    } else {
      memcpy(&packet_buffer_[packet_buffer_mark_], receive_buffer_.data(),
        bytes_transferred);
      int packet_len = packet_buffer_mark_ + bytes_transferred;
      int readed_len = 0;

      while (packet_len > 0) {
        if (packet_len < sizeof(TCP_PACKET_HEADER)) break;
        TCP_PACKET_HEADER* header =
          (TCP_PACKET_HEADER*)&packet_buffer_[readed_len];

        if (packet_len >= header->packet_size) {
          packet_queue_.Push(Packet(packet_buffer_ + readed_len +
            sizeof(TCP_PACKET_HEADER), 
            header->packet_size - sizeof(TCP_PACKET_HEADER),
            header->packet_id, session_id_,
            Packet::PACKET_TCP));
          packet_len -= header->packet_size;
          readed_len += header->packet_size;
        } else {
          break;
        }
      }

      if (packet_len > 0) {
        char temp_buffer[256*2] = {0,};
        memcpy(&temp_buffer[0], &packet_buffer_[readed_len], packet_len);
        memcpy(&packet_buffer_[0], &temp_buffer[0], packet_len);
      }
      packet_buffer_mark_ = packet_len;
      Receive();
    }
  }

  boost::asio::ip::tcp::socket socket_;
  PacketQueue& packet_queue_;
  pattern::ThreadSafeQueue<int>& available_sessions_;
  std::array<char, 256> receive_buffer_;
  std::deque<char*> send_data_queue_;
  int packet_buffer_mark_;
  int session_id_;
  char packet_buffer_[256*2];
};
}

namespace mgne::udp {
class Socket {
public:
private:
};
}

#endif
