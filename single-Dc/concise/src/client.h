/*
   client is responsible for transmitting original command
   into acceptable format in gateway. To achieve this, 
   client shall be able to
    1. get access to gateway
    2. store object-value pair to complete transimission
    3. update object-value pair according to command's result

   Written by Yueqi Chen (yueqichen.0x0@gmail.com) in 
   Nanjing University
*/

#ifndef OTHELLO_CLIENT_H_
#define OTHELLO_CLIENT_H_

#include<cstdio>
#include<iostream>
#include<sstream>
#include"gateway.h"
using namespace std;

class Client {
 public:
   Client(Gateway* gateway = NULL) : gateway_(gateway) {}
   bool set_gateway(Gateway* gateway) {
     gateway_ = gateway; 
     // root directory is pre-set
     obj_id_.insert(pair<string, uint64_t>("/", 0));
   }

   bool send_msg(string message);

   bool test_obj_id() {
     for (map<string, uint64_t>::iterator iter = obj_id_.begin(); 
	  iter != obj_id_.end(); iter++)
       fprintf(stdout, "%s %u\n", iter->first.c_str(), iter->second);
   }

 private:
   Gateway* gateway_;
   // object is defined as file or directory
   map<string, uint64_t> obj_id_; 

   // query obj-value pair for specified path
   uint64_t get_id(string path) {
     map<string, uint64_t>::iterator iter = obj_id_.find(path);
     if (iter == obj_id_.end()) {
       fprintf(stderr, "object %s does not exist %s %d\n", 
	       path.c_str(), __FILE__, __LINE__);
       return 0;
     }
     return iter->second;
   }

   string get_fa_path(string path) {
     int i = 0;
     for (i = path.size(); i > 1 && path[i] != '/'; i--) {}
     return path.substr(0,i);
   }

   void update_obj_id(map<string, uint64_t> &new_obj, vector<string> &old_obj) {
    for (vector<string>::iterator vec_iter = old_obj.begin(); 
		 vec_iter != old_obj.end(); vec_iter++) {
	  map<string, uint64_t>::iterator map_iter = obj_id_.find(*vec_iter);
	  obj_id_.erase(iter1);
    }

    for (map<string, uint64_t>::iterator map_iter = new_obj.begin();
		 map_iter != new_obj.end(); map_iter++)
	  obj_id_.insert(pair<string, uint64_t>(map_iter->first, map_iter->second));
   }
/*

   // query obj-value pair for the parent of specified path
   uint64_t get_fa_id(string path) {
	 string fa_path = get_fa_path(path);
	 return get_id(fa_path);
   }
   */
};

/*
   the resouces needed of each command is listed as follow:
	ls:		path-self	id-self

	touch:	path-self	id-father
	mkdir:	path-self	id-father

	read:	path-self	id-self		path-father		id-father
	rm: 	path-self	id-self		path-father 	id-father
	rmr:	path-self	id-self		path-father 	id-father

	write:	path-self	id-self		path-father 	id-father	size

	mv:		path-src	id-src		path-src-father	id-src-father	path-des	id-des
	cp:		path-src	id-src		path-src-father	id-src-father	path-des	id-des
	mvr:	path-src 	id-src		path-src-father	id-src-father	path-des	id-des
*/
bool Client::send_msg(string message) {
    stringstream msg(message);
    string op;
    getline(msg, op, ' ');

    map<string, uint64_t> new_obj;
    vector<string> old_obj;

	if (op == "ls") {
	  string path;
	  getline(msg, path, ' ');
	  uint64_t id = get_id(path);
	  gateway_->get_msg(op, path, "", "", 
		                id, 0, 0, 
						new_obj, old_obj);
	} else if (op == "touch" || op == "mkdir") {
	  string path;
	  getline(msg, path, ' ');
	  uint64_t fa_id = get_id(get_fa_path(path));
	  gateway_->get_msg(op, path, "", "", 
		                0, fa_id, 0, 
						new_obj, old_obj);
	} else if (op == "read" || op == "rm" || op == "rmr") {
	  string path, fa_path;
	  getline(msg, path, ' ');
	  uint64_t id = get_id(path);
	  uint64_t fa_id = get_id(fa_path = get_fa_path(path));
	  gateway_->get_msg(op, path, fa_path, "", 
		                id, fa_id, 0,
						new_obj, old_obj);
	} else if (op == "write") {
	  string path, fa_path;
	  getline(msg, path, ' ');
	  uint64_t size = 0;
	  msg >> size;
	  uint64_t id = get_id(path);
	  uint64_t fa_id = get_id(fa_path = get_fa_path(path));
	  gateway_->get_msg(op, path, fa_path, "", 
		                id, fa_id, size, 
						new_obj, old_obj);
	} else if (op == "mv" || op == "cp" || op == "mvr") {
	  string src_path, des_path, fa_src_path;
	  getline(msg, src_path, ' ');
	  getline(msg, des_path, ' ');
	  uint64_t src_id = get_id(src_path);
	  uint64_t des_id = get_id(des_path);
	  uint64_t fa_src_id = get_id(fa_src_path = get_fa_path(src_path));
	  gateway_->get_msg(op, src_path, fa_src_path, des_path, 
		                src_id, fa_src_id, des_id, 
						new_obj, old_obj);

	} else {
	  fprintf(stderr, "INVALID operation %s %d\n", 
		      __FILE__, __LINE__);
	  return false;
    }

	update_obj_id(old_obj, new_obj);
	return true;
};

#endif
