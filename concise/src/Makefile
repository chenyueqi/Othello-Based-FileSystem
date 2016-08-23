CC=g++
CFLAGS=  -Wno-unused-result

CFILES = $(shell find ./ -name "*.cc")
OBJS = $(CFILES:.cc=.o)

LDFLAGS= -lpthread

main: $(CFILES)
	$(CC) $(CFLAGS) -o main $(CFILES)

test:
	./main

clean: 
	rm -rf main
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f *~
