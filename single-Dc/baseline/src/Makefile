CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

CFILES = $(shell find . -maxdepth 1 -name "*.cc")
HFILES = $(shell find . -maxdepth 1 -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test: main
	./main ~/multi-dc/testcase/readTestcase read > ../result/read
#	./main ~/multi-dc/testcase/cpTestcase cp > ../result/cp
#	./main1 ~/multi-dc/testcase/mvTestcase mv > ../result/mv
#	./main1 ~/multi-dc/testcase/rmTestcase rm > ../result/rm
#	./main1 ~/multi-dc/testcase/mkdirTestcase mkdir > ../result/mkdir
#	./main1 ~/multi-dc/testcase/readTestcase read > ../result/read
#	./main1 ~/multi-dc/testcase/touchTestcase touch > ../result/touch
#	./main1 ~/multi-dc/testcase/faultTolTestcase read > ../result/faultTol
#	./main1 ~/multi-dc/testcase/writeTestcase write > ../result/write
#	./main1 ~/multi-dc/testcase/rmrTestcase rmr > ../result/rmr
#	./main1 ~/multi-dc/testcase/mvrTestcase mvr > ../result/mvr
#	./main1 ~/multi-dc/testcase/lsTestcase ls > ../result/ls
#	./main1 ~/multi-dc/testcase/scalaTestcase check > ../result/scala1
#	./main2 ~/multi-dc/testcase/scalaTestcase check > ../result/scala2

clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
