CFLAGS=
OBJS = message.o
CXX=g++

all: $(OBJS)
	g++ test.cpp $(OBJS)
	
.cpp.o:
	$(CXX) -c $< -o $@
