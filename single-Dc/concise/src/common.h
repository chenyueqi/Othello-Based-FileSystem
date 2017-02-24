#ifndef OTHELLO_COMMON_H_
#define OTHELLO_COMMON_H_

#include<cstring>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

const uint64_t defaultCapacity 	= 1000000; 	//1000000 B
const uint64_t fileBlockSize	= 10000; 	//10000 B
const uint64_t dirEntrySize = 100; 		//100 B

const uint8_t dcBit = 0x2; // 1<<dcBit-1 < datacenter number <= 1<<dcBit
const uint8_t serverPerDcBit = 0x7; // server number per datacenter = 1<<serverPerDcBit 

const uint8_t dcNum = 5;


class Gateway;
class Central;
class Server;
class Client;

#endif
