CFLAGS=
OBJS = message.o decoder.o
CXX=g++

all: $(OBJS)
	g++ test.cpp $(OBJS)
	
.cpp.o:
	$(CXX) -c $< -o $@
