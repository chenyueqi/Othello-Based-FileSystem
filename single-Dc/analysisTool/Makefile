CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

All: readAna touchAna writeAna lsAna mvAna mvrAna rmAna cpAna mkdirAna rmrAna faultolAna scalaAna

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
rmAna: rm_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
cpAna: cp_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
mkdirAna: mkdir_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
rmrAna: rmr_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
faultolAna: faultol_analyze.cc
	$(CC) $^ $(CFLAGS) -o $@
scalaAna: scala_analyze.cc
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
brm: rmAna
	./rmAna ../baseline/result/rm
bcp: cpAna
	./cpAna ../baseline/result/cp
bmkdir: mkdirAna
	./mkdirAna ../baseline/result/mkdir
brmr: rmrAna
	./rmrAna ../baseline/result/rmr
bfaultol: faultolAna
	./faultolAna ../baseline/result/faultTol
bscala: scalaAna
	./scalaAna ../baseline/result/scala1 ../baseline/result/scala2

clean: 
	rm -rf readAna
	rm -rf touchAna
	rm -rf writeAna
	rm -rf lsAna
	rm -rf mvAna
	rm -rf mvrAna
	rm -rf rmAna
	rm -rf cpAna
	rm -rf mkdirAna
	rm -rf rmrAna
	rm -rf faultolAna
	rm -rf scalaAna
