#ifndef OTHELLO_COMMON_H_
#define OTHELLO_COMMON_H_

#include<cstring>
#include<assert.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

const uint64_t defaultCapacity 	= 1000000; 	//1000000 B
const uint64_t fileBlockSize	= 10000; 	//10000 B
const uint64_t dirEntrySize = 100; 		//100 B
const uint64_t server_num = 256;


class Gateway;
class Central;
class Server;
class Client;

#endif
