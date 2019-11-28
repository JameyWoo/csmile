CXX = g++
CXXFLAGS = -std=c++11

SRC = main.cpp scan.cpp parse.cpp analyze.cpp symtab.cpp cgen.cpp code.cpp
OBJS = $(SRC:.cpp=.o)
EXEC = main.exe

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $(EXEC) $(OBJS) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	del *.exe *.o $(EXEC) $(OBJS)
