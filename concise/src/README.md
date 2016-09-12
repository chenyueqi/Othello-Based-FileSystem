# Brief Introduction 
In this directory, I try to use a simulated network file system to verify effectiness of my design. My design is based on Othello, which is a data structure originally used in fast forward in SDN. Since [Ye Yu](https://github.com/sdyy1990) have evaluted othello's performance in fast forward function, I am confident Othello will also make effect in my design. 

Othello here plays a role similar to B+ tree in Linux file system, indicating servers related according to file/directory path. I embed Othello into gateway add  centralized panel, which respond to different categories of unix-like operations. The operations here include mkdir, ls, rm -r , mv -r, rm, mv, touch, cp, read, write etc.

# File Organization
I simulate centralized panel, client, gateway and server in respective head file in C++. "common.h" includes some typdef and declaration of some classes.

The directory named "Othello" contains crucial components of othello.

# Documentation 
Detailed documentions is now incomplete. It is highly probable that I will write a paper to an academic conference which could behave like a documentation.

# Acknowledge 
Thanks again for [Ye Yu](https://github.com/sdyy1990)'s contribution.
Thanks again for [PASA Lab](http://pasa-bigdata.nju.edu.cn/)

