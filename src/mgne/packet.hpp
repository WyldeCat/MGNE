//
// packet.hpp
//

#ifndef _PACKET_HPP_
#define _PACKET_HPP_

#include <string.h>

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

  Packet(char* data, int packet_size, int session_id, PacketType packet_type)
    : data_(new char[packet_size])
    , session_id_(session_id)
    , packet_type_(packet_type)
  {
    strncpy(data_, data, packet_size);   
  }

  ~Packet()
  {
    delete[] data_;
  }

private:
  char* data_;
  int session_id_;
  int packet_id_;
  int packet_size_;
  PacketType packet_type_;

  friend class PacketAnalyzer;
  friend class tcp::Session;
  friend class udp::Session;
};
}

#endif
