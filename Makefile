CXX = g++
CXXFLAGS = -std=c++11

SRC = main.cpp scan.cpp parse.cpp analyze.cpp symtab.cpp
OBJS = $(SRC:.cpp=.o)
EXEC = main.exe

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $(EXEC) $(OBJS) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	del *.exe *.o $(EXEC) $(OBJS)


gcd: all
	./main tests/gcd.c

test: all
	./main tests/test.c

error1: all
	./main tests/error1.c 

error2: all
	./main tests/error2.c