CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

All: filter enlarge touchFilter writeFilter readFilter rmFilter mvFilter cpFilter mkdirFilter lsFilter mvrFilter rmrFilter faultTolFilter scalaFilter

filter: filter.cc
	$(CC) $^ $(CFLAGS) -o $@

enlarge: enlarge.cc
	$(CC) $^ $(CFLAGS) -o $@
	
touchFilter: touchFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

writeFilter: writeFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

readFilter: readFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

rmFilter: rmFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

mvFilter: mvFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

cpFilter: cpFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

mkdirFilter: mkdirFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

lsFilter: lsFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

mvrFilter: mvrFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

rmrFilter: rmrFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

faultTolFilter: faultTolFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

scalaFilter: scalaFilter.cc
	$(CC) $^ $(CFLAGS) -o $@

gen:	filter enlarge touchFilter writeFilter readFilter rmFilter mvFilter cpFilter mkdirFilter lsFilter mvrFilter rmrFilter faultTolFilter scalaFilter
	./filter metadata briefdata
	./enlarge briefdata enlargeddata
	./touchFilter enlargeddata touchTestcase
	./writeFilter enlargeddata writeTestcase
	./readFilter enlargeddata readTestcase
	./rmFilter enlargeddata rmTestcase
	./mvFilter enlargeddata mvTestcase
	./cpFilter enlargeddata cpTestcase
	./mkdirFilter enlargeddata mkdirTestcase
	./lsFilter enlargeddata lsTestcase
	./mvrFilter enlargeddata mvrTestcase
	./rmrFilter enlargeddata rmrTestcase
	./faultTolFilter enlargeddata faultTolTestcase
	./scalaFilter enlargeddata scalaTestcase
	mv touchTestcase ../testcase
	mv writeTestcase ../testcase
	mv readTestcase ../testcase
	mv rmTestcase ../testcase
	mv mvTestcase ../testcase
	mv cpTestcase ../testcase
	mv mkdirTestcase ../testcase
	mv lsTestcase ../testcase
	mv mvrTestcase ../testcase
	mv rmrTestcase ../testcase
	mv faultTolTestcase ../testcase
	mv scalaTestcase ../testcase
	
clean: 
	rm -rf filter
	rm -rf enlarge
	rm -rf touchFilter
	rm -rf writeFilter
	rm -rf readFilter
	rm -rf rmFilter
	rm -rf mvFilter
	rm -rf cpFilter
	rm -rf mkdirFilter
	rm -rf lsFilter
	rm -rf mvrFilter
	rm -rf rmrFilter
	rm -rf faultTolFilter
	rm -rf scalaFilter
	rm briefdata
	rm enlargeddata
