CC=g++
CFLAGS=  -Wno-unused-result

CFILES = $(shell find ./ -name "*.cc")
HFILES = $(shell find ./ -name "*.h")
OBJS = $(CFILES:.cc=.o)

main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test: main
	./main

clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
