//
// protocol.hpp
//

#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

// const 

struct TCP_PACKET_HEADER {
  short packet_size;
  short packet_id;
};

struct UDP_PACKET_HEADER {
  short packet_size; // It can be changed
  short packet_id;
};

const short PACKET_ADMIT_REQ = -1;
const short PACKET_ADMIT_ANS = -1;

#endif
