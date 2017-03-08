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
};

struct Obj {
  bool dir_or_file; // dir: true, file: false
  uint64_t size;
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


   // This is not a good design to explose all 
   // the operation interface, but in order to 
   // make it easy to implement, that's it
 public:
   bool new_entry(const string fa_path, const string path, 
	   			  uint64_t server_num, bool dir_or_file);
   bool rename_entry();
   bool delete_entry(const string fa_path, const string path,
	   				 map<string, uint64_t> &old_obj);
   bool touch_file(const string fa_path, const string path);
   bool read_file(const string fa_path, const string path, uint64_t &size);
   bool delete_file(const string fa_path, const string path);
   bool write_file(const string fa_path, const string path, const uint64_t size);
   bool rename_file(const string src_path,const string new_path);
   bool new_directory(const string path);
   bool rename_directory(const string src_path, const string des_path,
	   					 map<string , uint64_t> &new_obj, map<string, uint64_t> &old_obj);
   bool delete_directory(const string path, map<string, uint64_t> &old_obj);
   bool list_directory(const string path);
   

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

bool Server::new_entry(const string fa_path, const string path,
					   uint64_t server_num, bool dir_or_file) {
  return true;
}

bool Server::rename_entry() {
  return true;
}

bool Server::delete_entry(const string fa_path, const string path, 
						  map<string , uint64_t> &old_obj) {
  return true;
}

bool Server::touch_file(const string fa_path, const string path) {
  return true;
}

bool Server::read_file(const string fa_path, const string path, uint64_t &size) {
  return true;
}

bool Server::delete_file(const string fa_path, const string path) {
  return true;
}

bool Server::write_file(const string fa_path, const string path, const uint64_t size) {
  return true;
}

bool Server::rename_file(const string src_path, const string new_path) {
  return true;
}

bool Server::new_directory(const string path) {
  return true;
}

// suffix = src_path's suffix
// new_path = des_path + suffix 
bool Server::rename_directory(const string src_path, const string des_path,
							  map<string, uint64_t> &new_obj, map<string, uint64_t> &old_obj) {
  return true;
}

bool Server::delete_directory(const string path, map<string, uint64_t> &old_obj) {
  return true;
}

bool Server::list_directory(const string path) {
  return true;
}
#endif
