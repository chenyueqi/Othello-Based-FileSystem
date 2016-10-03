#ifndef __COMMON_H__
#define __COMMON_H__

#include<cstdint>

const uint64_t defaultCapacity = (6UL*1024UL*1024UL*1024UL*1024UL); 	//6TB
const uint64_t objEntrySize = 128; 		//128 B
const uint64_t messageSize = 100;

const uint8_t dcBit = 0x3;
const uint8_t serverPerDcBit = 0x7;

const uint8_t dcNum = 5; // 1 << dcBit >= dcNum

const uint16_t totalServer = (1 << (dcBit + serverPerDcBit));

class Gateway;
class Server;
class Client;

struct dataflow
{
    uint16_t cnt;
    uint64_t size;
};

uint16_t datacenter[dcNum] = {1<< serverPerDcBit, 1<<(serverPerDcBit - 1), 1 << (serverPerDcBit - 2) , 1 << (serverPerDcBit -3), 1<< (serverPerDcBit-1)};

#endif
