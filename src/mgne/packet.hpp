//
// packet.hpp
//

#ifndef _PACKET_HPP_
#define _PACKET_HPP_

#include <string.h>
#include <vector>
#include <memory>

namespace mgne::tcp {
class Session;
}

namespace mgne::udp {
class Session;
}

namespace mgne {
class PacketAnalyzer;
class Packet {
public:
  typedef enum PacketType {
    PACKET_UDP,
    PACKET_TCP
  } PacketType;

  Packet() { }
  Packet(char* data, short packet_size, short packet_id, int session_id,
    PacketType packet_type)
    : data_(std::make_shared<std::vector<char>>(packet_size))
    , packet_id_(packet_id)
    , packet_size_(packet_size)
    , session_id_(session_id)
    , packet_type_(packet_type)
  {
    memcpy(data_->data(), data, packet_size_);
  }

  ~Packet()
  {
  }

  std::shared_ptr<std::vector<char>> GetPacketData()
  {
    return data_;
  }

  int GetSessionId()
  {
    return session_id_;
  }

  short GetPacketSize()
  {
    return packet_size_;
  }

  short GetPacketId()
  {
    return packet_id_;
  }

  PacketType GetType()
  {
    return packet_type_;
  }


private:
  std::shared_ptr<std::vector<char>> data_;
  int session_id_;
  short packet_size_;
  short packet_id_;
  PacketType packet_type_;

  friend class PacketAnalyzer;
  friend class tcp::Session;
  friend class udp::Session;
};
}

#endif

