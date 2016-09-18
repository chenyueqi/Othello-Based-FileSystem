# Brief Introduction 
In this directory, I try to use a simulated network file system to verify effectiness of my design. My design is based on Othello, which is a data structure originally used in fast forward in SDN. Since [Ye Yu](https://github.com/sdyy1990) have evaluted othello's performance in fast forward function, I am confident Othello will also make effect in my design. 

Othello here plays a role similar to B+ tree in Linux file system, indicating servers related according to file/directory path. I embed Othello into gateway add  centralized panel, which respond to different categories of unix-like operations. The operations here include mkdir, ls, rm -r , mv -r, rm, mv, touch, cp, read, write etc.

# File Organization
I simulate centralized panel, client, gateway and server in respective head file in C++. "common.h" includes some typdef and declaration of some classes.

The directory named "Othello" contains crucial components of othello.

# Analysis
Based on the specific design, we analyze the time complexity of distinct UNIX-like operations as follow:

write: O(1) with data transmission  
read: O(1) with data transmission  
rm: O(1)  
mv or rename: O(1)  
copy: O(1) with data transmission  


list: O(1) with data transmission  
mkdir: O(1)  
rmdir: O(1)  
mvr: O(1) 

Apparently, all the operations here is O(1), which means few accesses to server could satisfy all requirement. What's more, unnecessary data transmisson is unavoided to decrease communication between servers. And also, it enjoys priori knowledge of the organization of datacenters, making it possible to shrink currency between different datacenters.

More detailed analysis of sever access time will be given in the future.

# Documentation 
Detailed documentions is now incomplete. It is highly probable that I will write a paper to an academic conference which could behave like a documentation.

# Acknowledge 
Thanks again for [Ye Yu](https://github.com/sdyy1990)'s contribution.

Thanks again for [PASA Lab](http://pasa-bigdata.nju.edu.cn/)

