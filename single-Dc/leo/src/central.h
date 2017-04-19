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
#include"Othello/muloth.h"
#include"Othello/io_helper.h"
//#include"./OldOthello/othelloV7.h"
using namespace std;

class Central {
 public:
   Central(vector<Server>* server_arr = NULL, Gateway* gateway = NULL) :
	 server_arr_(server_arr), 
	 gateway_(gateway), 
	 keys1_((uint64_t*)valloc(sizeof(uint64_t)*65536*2)),
	 keys2_((uint64_t*)valloc(sizeof(uint64_t)*65536*2)),
	 keys3_((uint64_t*)valloc(sizeof(uint64_t)*65536*2)),
	 values1_((uint64_t*)valloc(sizeof(uint64_t)*65536*2)),
	 values2_((uint64_t*)valloc(sizeof(uint64_t)*65536*2)),
	 values3_((uint64_t*)valloc(sizeof(uint64_t)*65536*2)),
	 oth1_(16, &keys1_[0], 0, false, &values1_[0], sizeof(values1_[0]), 0),
	 oth2_(16, &keys2_[0], 0, false, &values2_[0], sizeof(values2_[0]), 0),
	 oth3_(16, &keys3_[0], 0, false, &values3_[0], sizeof(values3_[0]), 0) {
	   // pre-set the first bit of id pool for root directory
	   srand((unsigned)(time(NULL)));
	   id_pool_[0] = 0x8000000000000000; 
	   for (int i = 1 ; i < 1024; i++)
		 id_pool_[i] = 0;

	   keys1_[0] = 0;
	   keys2_[0] = 0;
	   keys3_[0] = 0;
	   values1_[0] = rand()%server_num;
	   values2_[0] = rand()%server_num;
	   values3_[0] = rand()%server_num;
	   fprintf(stderr, "values1: %lu\n", values1_[0]);
	   fprintf(stderr, "values2: %lu\n", values2_[0]);
	   fprintf(stderr, "values3: %lu\n", values3_[0]);
	   oth1_.addkeys(1, &(values1_[0]), sizeof(values1_[0]));
	   oth2_.addkeys(1, &(values2_[0]), sizeof(values2_[0]));
	   oth3_.addkeys(1, &(values3_[0]), sizeof(values3_[0]));
	   id_location_map1_.insert(pair<uint64_t, uint64_t>(0, 0));
	   id_location_map2_.insert(pair<uint64_t, uint64_t>(0, 0));
	   id_location_map3_.insert(pair<uint64_t, uint64_t>(0, 0));
	 }

   bool set_gateway_server_arr(vector<Server>* server_arr, Gateway* gateway) {
	 server_arr_ = server_arr;
	 gateway_ = gateway;
   }

   bool get_msg(const string op, 
	            const string path, const string fa_path, const string des_path,
				const uint64_t id, const uint64_t fa_id, const uint64_t des_id, 
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool test_othello(uint64_t key);
   // alloacte a id for one directory, that is add id-server_num pair to othello
   uint64_t allocate_id(uint64_t *server_num);
   // detele a id since it's directory has been deleted, that's delete id-server_num pair from othello
   void delete_id(uint64_t id);

 private:
   vector<Server>* server_arr_;
   Gateway* gateway_;
   uint64_t* keys1_;
   uint64_t* keys2_;
   uint64_t* keys3_;
   uint64_t* values1_;
   uint64_t* values2_;
   uint64_t* values3_;
   Othello<uint64_t> oth1_;
   Othello<uint64_t> oth2_;
   Othello<uint64_t> oth3_;
   uint64_t id_pool_[1024]; // support up to 1024 * 64 = 2^18 different objects
   map<uint64_t, uint64_t> id_location_map1_;
   map<uint64_t, uint64_t> id_location_map2_;
   map<uint64_t, uint64_t> id_location_map3_;

   // TODO initiate these 
   double alpha1_;
   double alpha2_;
   double alpha3_;
   double theta_;

   unsigned int depth_max_;

   void get_server_num(uint64_t id, uint64_t *server_num) {
	 server_num[0] =  oth1_.queryInt(id);
	 server_num[1] =  oth2_.queryInt(id);
	 server_num[2] =  oth3_.queryInt(id);
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
   uint64_t allocate_server(uint64_t *fa_server_num, uint64_t *server_num, string path);

   unsigned int cal_path_depth(string path) {
	 unsigned int depth = 0;
	 for (int i = 0; i < path.size(); i++) {
	   if(path[i] == '/')
		 depth++;
	 }
	 return depth;
   }
};

uint64_t Central::allocate_server(uint64_t *fa_server_num, uint64_t *server_num, string path) {
  double relative_depth = (double)cal_path_depth(path)/(double)depth_max_;
  uint64_t average_aval_storage = 0;
  uint64_t total_aval_storage = 0;
  for (int i = 0; i < server_arr_->size(); i++)
	total_aval_storage += server_arr_->at(i).get_available_capacity();
  average_aval_storage = total_aval_storage/server_arr_->size();
  
  uint64_t average_fd_rate = 0;
  double total_fd_rate = 0;
  for (int i = 0; i < server_arr_->size(); i++) 
	total_fd_rate += server_arr_->at(i).get_fd_rate();
  average_fd_rate = total_fd_rate/server_arr_->size();

  for(int i = 0; i < 3; i++) {
  	double res = alpha1_ * relative_depth + 
	    		 alpha2_ * server_arr_->at(fa_server_num[i]).get_available_capacity()/average_aval_storage + 
				 alpha3_ * server_arr_->at(fa_server_num[i]).get_fd_rate()/average_fd_rate;
  	if (res>theta_) {
	  fa_server_num[i] = server_num[i];
  	} else {
	  double max_t = 0;
	  for (int j = 0; j < server_arr_->size(); j++) {
		double t = alpha2_ * server_arr_->at(fa_server_num[j]).get_available_capacity()/average_aval_storage + 
		    	   alpha3_ * server_arr_->at(fa_server_num[j]).get_fd_rate()/average_fd_rate;
		if (t > max_t)
		  fa_server_num[i] = j;
	  }
  	}
  }
  return 0;
}


void Central::delete_id(uint64_t id) {
  map<uint64_t, uint64_t>::iterator iter;

  iter  = id_location_map1_.find(id);
  assert(iter != id_location_map1_.end());
  values1_[iter->second] = values1_[oth1_.mykeycount-1];
  oth1_.removeOneKey(iter->second);
  id_location_map1_.erase(iter);
  
  iter = id_location_map2_.find(id);
  assert(iter != id_location_map2_.end());
  values2_[iter->second] = values2_[oth2_.mykeycount-1];
  oth2_.removeOneKey(iter->second);
  id_location_map2_.erase(iter);

  iter = id_location_map3_.find(id);
  assert(iter != id_location_map3_.end());
  values3_[iter->second] = values3_[oth3_.mykeycount-1];
  oth3_.removeOneKey(iter->second);
  id_location_map3_.erase(iter);

  // shall delete id, but we ignore it

  return;
}

uint64_t Central::allocate_id(uint64_t *server_num)
{
    uint64_t id = 0;
    int i = 0;
    for( ; i < 1024 && id_pool_[i] == 0xffffffffffffffff; i++)
  	  id += 64;
    uint64_t temp = id_pool_[i];

    int j = 0;
    for(; j < 64; j++){
	if((temp<<j)>>63 == 0)
	    break;
    }

    id += j;

    id_pool_[i] |= (0x8000000000000000>>j);

	id_location_map1_.insert(pair<uint64_t, uint64_t>(id, oth1_.mykeycount));
	id_location_map2_.insert(pair<uint64_t, uint64_t>(id, oth2_.mykeycount));
	id_location_map3_.insert(pair<uint64_t, uint64_t>(id, oth3_.mykeycount));
	keys1_[oth1_.mykeycount] = id;
	keys2_[oth2_.mykeycount] = id;
	keys3_[oth3_.mykeycount] = id;
	values1_[oth1_.mykeycount] = server_num[0];
	values2_[oth2_.mykeycount] = server_num[1];
	values3_[oth3_.mykeycount] = server_num[2];
	oth1_.addkeys(1, &values1_[0], sizeof(values1_[0]));
	oth2_.addkeys(1, &values2_[0], sizeof(values2_[0]));
	oth3_.addkeys(1, &values3_[0], sizeof(values3_[0]));
    return id;
}

bool Central::test_othello(uint64_t key) {
    fprintf(stderr, "%lu %s %d\n", oth1_.queryInt(key), __FILE__, __LINE__);
    fprintf(stderr, "%lu %s %d\n", oth2_.queryInt(key), __FILE__, __LINE__);
    fprintf(stderr, "%lu %s %d\n", oth3_.queryInt(key), __FILE__, __LINE__);
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
  int depth = cal_path_depth(path);
  if (depth > depth_max_)
	depth_max_ = depth;
  uint64_t fa_server_num[3], server_num[3];
  get_server_num(fa_id, fa_server_num);
  allocate_server(fa_server_num, server_num, path);
  uint64_t id = allocate_id(server_num);

  for (int i = 0; i < 3; i++) {
	server_arr_->at(fa_server_num[i]).new_directory_entry(fa_path, path, server_num[i], id);
	server_arr_->at(server_num[i]).new_directory(path, id);
  }

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
  map<string, uint64_t> old_obj_map;

  for (int i = 0; i < 3; i++) {
	server_arr_->at(fa_server_num[i]).delete_directory_entry(fa_path, path, old_obj_map);
	server_arr_->at(server_num[i]).delete_directory(path, old_obj_map);
  }

  for (map<string, uint64_t>::iterator iter = old_obj_map.begin();
	  iter != old_obj_map.end(); iter++) {
	old_obj.push_back(iter->first);
	delete_id(iter->second);
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
