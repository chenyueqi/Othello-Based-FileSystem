CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

All: filter enlarge enlarg-multi touchFilter writeFilter readFilter rmFilter mvFilter cpFilter mkdirFilter lsFilter mvrFilter rmrFilter faultTolFilter scalaFilter

filter: filter.cc
	$(CC) $^ $(CFLAGS) -o $@

enlarge: enlarge.cc
	$(CC) $^ $(CFLAGS) -o $@
	
enlarge-multi: enlarge-multi.cc
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
	mv touchTestcase ../single-Dc/testcase
	mv writeTestcase ../single-Dc/testcase
	mv readTestcase ../single-Dc/testcase
	mv rmTestcase ../single-Dc/testcase
	mv mvTestcase ../single-Dc/testcase
	mv cpTestcase ../single-Dc/testcase
	mv mkdirTestcase ../single-Dc/testcase
	mv lsTestcase ../single-Dc/testcase
	mv mvrTestcase ../single-Dctestcase
	mv rmrTestcase ../single-Dc/testcase
	mv faultTolTestcase ../single-Dc/testcase
	mv scalaTestcase ../single-Dc/testcase
	
multi-gen:	filter enlarge-multi touchFilter writeFilter readFilter rmFilter mvFilter cpFilter mkdirFilter lsFilter mvrFilter rmrFilter faultTolFilter scalaFilter
	./filter metadata briefdata
	./enlarge-multi briefdata enlargeddata
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
	mv touchTestcase ../multi-Dc/testcase
	mv writeTestcase ../multi-Dc/testcase
	mv readTestcase ../multi-Dc/testcase
	mv rmTestcase ../multi-Dc/testcase
	mv mvTestcase ../multi-Dc/testcase
	mv cpTestcase ../multi-Dc/testcase
	mv mkdirTestcase ../multi-Dc/testcase
	mv lsTestcase ../multi-Dc/testcase
	mv mvrTestcase ../multi-Dc/testcase
	mv rmrTestcase ../multi-Dc/testcase
	mv faultTolTestcase ../multi-Dc/testcase
	mv scalaTestcase ../multi-Dc/testcase

clean: 
	rm -rf filter
	rm -rf enlarge
	rm -rf enlarge-multi
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
