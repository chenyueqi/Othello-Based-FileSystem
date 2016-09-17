#ifndef __COMMON_H__
#define __COMMON_H__

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

const uint64_t defaultCapacity 	= 1000000; 	//1000000 B
const uint64_t fileBlockSize	= 10000; 	//10000 B
const uint64_t dirFileEntrySize = 100; 		//100 B

const uint16_t totalServer = 128 * 4;
const uint8_t dcBit = 0x2;
const uint8_t serverPerDcBit = 0x7;


class Gateway;
class Server;
class Client;

#endif
