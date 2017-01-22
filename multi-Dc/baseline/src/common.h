#ifndef __COMMON_H__
#define __COMMON_H__

#include<cstdint>

const uint64_t defaultCapacity = (6UL*1024UL*1024UL*1024UL*1024UL); 	//6TB
const uint64_t objEntrySize = 128; 		//128 B
const uint64_t messageSize = 100;


class Gateway;
class Server;
class Client;

struct dataflow
{
    uint16_t cnt;
    uint64_t size;
};

#ifdef SYSTEM1
const uint8_t dcBit = 0x3;
const uint8_t dcNum = 5; // 1 << dcBit >= dcNum

//const uint16_t totalServer = (1 << (dcBit + serverPerDcBit));
const uint8_t serverPerDcBit = 0x7;
uint16_t datacenter[dcNum] = {
    1<< serverPerDcBit, 	//128
    1<<(serverPerDcBit - 1), 	//64
    1<<(serverPerDcBit - 2), 	//32
    1<<(serverPerDcBit - 3), 	//16
    1<<(serverPerDcBit - 1)	//64
};
#endif

#ifdef SYSTEM2
const uint8_t dcBit = 0x3;
const uint8_t dcNum = 5; // 1 << dcBit >= dcNum

//const uint16_t totalServer = (1 << (dcBit + serverPerDcBit));
const uint8_t serverPerDcBit = 0x7;
uint16_t datacenter[dcNum] = {
    1<< serverPerDcBit, 		//128
    (1<<(serverPerDcBit - 1)) + 10,	//74
    (1<<(serverPerDcBit - 2)) + 10, 	//42
    (1<<(serverPerDcBit - 3)) + 10, 	//26
    (1<<(serverPerDcBit - 1)) + 10	//74
};
#endif

#endif
