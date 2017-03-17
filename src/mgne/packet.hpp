//
// packet.hpp
//

#ifndef _PACKET_HPP_
#define _PACKET_HPP_

#include <string.h>

#include <mgne/session.hpp>

namespace mgne {
class Packet {
public:
  typedef enum PacketType {
    PACKET_UDP,
    PACKET_TCP
  } PacketType;

  Packet(char* data, int packet_size, Session session, PacketType packet_type)
    : data_(new char[packet_size])
    , session_(session)
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
  int packet_size_;
  Session& session_;
  PacketType packet_type_;

  friend class PacketAnalyzer;
};
}

#endif
