/*
   gateway is responsible for get acceptable format 
   command from client and specify these commands to
   acheive expected results. More importantly, gateway
   shall produce respective log for future analysis

   acceptable format is defined in the method get_msg()
   and log format is defined in the README.md

   Written by Yueqi Chen (yueqichen.0x0@gmail.com) in
   Nanjing University
*/

#ifndef OTHELLO_GATEWAY_H_
#define OTHELLO_GATEWAY_H_

#include<iostream>
#include<sstream>
#include<vector>
#include"central.h"
using namespace std;

class Gateway {
 public:
   Gateway(Central* central = NULL, vector<Server>* server_arr = NULL) : 
	       central_(central), server_arr_(server_arr) {}
   bool set_central_server_arr(Central* p1, vector<Server>* p2) :
           central_(central), server_arr_(server_arr) {}
   bool get_msg(const string op, 
	            const string path, const string fa_path, const string des_path, 
	            const uint64_t id, const uint64_t fa_id, const uint64_t des_id_or_size, 
				map<string, uint64_t> &new_obj, vector<string> &old_obj);
   // TODO update othello and idBitMap for Yueqi Chen
   bool getUpdate();

 private:
   Central* central_;
   vector<Server>* server_arr_;
   // TODO Othello for Yueqi Chen

   bool ls_proc(const string path, const uint64_t id);

   bool touch_proc(const string path, const uint64_t fa_id
	               map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool mkdir_proc(const string path, const uint64_t fa_id
	               map<string, uint64_t> &new_obj, vector<string> &old_obj);

   bool read_proc(const string path, const uint64_t id, 
	              const path fa_path, const uint64_t fa_id
	              map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool rm_proc(const string path, const uint64_t id, 
	            const path fa_path, const uint64_t fa_id
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool rmr_proc(const string path, const uint64_t id, 
	             const path fa_path, const uint64_t fa_id
	             map<string, uint64_t> &new_obj, vector<string> &old_obj);

   bool write_proc(const string path, const uint64_t id, 
	               const path fa_path, const uint64_t fa_id, 
				   const uint64_t size
	               map<string, uint64_t> &new_obj, vector<string> &old_obj);

   bool mv_proc(const string src_path, const uint64_t src_id,
	            const string fa_src_path, const uint64_t fa_src_id, 
				const string des_path, const uint64_t des_id
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool cp_proc(const string src_path, const uint64_t src_id,
	            const string fa_src_path, const uint64_t fa_src_id, 
				const string des_path, const uint64_t des_id
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool mvr_proc(const string src_path, const uint64_t src_id,
	             const string fa_src_path, const uint64_t fa_src_id, 
			 	 const string des_path, const uint64_t des_id
	             map<string, uint64_t> &new_obj, vector<string> &old_obj);
};

bool Gateway::get_msg(const string op, 
	                  const string path, const string fa_path, const string des_path, 
	                  const uint64_t id, const uint64_t fa_id, const uint64_t des_id_or_size, 
				      map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  if (op == "ls")
	return ls_proc(path, id);
  else if (op == "touch")
	return touch_proc(path, fa_id, new_obj, old_obj);
  else if (op == "mkdir")
	return mkdir_proc(path, fa_id, new_obj, old_obj);
  else if (op == "read")
	return read_proc(path, id, fa_path, fa_id, new_obj, old_obj);
  else if (op == "rm")
	return rm_proc(path, id, fa_path, fa_id, new_obj, old_obj);
  else if (op == "rmr")
	return rmr_proc(path, id, fa_path, fa_id, new_obj, old_obj);
  else if (op == "write")
	return write_proc(path, id, fa_path, fa_id, des_id_or_size, 
		              new_obj, old_obj);
  else if (op == "mv")
	return mv_proc(path, id, fa_path, fa_id, des_path, des_id_or_size, 
		           new_obj, old_obj);
  else if (op == "cp")
	return cp_proc(path, id, fa_path, fa_id, des_path, des_id_or_size, 
		           new_obj, old_obj);
  else if (op == "mvr")
	return mvr_proc(path, id, fa_path, fa_id, des_path, des_id_or_size, 
		           new_obj, old_obj);
  else 
	return false; // never reach here
}

#endif
