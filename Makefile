CXX = g++

SRCS = lexer.cpp

OBJS = $(SRCS:.cpp=.o)

MAIN = lexer

GENS = graph.dot graph.png

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(OBJS) -o $(MAIN)

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	$(RM) $(OBJS) $(MAIN) $(GENS)