//
// protocol.hpp
//

#ifndef _MGNE_PROTOCOL_HPP_
#define _MGNE_PROTOCOL_HPP_

// const 

struct TCP_PACKET_HEADER {
  short packet_size;
  short packet_id;
};

struct UDP_PACKET_HEADER {
  short packet_size;
  short packet_id;
};

const short PACKET_ADMIT_REQ = -1;
const short PACKET_ADMIT_ANS = -2;

#endif
