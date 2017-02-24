/*
   centralized control platform is the core component of 
   this design as it is responsible for allocating server for 
   directories according to the allocation algorithm. Noted 
   that centralized only process three commands: mkdir, rmr and 
   mvr cause only these commands would modify intermediate nodes
   of the filesystem tree.

   To achieve the goal, centralized control platform is expected
   to interact with different servers.
*/

#ifndef OTHELLO_CENTRAL_H__
#define OTHELLO_CENTRAL_H__

#include<iostream>
#include<sstream>
#include<vector>
#include<stack>
#include"common.h"
#include<map>
#include"server.h"
#include"Othello/othello.h"
//#include"./OldOthello/othelloV7.h"
using namespace std;

class Central {
 public:
   Central(vector<Server>* server_arr = NULL, Gateway* gateway = NULL) :
	 server_arr_(server_arr), gateway_(gateway), prekey(0), prevalue(0), 
	 oth(dcBit+serverPerDcBit, &prekey, 1, false, &prevalue, sizeof(uint64_t)) {
	   // pre-set the first bit of id pool for root directory
	   id_pool_[0] = 0x8000000000000000; 
	   for (int i = 1 ; i < 1024; i++)
		 id_pool_[i] = 0;
	 }

   bool set_gateway_server_arr(vector<Server>* server_arr, Gateway* gateway) {
	 server_arr_ = server_arr;
	 gateway_ = gateway;
   }

   bool get_msg(const string op, 
	            const string path, const string fa_path, const string des_path,
				const uint64_t id, const uint64_t fa_id, const uint64_t des_id, 
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool testOthello();

 private:
   uint64_t prekey;
   uint16_t prevalue;
   vector<Server>* server_arr_;
   Gateway* gateway_;
   Othello<uint64_t> oth;
   uint64_t id_pool_[1024]; // support up to 1024 * 64 = 2^18 different objects
   uint64_t get_id();

   uint64_t get_server_num(uint64_t id, uint64_t *server_num) {
	 return 0;
   }
	
   bool mkdir_proc(const string path, const string fa_path, const uint64_t fa_id,
				   map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool rmr_proc(const string path, const uint64_t id,
	             const string fa_path, const uint64_t fa_id,
				 map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool mvr_proc(const string src_path, const uint64_t src_id,
	             const string fa_src_path, const uint64_t fa_src_id,
				 const string des_path, const uint64_t des_id,
				 map<string, uint64_t> &new_obj, vector<string> &old_obj);

   bool update_othello_to_gateway() {
	 return true; // TODO
   }
   uint64_t allocate_server(uint64_t *fa_server_num, uint64_t *server_num);
   uint64_t allocate_id(uint64_t *fa_server_num);
   void delete_id(uint64_t id);
};

// TODO complete allocate algorithm for Yueqi Chen
uint64_t Central::allocate_server(uint64_t *fa_server_num, uint64_t *server_num) {
  return 0;
}

// TODO complete allocate id for Yueqi Chen
uint64_t Central::allocate_id(uint64_t *fa_server_num) {
  return 0;
}

// TODO complete delete id for Yueqi Chen
void Central::delete_id(uint64_t id) {
  return;
}

uint64_t Central::get_id()
{
    uint64_t id = 0;
    int i = 0;
    for( ; i < 1024 && id_pool_[i] == 0xffffffffffffffff; i++)
    // for( ; i < 1024 && id_pool_[i] == ~0L; i++)
	id += 64;
    uint64_t temp = id_pool_[i];

    int j = 0;
    for(; j < 64; j++){
	if(((temp<<j))>>63 == 0)
	    break;
    }

    id += j;

    id_pool_[i] |= (0x8000000000000000>>j);
    // id_pool_[i] |= (0x1UL<<63>>j);
    return id;
}

bool Central::testOthello() {
    uint16_t key = 0;
    fprintf(stderr, "%lu %s %d\n", oth.queryInt(key), __FILE__, __LINE__);
}

bool Central::get_msg(const string op, 
	                  const string path, const string fa_path, const string des_path,
				      const uint64_t id, const uint64_t fa_id, const uint64_t des_id, 
	                  map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  if(op == "mkdir")
	return mkdir_proc(path, fa_path, fa_id, new_obj, old_obj);
  else if (op == "rmr")
	return rmr_proc(path, id, fa_path, fa_id, new_obj, old_obj);
  else if (op == "mvr")
	return mvr_proc(path, id, fa_path, fa_id, des_path, des_id,
		            new_obj, old_obj);
  else
	return false;
}

bool Central:: mkdir_proc(const string path, const string fa_path, const uint64_t fa_id,
	                      map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3], server_num[3];
  get_server_num(fa_id, fa_server_num);
  allocate_server(fa_server_num, server_num);
  uint64_t id = allocate_id(server_num);

  // TODO send message to 3 father servers:  add new subdirectory entry
  // path, fa_path, server_num
  // TODO send message to 3 servers: add new directory
  // path

  new_obj.insert(pair<string, uint64_t>(path, id));
  update_othello_to_gateway();
  return true;
}

bool Central::rmr_proc(const string path, const uint64_t id,
	                   const string fa_path, const uint64_t fa_id,
				       map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3], server_num[3];
  get_server_num(id, server_num);
  get_server_num(fa_id, fa_server_num);

  // TODO send message to 3 father servers: remove subdirectory entry
  // path, fa_path
  // TODO send message to 3 servers: remove directory
  // path, old_obj + obj_id
  // TODO delete all related id: id of all objects in the targeted directory

  update_othello_to_gateway();
  return true;
}

bool Central::mvr_proc(const string src_path, const uint64_t src_id,
	                   const string fa_src_path, const uint64_t fa_src_id,
				       const string des_path, const uint64_t des_id,
				       map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t src_server_num[3], fa_src_server_num[3], des_server_num[3];
  get_server_num(src_id, src_server_num);
  get_server_num(fa_src_id, fa_src_server_num);
  get_server_num(des_id, des_server_num);

  // TODO send message to 3 father src servers: remove subdirectory entry
  // path, fa_path, 
  // TODO send message to 3 src servers: rename recursively
  // path, new_obj, old_obj
  // TODO send message to 3 des servers: add new subdirectory entry
  // path, fa_path, server_num
  // TODO update all related id

  update_othello_to_gateway();
  return true;
}
#endif
