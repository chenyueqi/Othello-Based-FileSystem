CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

CFILES = $(shell find . -maxdepth 1 -name "*.cc")
HFILES = $(shell find . -maxdepth 1 -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main1 -D SYSTEM1 $(CFILES)
	$(CC) $(CFLAGS) -o main2 -D SYSTEM2 $(CFILES)

test: main
	./main1 ../../testcase/writeTestcase write > ./result/write1
	./main2 ../../testcase/writeTestcase write > ./result/write2
#	./main1 ../../testcase/faultTolTestcase read > ./result/faultTol
#	./main1 ../../testcase/scalaTestcase rmr > ./result/scala
#	./main1 ../../testcase/rmrTestcase rmr > ./result/rmr
#	./main1 ../../testcase/cpTestcase cp > ./result/cp
#	./main1 ../../testcase/mvTestcase mv > ./result/mv
#	./main1 ../../testcase/rmTestcase rm > ./result/rm
#	./main1 ../../testcase/mkdirTestcase mkdir > ./result/mkdir
#	./main1 ../../testcase/readTestcase read > ./result/read
#	./main1 ../../testcase/touchTestcase touch > ./result/touch
#	./main1 ../../testcase/mvrTestcase mvr > ./result/mvr
#	./main1 ../../testcase/lsTestcase ls > ./result/ls

clean: 
	rm -rf main1
	rm -rf main2
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
