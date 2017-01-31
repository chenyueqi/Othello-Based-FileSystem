CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

CFILES = $(shell find . -maxdepth 1 -name "*.cc")
HFILES = $(shell find . -maxdepth 1 -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test: main
	./main ../../testcase/writeTestcase loadbalance > ../result/loadbalance

# 已经测试的
#	./main ../../testcase/scalaTestcase check > ../result/scala
#	./main ../../testcase/faultTolTestcase exit > ../result/faultTol
#	./main ../../testcase/mvrTestcase mvr > ../result/mvr
#	./main ../../testcase/rmrTestcase rmr > ../result/rmr
#	./main ../../testcase/mkdirTestcase mkdir > ../result/mkdir
#	./main ../../testcase/lsTestcase ls > ../result/ls
#	./main ../../testcase/rmTestcase rm > ../result/rm
#	./main ../../testcase/mvTestcase mv > ../result/mv
#	./main ../../testcase/readTestcase read > ../result/read
#	./main ../../testcase/cpTestcase cp > ../result/cp
#	./main ../../testcase/touchTestcase touch > ../result/touch
#	./main ../../testcase/writeTestcase write > ../result/write


clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
