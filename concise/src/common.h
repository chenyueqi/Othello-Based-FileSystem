#ifndef __COMMON_H__
#define __COMMON_H__

#include<cstring>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

const uint64_t defaultCapacity 	= 1000000; 	//1000000 B
const uint64_t fileBlockSize	= 10000; 	//10000 B
const uint64_t dirFileEntrySize = 100; 		//100 B

const uint8_t dcBit = 0x2; // 1<<dcBit-1 < datacenter number <= 1<<dcBit
const uint8_t serverPerDcBit = 0x7; // server number per datacenter = 1<<serverPerDcBit 



class Gateway;
class Central;
class Server;
class Client;

// I consider the path is only consisted of "/" and positive numbers
// which is  (/|(1-9)+)+ in regular expression
bool keyConvert(char* path, uint64_t &key)
{
    key = 0;
    for(int i = strlen(path)-1; i > 0 ; i--)
    {
	fprintf(stderr, "%c ", path[i]);
	if(path[i] == '/')
	    key *= 10;
	else
	{
	    key *= 10;
	    key += path[i]-'0';
	}
    fprintf(stderr, "%u\n", key);
    }
    fprintf(stderr, "%u\n", key);
}


#endif
