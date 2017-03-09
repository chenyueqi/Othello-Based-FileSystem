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

   void get_server_num(uint64_t id, uint64_t *server_num) {
	 // TODO query Othello for Yueqi Chen
	 return;
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
   // allocate a server for one directory according its father server
   uint64_t allocate_server(uint64_t *fa_server_num, uint64_t *server_num);
   // alloacte a id for one directory, that is add id-server_num pair to othello
   uint64_t allocate_id(uint64_t *server_num);
   // detele a id since it's directory has been deleted, that's delete id-server_num pair from othello
   void delete_id(uint64_t id);
};

// TODO complete allocate algorithm for Yueqi Chen
uint64_t Central::allocate_server(uint64_t *fa_server_num, uint64_t *server_num) {
  return 0;
}


// TODO complete delete id for Yueqi Chen
void Central::delete_id(uint64_t id) {
  return;
}

// TODO complete allocate id for Yueqi Chen
uint64_t Central::allocate_id(uint64_t *server_num)
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
	// TODO update othello
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

  for (int i = 0; i < 3; i++) {
	server_arr_->at(fa_server_num[i]).new_directory_entry(fa_path, path, server_num[i], id);
	server_arr_->at(server_num[i]).new_directory(path, id);
  }

  new_obj.insert(pair<string, uint64_t>(path, id));
  // add new key-value pair to Othello TODO
  update_othello_to_gateway();
  return true;
}

bool Central::rmr_proc(const string path, const uint64_t id,
	                   const string fa_path, const uint64_t fa_id,
				       map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3], server_num[3];
  get_server_num(id, server_num);
  get_server_num(fa_id, fa_server_num);
  map<string, uint64_t> old_obj_map;

  for (int i = 0; i < 3; i++) {
	server_arr_->at(fa_server_num[i]).delete_directory_entry(fa_path, path, old_obj_map);
	server_arr_->at(server_num[i]).delete_directory(path, old_obj_map);
  }

  for (map<string, uint64_t>::iterator iter = old_obj_map.begin();
	  iter != old_obj_map.end(); iter++) {
	old_obj.push_back(iter->first);
	// TODO delete all related id: id of all objects in the targeted directory
  }

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
  string new_path;
  int i;
  for (i = src_path.size();  i > 1 && src_path[i] != '/'; i--) {}
  new_path = des_path + src_path.substr(i, src_path.size());

  map<string, uint64_t> old_obj_map;

  for (int i = 0; i < 3; i++) {
	server_arr_->at(fa_src_server_num[i]).delete_directory_entry(fa_src_path, src_path, old_obj_map);
	server_arr_->at(src_server_num[i]).rename_directory(src_path, des_path, 
														new_obj, old_obj_map);
	server_arr_->at(des_server_num[i]).new_directory_entry(des_path, new_path, src_server_num[i], src_id);
  }

  for (map<string, uint64_t>::iterator iter = old_obj_map.begin();
	  iter != old_obj_map.end(); iter++) {
	old_obj.push_back(iter->first);
	// TODO modify all related id: id of all objects in the targeted directory
  }

  update_othello_to_gateway();
  return true;
}
#endif
