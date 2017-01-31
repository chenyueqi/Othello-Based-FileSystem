#ifndef __COMMON_H__
#define __COMMON_H__

#include<cstdint>

const uint64_t defaultCapacity = (6UL*1024UL*1024UL*1024UL*1024UL); 	//6TB
const uint16_t MSGSIZE = 100;
const uint16_t ENTRYSIZE = 128;
const uint16_t objEntrySize = 128;

const uint8_t dcNum = 5;

uint16_t datacenter[dcNum] = {256, 256, 128 , 512, 64};

const uint16_t server_num = 1216;

class Gateway;
class Server;
class Client;

#endif
