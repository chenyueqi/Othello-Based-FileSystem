# Brief Introduction
In this directory, I try to use implement [consistant hash](http://www.cs.princeton.edu/courses/archive/fall07/cos518/papers/chash.pdf) and use it in a network file system

# Apparatus
I am considering using [CityHash of Google](https://github.com/google/cityhash) instead of CRC on the basis of the poor load balance of the later.

# Time Complexity Analysis
Directory File is not supported in this implement. According to orginal consistent hash, all files are considered as objects without relation. Thus, directory is also considered as an ordinary file here, but not a father of its sub file or sub direcctory. Based on this, We analyze the time complexity of distinct UNIX-like operations as follow:

touch: O(1) without data transmision  
write: O(1) with data transmission  
read: O(1) with data transmission  
rm: O(1)  
mv or rename: O(1) with data transmission  
copy: O(1) with data transmission  


list: O(N) with data transmission  
mkdir: O(1)  
rmdir: O(N)  
mvr: O(N) with data transmissiom  

N represents number of files in the system. 

Since we could not control the hash result of consistent hash, operations like mv or rename, mvr is in need of data transmission between servers. What's more, each file in the system is conisdered as a single objetc as I have mentioned. This results in O(N) complexity in directory related operations. To make it worse, consistent hash is unaware of cloud existence and , a gamut of cross datacenter transmission is unavoided, which definitely slow down performance.


 ./common.h  -- some typdef and definition of class'  
 ./conHash.h -- implementation fo consistent hash ring  
 ./client.h  -- client  
 ./gateway.h -- gateway, communication with server and client  
 ./server.h  -- server
 ./main.cc   -- includes main function
 ./Makefile  -- make tools
 ./README.md -- file you are opening


#output format
 Since we are using output for analyzing, output format matters
 
 touch:	# serverNum MSGSIZE #
 write: # serverNum MSGSIZE size #
 read: 	# serverNum MSGSIZE size #
 rm: 	# serverNum MSGSIZE #
 mv: 	# src des MSGSIZE MSGSIZE size #
 cp: 	# src des MSGSIZE MSGSIZE size #
 ls: 	# serverNum MSGSIZE size #
 mdkir: # serverNum MSGSIZE #
 rmdir: # serverNum MSGSIZE #
 mvr: 	# src des MSGSIZE MSGSIZE size #

 ERROR: ERROR!
 exit: 	# TOTAL ${servernum} servers have exited abruptedly #
	# ${serve list} #
 check: # serverNum size #
