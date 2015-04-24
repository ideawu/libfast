CFLAGS=
OBJS = message.o field.o template.o
CXX=g++

all: $(OBJS)
	g++ test.cpp $(OBJS) ../fix/message.o
	
.cpp.o:
	$(CXX) -c $< -o $@
