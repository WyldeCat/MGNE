//
// socket.hpp
//

#ifndef _MGNE_SOCKET_HPP_
#define _MGNE_SOCKET_HPP_

#include <string.h>

#include <array>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <mgne/packet_queue.hpp>
#include <mgne/pattern/thread_safe_queue.hpp>
#include <mgne/pattern/thread_safe.hpp>
#include <mgne/log/logger.hpp>
#include <mgne/protocol.hpp>

namespace mgne::tcp {
class Socket : mgne::pattern::ThreadSafe {
public:
  Socket(boost::asio::io_service& io_service, PacketQueue& packet_queue,
    void (*error_handler)(int),
    pattern::ThreadSafeQueue<int>& available_sessions, int session_id)
    : socket_(io_service)
    , packet_queue_(packet_queue)
    , error_handler_(error_handler)
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
      Lock();
      data = new char[packet_size];
      memcpy(data, packet_data, packet_size);
      send_data_queue_.push_back(data);
      Unlock();
    } else {
      data = packet_data;
    }
    if (immediately == false && send_data_queue_.size() > 1) {
      return;
    }
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
  short GetPort() { return socket_.local_endpoint().port(); }
  boost::asio::io_service& GetIOService() { return socket_.get_io_service(); }
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
    Lock();
    if (send_data_queue_.size() == 0) {
      Unlock();
      return;
    }
    delete[] send_data_queue_.front();
    send_data_queue_.pop_front();

    if (send_data_queue_.empty() == false) {
      char* data = send_data_queue_.front();
      TCP_PACKET_HEADER* header = (TCP_PACKET_HEADER*)data;
      Send(true, header->packet_size, header->packet_id, data);
    }
    Unlock();
  }

  void handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
  {
    if (error) {
      error_handler_(session_id_); 
      close();
    } else {
      memcpy(&packet_buffer_[packet_buffer_mark_], receive_buffer_.data(),
        bytes_transferred);
      int packet_len = packet_buffer_mark_ + bytes_transferred;
      int readed_len = 0;

      while (packet_len > 0) {
        if (packet_len <= sizeof(TCP_PACKET_HEADER)) break;
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
  void (*error_handler_)(int session_id);
  pattern::ThreadSafeQueue<int>& available_sessions_;
  std::array<char, 256> receive_buffer_;
  std::deque<char*> send_data_queue_;
  int packet_buffer_mark_;
  int session_id_;
  char packet_buffer_[256*2];
};
}

namespace mgne::udp {
class Socket : mgne::pattern::ThreadSafe {
public:
  Socket(boost::asio::io_service& io_service,
    boost::asio::ip::udp::endpoint& endpoint,
    PacketQueue& packet_queue) 
    : socket_(io_service, endpoint)
    , packet_queue_(packet_queue)
  {
  }

  ~Socket() { socket_.close(); }

  void Send(const bool immediately, const short packet_size,
    const short packet_id, char* packet_data,
    boost::asio::ip::udp::endpoint& remote_endpoint)
  {
    char* data = nullptr;
    boost::asio::ip::udp::endpoint endpoint;
    if (immediately == false) {
      Lock();
      data = new char[packet_size];
      endpoint = remote_endpoint;
      memcpy(data, packet_data, packet_size);
      send_endpoint_queue_.push_back(remote_endpoint);
      send_data_queue_.push_back(data);
      Unlock();
    } else {
      data = packet_data;
      endpoint = remote_endpoint;
    }
    if (immediately == false && send_data_queue_.size() > 1) return;
    socket_.async_send_to(boost::asio::buffer(data, packet_size), endpoint,
      boost::bind(&Socket::handle_write, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
  }

  void Receive()
  {
    memset(&recv_buffer_, 0, sizeof(recv_buffer_));
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
      remote_endpoint_,
      boost::bind(&Socket::handle_read, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
  }

  static long long ep2ll(boost::asio::ip::udp::endpoint& endpoint)
  {
    long long ret = endpoint.port();
    ret += (*(short*)(endpoint.address().to_v4().to_bytes().data()) << 2);
    return ret; 
  }

  std::unordered_map<long long,int>& GetAttachedSessions() 
  { 
    return attached_sessions_; 
  }

  void Close() { close(); }
  short GetPort() { return socket_.local_endpoint().port(); }
  boost::asio::io_service& GetIOService() { return socket_.get_io_service(); }
 
  boost::asio::ip::udp::socket& get_socket() { return socket_; }

private:
  void close()
  {
    socket_.close();
  }

  void handle_write(const boost::system::error_code& error,
    size_t bytes_transferred)
  {
    Lock();
    if (send_data_queue_.size() == 0) {
      Unlock();
      return; }
    delete[] send_data_queue_.front();
    send_data_queue_.pop_front();
    send_endpoint_queue_.pop_front();

    if (send_data_queue_.empty() == false) {
      char* data = send_data_queue_.front();
      UDP_PACKET_HEADER* header = (UDP_PACKET_HEADER*)data;
      Send(true, header->packet_size, header->packet_id, data,
        send_endpoint_queue_.front());
    }
    Unlock();
  }

  void handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
  {
    int session_id = attached_sessions_[ep2ll(remote_endpoint_)];
    if (error) {

    } else {
      UDP_PACKET_HEADER* header = (UDP_PACKET_HEADER*)recv_buffer_.data();
      packet_queue_.Push(Packet((char*)(header + 1),
        header->packet_size - sizeof(UDP_PACKET_HEADER), header->packet_id,
        session_id, Packet::PacketType::PACKET_UDP));
      Receive();
    }
  }

  boost::asio::ip::udp::socket socket_;
  std::array<char, 256> recv_buffer_;
  std::unordered_map<long long, int> attached_sessions_;
  std::deque<char*> send_data_queue_;
  std::deque<boost::asio::ip::udp::endpoint> send_endpoint_queue_;
  boost::asio::ip::udp::endpoint remote_endpoint_;

  PacketQueue& packet_queue_;
};
}

#endif
