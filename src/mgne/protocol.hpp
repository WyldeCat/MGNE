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
  short packet_sender;
  short packet_id;
};

#endif
