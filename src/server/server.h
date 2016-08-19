#ifndef __SERVER_H__
#define __SERVER_H__

#include<iostream>
#include<cstdint>
#include<string>

using namespace std;

const uint64_t defaultCapacity = 0;

class server
{
    private:
	uint64_t serverCapacity;
	uint64_t availCapacity;
	uint64_t usedCapacity;

    public:
	server(): serverCapacity(defaultCapacity), availCapacity(serverCapacity), usedCapacity(0){} // constructor

	server(uint64_t capacity): serverCapacity(capacity), availCapacity(serverCapacity), usedCapacity(0){} // constructor

	uint16_t  getMessage(const string inst, const string path, const uint16_t attach); // message interface




};

#endif
