CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

All: readAna touchAna writeAna

readAna: read_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
touchAna: touch_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
writeAna: write_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
lsAna: ls_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
mvAna: mv_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
mvrAna: mvr_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@

bread:	readAna
	./readAna ../baseline/result/read
btouch:	touchAna
	./touchAna ../baseline/result/touch
bwrite: writeAna
	./writeAna ../baseline/result/write
bls: lsAna
	./lsAna ../baseline/result/ls
bmv: mvAna
	./mvAna ../baseline/result/mv
bmvr: mvrAna
	./mvrAna ../baseline/result/mvr

clean: 
	rm -rf readAna
	rm -rf touchAna
	rm -rf writeAna
	rm -rf lsAna
	rm -rf mvAna
	rm -rf mvrAna
