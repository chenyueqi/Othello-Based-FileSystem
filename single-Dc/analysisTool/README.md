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

# test target
 It is obvious that some operations share similar performance. For example, read and write. Therefore, we just checked the following operations for evaluation
 read: for file access
 mv: for file move and cp
 mvr: for directory mv
 loadbalance:
 faulttol:
