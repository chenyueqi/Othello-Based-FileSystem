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
   bool set_central_server_arr(Central* central, vector<Server>* server_arr) {
	 central_ = central;
	 server_arr_ = server_arr;
   }
   bool get_msg(const string op, 
	            const string path, const string fa_path, const string des_path, 
	            const uint64_t id, const uint64_t fa_id, const uint64_t des_id_or_size, 
				map<string, uint64_t> &new_obj, vector<string> &old_obj);
   // TODO update othello and idBitMap for Yueqi Chen
   bool getUpdate();

 private:
   Central* central_;
   vector<Server>* server_arr_;
   void get_server_num(const uint64_t id, uint64_t *server_num) {	 
	 // TODO query Othello for Yueqi Chen
	 return;
   }

/*
   all command could be classfied into two parts, one of which
   is directly related to central while the rest could skip central:

   central:
   mkdir 	path-self	path-father	id-father
   rmr		path-self	id-self		path-father 	id-father
   mvr		path-src	id-src 		path-src-father	id-src-father 	path-des	id-des

   server:
   ls		path-self	id-self
   touch	path-self	path-father	id-father
   write	path-self	id-self		path-father		id-father	size
   read		path-self	id-self		path-fathe		id-father
   rm		path-self	id-self		path-fathe		id-father
   mv		path-src 	id-src 		path-src-father id-src-father 	path-des 	id-des
   cp		path-src 	id-src 		path-src-father id-src-father 	path-des 	id-des
*/
   bool mkdir_proc(const string path, const string fa_path, const uint64_t fa_id,
	               map<string, uint64_t> &new_obj, vector<string> &old_obj) {
	 return central_->get_msg("mkdir", path, fa_path, "", 
		                      0, fa_id, 0, 
							  new_obj, old_obj);
   }
   bool rmr_proc(const string path, const uint64_t id, 
	             const string fa_path, const uint64_t fa_id,
	             map<string, uint64_t> &new_obj, vector<string> &old_obj) {
	 return central_->get_msg("rmr", path, fa_path, "",
		                      id, fa_id, 0,
							  new_obj, old_obj);
   }
   bool mvr_proc(const string src_path, const uint64_t src_id,
	             const string fa_src_path, const uint64_t fa_src_id, 
			 	 const string des_path, const uint64_t des_id,
	             map<string, uint64_t> &new_obj, vector<string> &old_obj) {
	 return central_->get_msg("mvr", src_path, fa_src_path, des_path,
		 				      src_id, fa_src_id, des_id,
							  new_obj, old_obj);
   }

   // most parameters in these methods, such as id, fa_id shall be useless
   bool ls_proc(const string path, const uint64_t id);
   bool touch_proc(const string path, const string fa_path, const uint64_t fa_id,
	               map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool read_proc(const string path, const uint64_t id, 
	              const string fa_path, const uint64_t fa_id,
	              map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool rm_proc(const string path, const uint64_t id, 
	            const string fa_path, const uint64_t fa_id,
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool write_proc(const string path, const uint64_t id, 
	               const string fa_path, const uint64_t fa_id, 
				   const uint64_t size,
	               map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool mv_proc(const string src_path, const uint64_t src_id,
	            const string fa_src_path, const uint64_t fa_src_id, 
				const string des_path, const uint64_t des_id,
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool cp_proc(const string src_path, const uint64_t src_id,
	            const string fa_src_path, const uint64_t fa_src_id, 
				const string des_path, const uint64_t des_id,
	            map<string, uint64_t> &new_obj, vector<string> &old_obj);
   bool exit_proc(const unsigned int exit_num);
};

bool Gateway::get_msg(const string op, 
	                  const string path, const string fa_path, const string des_path, 
	                  const uint64_t id, const uint64_t fa_id, const uint64_t des_id_or_size, 
				      map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  // new_obj, old_obj shall be useless in gateway all the time
  if (op == "ls")
	return ls_proc(path, id);
  else if (op == "touch")
	return touch_proc(path, fa_path, fa_id, new_obj, old_obj);
  else if (op == "mkdir")
	return mkdir_proc(path, fa_path, fa_id, new_obj, old_obj);
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
  else if (op == "exit")
	return exit_proc(des_id_or_size);
  else 
	return false; // never reach here
}

bool Gateway::ls_proc(const string path, const uint64_t id) {
  uint64_t server_num[3];
  get_server_num(id, server_num);
  //ignore failure situation as there is no failure in normal test
  return server_arr_->at(server_num[0]).list_directory(path);
}

bool Gateway::touch_proc(const string path,const string fa_path, const uint64_t fa_id,
	                     map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3];
  get_server_num(fa_id, fa_server_num);
  for (int i = 0; i < 3; i++)
	server_arr_->at(fa_server_num[i]).touch_file(fa_path, path);
  return true;
}

bool Gateway::read_proc(const string path, const uint64_t id, 
	                    const string fa_path, const uint64_t fa_id,
	                    map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3];
  get_server_num(fa_id, fa_server_num);
  uint8_t cnt = 0;
  for (int i = 0; i < 3 ; i++) {
	uint64_t size = 0;
	if (!server_arr_->at(fa_server_num[i]).get_state()){
	  cnt++;
	  continue;
	}
	if (!server_arr_->at(fa_server_num[i]).read_file(fa_path, path, size))// faile
	  cnt++;
  }
  if (cnt == 3) {
	fprintf(stdout, "ERROR!");
	return false;
  }
  return true;
}

bool Gateway::rm_proc(const string path, const uint64_t id, 
	                  const string fa_path, const uint64_t fa_id,
	                  map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3];
  get_server_num(fa_id, fa_server_num);
  for (int i = 0; i < 3; i++)
	server_arr_->at(fa_server_num[i]).delete_file(fa_path, path);
  return true;
}

bool Gateway::write_proc(const string path, const uint64_t id, 
	                     const string fa_path, const uint64_t fa_id, 
				         const uint64_t size,
	                     map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_server_num[3];
  get_server_num(fa_id, fa_server_num);;
  for (int i = 0; i < 3; i++)
	server_arr_->at(fa_server_num[i]).write_file(fa_path, path, size);
  return true;
}

bool Gateway::mv_proc(const string src_path, const uint64_t src_id,
	                  const string fa_src_path, const uint64_t fa_src_id, 
				      const string des_path, const uint64_t des_id,
	                  map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_src_server_num[3], des_server_num[3];
  get_server_num(fa_src_id, fa_src_server_num);
  get_server_num(des_id, des_server_num);
  string new_path;
  int i = 0;
  for (i = src_path.size(); i > 1 && src_path[i] != '/'; i--) {}
  new_path = des_path + src_path.substr(i, src_path.size());
  for (int i = 0; i < 3 ; i++) {
	map<string, uint64_t> old_obj_map;
	server_arr_->at(fa_src_server_num[i]).delete_file_entry(fa_src_path, src_path, old_obj_map);
	assert(old_obj_map.size()!=0);
	server_arr_->at(old_obj_map.begin()->second).rename_file(src_path, new_path);
	server_arr_->at(des_server_num[i]).new_file_entry(des_path, new_path, 
												 	  old_obj_map.begin()->second);
  }
  return true;
}

bool Gateway::cp_proc(const string src_path, const uint64_t src_id,
	                  const string fa_src_path, const uint64_t fa_src_id, 
				      const string des_path, const uint64_t des_id,
	                  map<string, uint64_t> &new_obj, vector<string> &old_obj) {
  uint64_t fa_src_server_num[3], des_server_num[3];
  get_server_num(fa_src_id, fa_src_server_num);
  get_server_num(des_id, des_server_num);
  string new_path;
  int i = 0;
  for (i = src_path.size(); i > 1 && src_path[i] != '/'; i--) {}
  new_path = des_path + src_path.substr(i, src_path.size());
  for (int i = 0; i < 3 ; i++) {
	uint64_t size = 0;
	server_arr_->at(fa_src_server_num[i]).read_file(fa_src_path, src_path, size);
	server_arr_->at(des_server_num[i]).write_file(des_path, new_path, size);
  }
  return true;
}

bool Gateway::exit_proc(const unsigned int exit_num) {
  fprintf(stdout, "# TOTAL %u servers have exited abruptedly #\n#", exit_num);
  for (int i = 0; i < exit_num;) {
	uint32_t candidate = rand()%(server_arr_->size());
	if (server_arr_->at(candidate).get_state()) {
	  server_arr_->at(candidate).set_state(false);
	  fprintf(stdout, "%u ", candidate);
	  i++;
	}
  }
  fprintf(stdout, " #\n");
  return true;
}

#endif
