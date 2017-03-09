# LeO vs. Consistant Hash in single Datacenter
We wish to draw comparison between LeO and Consistant Hash filesystem for the sake of highlighting LeO's performance in shrinking data transmission intradatacenter as well as LeO's less accessing servers especially in the directory operations

# output format
 Since we are using log for analyzing, output format matters

 touch: # touch-access ServerNum #	in Consistant Hash, it is one (three if considering replicaiton), but what about LeO?    
 write: # write-access ServerNum #  in Consistant Hash, it is one (three if considering replication), but what about LeO?  
 		# write-data-transmission size #  
 read: 	# read-access ServerNum #  in Consistant Hash, it is one if no error, but what about LeO?  
 		# read-data-transmission size #  
 rm:	# rm-access ServerNum #  in Consistant Hash, it is one (three if considering replication), but what about LeO?   
 mv: 	# mv-access ServerNum #  in Consistant Hash, it is two (six if considering replication), LeO shall be three (nine if considering replication)  
		# mv-data-transmission size #  LeO is expected as zero  
 cp:	# cp-access ServerNum #  in Consistant Hash, it is two (six if considering replication), LeO shall be three (nine if considering replication)  
 		# cp-data-transmission size #  
 ls: 	# ls-access ServerNum #  
 		# ls-hit ServerNum #  how many servers do have related file or directory  
 mkdir:	# mkdir-access ServerNum # in Consistant Hash, it is one (three if considering replicaiton), but what about LeO?  
 rmdir: # rmr/rm-access ServerNum #  
 		# rmr/rm-hit ServerNum #  
 mvr: 	# mvr-access ServerNum #
 		# mvr/mv-data-transmission size #  

 ERROR: ERROR!  test error in read operation
 exit:  # TOTAL ${servernum} servers have exited abruptedly #  
		# ${serve list} #  
 check: # serverNum size #
