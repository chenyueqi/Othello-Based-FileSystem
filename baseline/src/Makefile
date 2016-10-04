CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

CFILES = $(shell find . -maxdepth 1 -name "*.cc")
HFILES = $(shell find . -maxdepth 1 -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test: main
	./main ../../testcase/lsTestcase ls > ./result/ls
#	./main ../../testcase/cpTestcase cp > ./result/cp
#	./main ../../testcase/mvTestcase mv > ./result/mv
#	./main ../../testcase/rmTestcase rm > ./result/rm
#	./main ../../testcase/readTestcase read > ./result/read
#	./main ../../testcase/touchTestcase touch > ./result/touch
#	./main ../../testcase/mkdirTestcase mkdir > ./result/mkdir
#	./main ../../testcase/writeTestcase write > ./result/write
#	./main ../../testcase/mvrTestcase mvr > ./result/mvr
#	./main ../../testcase/rmrTestcase rmr > ./result/rmr

clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
