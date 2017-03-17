//
// packet_analyzer.hpp
//
/* 
  Until now, XOR cipher is only supported one.
*/

#ifndef _PACKET_ANALYZER_HPP_
#define _PACKET_ANALYZER_HPP_

#include <mgne/packet.hpp>

#define KEY_XOR_CIPHER 0xc247a3ef


namespace mgne {
class PacketAnalyzer {
public:
  static void Encrypt(Packet& packet)
  {
    for (int i = 0; i < packet.packet_size_; i++) {
      packet.data_[i] ^= KEY_XOR_CIPHER;
    }
  }

  static void Decrypt(Packet& packet)
  {
    for (int i = 0; i < packet.packet_size_; i++) {
      packet.data_[i] ^= KEY_XOR_CIPHER;
    }
  }
};
}

#endif

