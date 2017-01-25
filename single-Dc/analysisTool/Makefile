CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

All: readAna touchAna writeAna

readAna: readAna.cc
	$(CC) $^ $(CFLAGS) -o $@
touchAna: touchAna.cc
	$(CC) $^ $(CFLAGS) -o $@
writeAna: writeAna.cc
	$(CC) $^ $(CFLAGS) -o $@
lsAna: lsAna.cc
	$(CC) $^ $(CFLAGS) -o $@

bread:	readAna
	./readAna ../baseline/result/read

btouch:	touchAna
	./touchAna ../baseline/result/touch

bwrite: writeAna
	./writeAna ../baseline/result/write

bls: lsAna
	./lsAna ../baseline/result/ls

clean: 
	rm -rf readAna
	rm -rf touchAna
	rm -rf writeAna
	rm -rf lsAna
