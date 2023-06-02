OBJS = $(SRCS:.cpp=.o)

PNGS = graph.png *fa*.png

DOTS = graph.dot *fa*.dot

all: main

main: main.cpp libraries/lexer.hpp libraries/parser.hpp
	g++ main.cpp -o main

clean:
	$(RM) $(OBJS) main $(DOTS) $(PNGS) test scanner parser
clean_dots:
	$(RM) $(DOTS)