CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

All: readAna touchAna

readAna: readAna.cc
	$(CC) $^ $(CFLAGS) -o $@
touchAna: touchAna.cc
	$(CC) $^ $(CFLAGS) -o $@

bread:	readAna
	./readAna ../baseline/result/read

btouch:	touchAna
	./touchAna ../baseline/result/touch

clean: 
	rm -rf readAna
	rm -rf touchAna
