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
  bool dir_or_file; // dir: true, file: false
  uint64_t server_num; 
  uint64_t id; // for directory only
};

struct Obj {
  bool dir_or_file; // dir: true, file: false
  uint64_t size_or_id; // size for file, id for directory
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
	 server_arr_(server_arr),
 	 state_(true) {}

   uint64_t get_num() { return num_; }   
   uint64_t get_server_capacity() { return server_capacity_; }
   uint64_t get_available_capacity() { return avail_capacity_; }
   uint64_t get_used_capacity() { return used_capacity_; }
   bool get_state() {return state_;}
   bool set_num(uint64_t num){num_ = num;}


   // This is not a good design to explose all 
   // the operation interface, but in order to 
   // make it easy to implement, that's it
 public:
   bool new_file_entry(const string fa_path, const string path, 
	   			 	   uint64_t server_num);
   bool new_directory_entry(const string fa_path, const string path, 
	   			 	   		uint64_t server_num, uint64_t id);
   bool delete_file_entry(const string fa_path, const string path,
				   		  map<string, uint64_t> &old_obj);
   bool delete_directory_entry(const string fa_path, const string path,
				   		  		map<string, uint64_t> &old_obj);
   bool touch_file(const string fa_path, const string path);
   bool read_file(const string fa_path, const string path, uint64_t &size);
   bool delete_file(const string fa_path, const string path);
   bool write_file(const string fa_path, const string path, const uint64_t size);
   bool rename_file(const string src_path,const string new_path);
   bool new_directory(const string path, const uint64_t id);
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
   bool state_;

   bool use_storage(uint64_t capacity) {
	 avail_capacity_ -= capacity;
	 used_capacity_ += capacity;
   }

   bool free_storage(uint64_t capacity) {
	 avail_capacity_ += capacity;
	 used_capacity_ -= capacity;
   }
};

bool Server::new_file_entry(const string fa_path, const string path,
							uint64_t server_num) {
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  struct Entry new_entry;
  new_entry.dir_or_file = false;
  new_entry.server_num = server_num;
  new_entry.id = 0;
  fa_iter->second.entry_map.insert(pair<string, Entry>(path, new_entry));
  return true;
}

bool Server::new_directory_entry(const string fa_path, const string path,
								 uint64_t server_num, uint64_t id) {
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  struct Entry new_entry;
  new_entry.dir_or_file = true;
  new_entry.server_num = server_num;
  new_entry.id = id;
  fa_iter->second.entry_map.insert(pair<string, Entry>(path, new_entry));
  return true;
}

bool Server::delete_file_entry(const string fa_path, const string path, 
						  map<string , uint64_t> &old_obj) {
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  map<string, Entry>::iterator iter = fa_iter->second.entry_map.find(path);
  old_obj.insert(pair<string, uint64_t>(iter->first, iter->second.server_num));
  fa_iter->second.entry_map.erase(iter);
  return true;
}

bool Server::delete_directory_entry(const string fa_path, const string path, 
						  map<string , uint64_t> &old_obj) {
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  map<string, Entry>::iterator iter = fa_iter->second.entry_map.find(path);
  old_obj.insert(pair<string, uint64_t>(iter->first, iter->second.id));
  fa_iter->second.entry_map.erase(iter);
  return true;
}

bool Server::touch_file(const string fa_path, const string path) {
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  struct Entry new_entry;
  new_entry.dir_or_file = false;
  new_entry.server_num = num_; // touch file locally
  new_entry.id = 0;
  fa_iter->second.entry_map.insert(pair<string, Entry>(path, new_entry));
  struct Obj new_obj;
  new_obj.dir_or_file = false;
  new_obj.size_or_id = 0;
  obj_map_.insert(pair<string, Obj>(path, new_obj));
  return true;
}

bool Server::read_file(const string fa_path, const string path, uint64_t &size) {
  map<string, Obj>::iterator iter = obj_map_.find(path);
  if(iter != obj_map_.end()) { // this server has the file
	size = iter->second.size_or_id;
	return true;
  }
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  map<string, Entry>::iterator file_iter = fa_iter->second.entry_map.find(path);
  uint64_t server_num = file_iter->second.server_num;
  if (!server_arr_->at(server_num).get_state())
	return false;
  return server_arr_->at(server_num).read_file(fa_path, path, size);
}

bool Server::delete_file(const string fa_path, const string path) {
  map<string, Obj>::iterator iter = obj_map_.find(path);
  if(iter != obj_map_.end())  // this server has the file
	obj_map_.erase(iter);

  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  if(fa_iter != obj_map_.end()) { // father server
  	map<string, Entry>::iterator iter = fa_iter->second.entry_map.find(path);
	uint64_t server_num = iter->second.server_num;
	fa_iter->second.entry_map.erase(iter);
	if(server_num != num_)
	  return server_arr_->at(server_num).delete_file(fa_path, path);
  }
  return true;
}

// we use write file to new file in simulating, in this case, write is similar to touch
bool Server::write_file(const string fa_path, const string path, const uint64_t size) {
  map<string, Obj>::iterator fa_iter = obj_map_.find(fa_path);
  struct Entry new_entry;
  new_entry.dir_or_file = false;
  new_entry.server_num = num_; // write file locally
  new_entry.id = 0;
  fa_iter->second.entry_map.insert(pair<string, Entry>(path, new_entry));
  struct Obj new_obj;
  new_obj.dir_or_file = false;
  new_obj.size_or_id = size;
  obj_map_.insert(pair<string, Obj>(path, new_obj));
  return true;
}

bool Server::rename_file(const string src_path, const string new_path) {
  map<string, Obj>::iterator iter = obj_map_.find(src_path);
  assert(iter != obj_map_.end());
  struct Obj new_obj;
  new_obj.dir_or_file = iter->second.dir_or_file;
  new_obj.size_or_id = iter->second.size_or_id;
  obj_map_.erase(iter);
  obj_map_.insert(pair<string, Obj>(new_path, new_obj));
  return true;
}

bool Server::new_directory(const string path, const uint64_t id) {
  struct Obj new_obj;
  new_obj.dir_or_file = true;
  new_obj.size_or_id = id;
  obj_map_.insert(pair<string, Obj>(path, new_obj));
  return true;
}

// suffix = src_path's suffix
// new_path = des_path + suffix 
// set src_path as /home/user1/foo
// des_path as /home/user2
bool Server::rename_directory(const string src_path, const string des_path,
							  map<string, uint64_t> &new_obj_map, 
							  map<string, uint64_t> &old_obj_map) {
  map<string, Obj>::iterator iter = obj_map_.find(src_path);
  int i = 0;
  for (i = src_path.size(); i > 0 && src_path[i] != '/'; i--);
  string suffix = src_path.substr(i, src_path.size()); // suffix = /foo
  
  struct Obj new_obj;
  new_obj.dir_or_file = iter->second.dir_or_file;
  new_obj.size_or_id = iter->second.size_or_id;
  for (map<string, Entry>::iterator entry_iter = iter->second.entry_map.begin();
	   entry_iter != iter->second.entry_map.end(); entry_iter++) {
	// set entry_iter->first as /home/user1/foo/bar
	struct Entry new_entry;
	new_entry.dir_or_file = entry_iter->second.dir_or_file; 
	new_entry.server_num = entry_iter->second.server_num;
	new_entry.id = entry_iter->second.id;

	int i = 0;
	for (i = entry_iter->first.size(); i > 0 && entry_iter->first[i] != '/'; i--);
	// new_path = /home/user2/foo/bar
	string new_path = des_path + 
	  				  suffix + 
					  entry_iter->first.substr(i, entry_iter->first.size()); 
	new_obj.entry_map.insert(pair<string, Entry>(new_path, new_entry));

	if(entry_iter->second.dir_or_file == true) {
	  server_arr_->at(new_entry.server_num).rename_directory(entry_iter->first, // /home/user1/foo/bar
		  													 des_path+suffix, // /home/user2/foo
		  													 new_obj_map, old_obj_map);
	  old_obj_map.insert(pair<string, uint64_t>(entry_iter->first, entry_iter->second.id));
	  new_obj_map.insert(pair<string, uint64_t>(new_path, new_entry.id));

	} else {
	  server_arr_->at(new_entry.server_num).rename_file(entry_iter->first, new_path);
	}
  }

  obj_map_.erase(iter);
  obj_map_.insert(pair<string, Obj>(des_path + suffix, new_obj));
  return true;
}

bool Server::delete_directory(const string path, map<string, uint64_t> &old_obj) {
  map<string, Obj>::iterator iter = obj_map_.find(path);
  assert(iter != obj_map_.end());
  for (map<string, Entry>::iterator entry_iter = iter->second.entry_map.begin();
	   entry_iter != iter->second.entry_map.end(); entry_iter++) {
	if(entry_iter->second.dir_or_file == true) {
	  server_arr_->at(entry_iter->second.server_num).delete_directory(entry_iter->first, old_obj);
	  old_obj.insert(pair<string, uint64_t>(entry_iter->first, entry_iter->second.id));
	}
	else 
	  server_arr_->at(entry_iter->second.server_num).delete_file(path, entry_iter->first);
  }
  obj_map_.erase(iter);
  return true;
}

bool Server::list_directory(const string path) {
  map<string, Obj>::iterator iter = obj_map_.find(path);
  assert(iter != obj_map_.end());
  for (map<string, Entry>::iterator entry_iter = iter->second.entry_map.begin();
	   entry_iter != iter->second.entry_map.end(); entry_iter++) {
	// do nothing
  }
  return true;
}
#endif
