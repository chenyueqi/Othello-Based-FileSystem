#ifndef __COMMON_H__
#define __COMMON_H__

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

const uint16_t totalServer = (1 << (dcBit + serverPerDcBit));


class Gateway;
class Central;
class Server;
class Client;

struct dataflow
{
    uint16_t cnt;
    uint64_t size;
};

uint16_t datacenter[dcNum] = {1<< serverPerDcBit, 1<<(serverPerDcBit - 1), 1 << (serverPerDcBit - 2) , 1 << (serverPerDcBit -3), 1<< (serverPerDcBit-1)};

#endif
