CXX = g++ -g -Wall
CXXFLAGS = -std=c++11

SRC = main.cpp scan.cpp parse.cpp analyze.cpp symtab.cpp cgen.cpp code.cpp assembly.cpp
OBJS = $(SRC:.cpp=.o)
EXEC = main.exe

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $(EXEC) $(OBJS) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	del *.exe *.o $(EXEC) $(OBJS)

test1: all
	./main.exe tests/test1.c

test2: all
	./main.exe tests/test2.c