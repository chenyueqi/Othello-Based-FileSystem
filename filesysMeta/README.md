# Brief Introduction
In this directory, I attempt to utilize metadata crawled from real in-use HDFS to generate my simulation testcases which are stored to ../testcase.

# Acknowledge
Thanks a lot to [PASA Lab of Nanjing University](http://pasa-bigdata.nju.edu.cn/) for providing metadata

# Details
metadata: 	original data  crawled from PASA's cluster  
filter.cc: 	extract useful data from metadata  
enlarge.cc: 	generate more data from the result of filter.cc  
readFilter.cc: 	generate read testcase from the result of enlarge.cc, including 1000 read operations  
touchFilter.cc:	generate touch tetscase from the result of enlarge.cc, including all operations  
writeFilter.cc: generate write tetscase from the result of enlarge.cc, including all operations  
rmFilter.cc:	generate rm testcase from the result of enlarge.cc, including 1000 rm operations  
mvFilter.cc: 	generate mv testcase from the result of enlarge.cc, including 1000 mv operations  
cpFilter.cc: 	generate cp tetscase from the result of enlarge.cc, including 1000 cp operations  
lsFilter.cc:	generate ls testcase from the result of enlarge.cc, including 100 ls operations  
mkdirFilter.cc:	generate mkdir testcase from the result of enlarge.cc, including all operations  
mvrFilter.cc:	generate mvr testcase from the result of enlarge.cc, including 100 mvr operations  
rmrFilter.cc:	generate rmr testcase from the result of enlarge.cc, including 100 rmr operations  
faultTolFilter.cc test fault tolerance by checking if files could be accessed via read operations  
scalaFilter.cc	test scalability by comparing  
