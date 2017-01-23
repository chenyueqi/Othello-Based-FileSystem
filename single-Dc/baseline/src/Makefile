CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

CFILES = $(shell find . -maxdepth 1 -name "*.cc")
HFILES = $(shell find . -maxdepth 1 -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test: main
	./main ~/multi-dc/testcase/writeTestcase loadbalance > ../result/loadbalance

# 已经测试的
#	./main ~/multi-dc/testcase/scalaTestcase check > ../result/scala
#	./main ~/multi-dc/testcase/faultTolTestcase exit > ../result/faultTol
#	./main ~/multi-dc/testcase/mvrTestcase mvr > ../result/mvr
#	./main ~/multi-dc/testcase/rmrTestcase rmr > ../result/rmr
#	./main ~/multi-dc/testcase/mkdirTestcase mkdir > ../result/mkdir
#	./main ~/multi-dc/testcase/lsTestcase ls > ../result/ls
#	./main ~/multi-dc/testcase/rmTestcase rm > ../result/rm
#	./main ~/multi-dc/testcase/mvTestcase mv > ../result/mv
#	./main ~/multi-dc/testcase/readTestcase read > ../result/read
#	./main ~/multi-dc/testcase/cpTestcase cp > ../result/cp
#	./main ~/multi-dc/testcase/touchTestcase touch > ../result/touch
#	./main ~/multi-dc/testcase/writeTestcase write > ../result/write


clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
