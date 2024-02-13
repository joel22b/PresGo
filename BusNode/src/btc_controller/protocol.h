/*
 * protocol.h
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "stdint.h"

#define UUID_LEN 16

typedef enum {
  pt_req_none = 0x00,
  pt_req_fare_id = 0x01,
} pt_req_t;

#define pt_msg_req_type 0x00001000
struct pt_msg_req {
  pt_req_t req;
};
typedef struct pt_msg_req pt_msg_req_t;

#define pt_msg_fare_id_type 0x00010000
struct pt_msg_fare_id {
  uint8_t uuid[UUID_LEN];
};
typedef struct pt_msg_fare_id pt_msg_fare_id_t;

struct pt_msg {
  uint32_t type;
  union {
    pt_msg_req_t      request;
    pt_msg_fare_id_t  fare_id;
  } data;
};
typedef struct pt_msg pt_msg_t;

inline uint8_t pt_sizeof(pt_msg_t* msg) {
  uint8_t data_size = 0;
  switch (msg->type) {
    case pt_msg_req_type:
      data_size = sizeof(pt_msg_req_t);
      break;
    case pt_msg_fare_id_type:
      data_size = sizeof(pt_msg_fare_id_t);
      break;
    default:
      return sizeof(pt_msg_t);
  }
  // Add the rest of the pt_msg_t struct other than data
  return data_size + sizeof(uint32_t);
}

#endif /* PROTOCOL_H_ */
