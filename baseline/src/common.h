#ifndef __COMMON_H__
#define __COMMON_H__

#include<cstdint>
/*
*/
const uint64_t defaultCapacity = (6UL*1024UL*1024UL*1024UL*1024UL); 	//6TB
const uint64_t objEntrySize = 128; 		//128 B

const uint16_t totalServer = (128 * 4);

class Gateway;
class Server;
class Client;

#endif
