CC=g++
CFLAGS= -std=c++11 -Wno-unused-result

CFILES = $(shell find . -maxdepth 1 -name "*.cc")
HFILES = $(shell find . -maxdepth 1 -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test: main
	./main ../../testcase/mvrTestcase mvr
#	./main ../../testcase/rmrTestcase rmr
#	./main ../../testcase/rmTestcase rm
#	./main ../../testcase/mkdirTestcase mkdir
#	./main ../../testcase/writeTestcase write
#	./main ../../testcase/touchTestcase touch
#	./main ../../testcase/readTestcase read
#	./main ../../testcase/mvTestcase mv
#	./main ../../testcase/cpTestcase cp
#	./main ../../testcase/lsTestcase ls

clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
