/*
   server is responsible for store objects, including files and 
   directories. And also to accelerate the query 
   speed, we are consider introducing Othello to server to trace
   path_id-server_num. We will test hit rate of this after tons of
   mv and mvr commands
*/

#ifndef OTHELLO_SERVER_H__
#define OTHELLO_SERVER_H__

#include<iostream>
#include<string>
#include<cstdlib>
#include<map>
#include<vector>
#include<sstream>
#include<stack>
#include"common.h"

using namespace std;

struct Entry{
  bool dir_or_entry; // dir: true, file: false
  uint64_t server_num;
  uint64_t id;
};

struct Obj {
  bool dir_or_file; // dir: true, file: false
  uint64_t size;
  uint64_t id;
  map<string, Entry> entry_map; // for dir only
};

class Server{
 public:
   Server(uint64_t num, uint64_t capacity=defaultCapacity, 
	      vector<Server>* server_arr=NULL) :
	 num_(num), 
	 server_capacity_(capacity), 
	 avail_capacity_(defaultCapacity), 
	 used_capacity_(0),
	 server_arr_(server_arr) {}

   uint64_t get_num() { return num_; }   
   uint64_t get_server_capacity() { return server_capacity_; }
   uint64_t get_available_capacity() { return avail_capacity_; }
   uint64_t get_used_capacity() { return used_capacity_; }
   bool set_num(uint64_t num){num_ = num;}
   bool get_msg(const string op);

 private:
   uint16_t num_;
   uint64_t server_capacity_; // in Byte
   uint64_t avail_capacity_;
   uint64_t used_capacity_;
   vector<Server>* server_arr_; //server cluster
   map<string, Obj> obj_map_;

   bool use_storage(uint64_t capacity) {
	 avail_capacity_ -= capacity;
	 used_capacity_ += capacity;
   }

   bool free_storage(uint64_t capacity) {
	 avail_capacity_ += capacity;
	 used_capacity_ -= capacity;
   }
};

bool Server::get_msg(const string op) {
  return true;
  /*
  if (op == "add entry") {

  } else if (op == "delete entry") {

  } else if (op == "rename entry") {

  } else if (op == "new directory") {

  } else if (op == "new file") {

  } else if (op == "write file") {

  } else if (op == "read file") {

  } else if (op == "touch file") {

  } else if (op == "delete file" ) {

  } else if (op == "delete directory") {
  
  } else if (op == "list directory") {

  } else {
	fprintf(stderr, "INVALID Operation %s %d\n", __FILE__, __LINE__);
	return false;
  }
  */
}

#endif
