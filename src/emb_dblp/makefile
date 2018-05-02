CC = g++
CFLAGS = -lm -pthread -Ofast -march=native -Wall -funroll-loops -Wno-unused-result -lgsl -lm -lgslcblas
LFLAGS = -lgsl -lm -lgslcblas
INCLUDES = -I/usr/local/include -Ilib/
LIBS = -L/usr/local/lib
BINDIR = ./bin

all: $(BINDIR) linelib.o main.o emb_dblp

$(BINDIR):
	mkdir -p $(BINDIR)

emb_dblp : linelib.o main.o
	$(CC) $(CFLAGS) -o $(BINDIR)/emb_dblp linelib.o main.o $(INCLUDES) $(LIBS) $(LFLAGS)

linelib.o : linelib.cpp
	$(CC) $(CFLAGS) -c linelib.cpp $(INCLUDES) $(LIBS) $(LFLAGS)

main.o : main.cpp linelib.o
	$(CC) $(CFLAGS) -c main.cpp $(INCLUDES) $(LIBS) $(LFLAGS)

clean :
	rm -rf *.o $(BINDIR)
