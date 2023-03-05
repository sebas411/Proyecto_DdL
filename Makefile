CXX = g++

SRCS = lexer.cpp

OBJS = $(SRCS:.cpp=.o)

MAIN = lexer

PNGS = graph.png *fa*.png

DOTS = graph.dot *fa*.dot

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(OBJS) -o $(MAIN)

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	$(RM) $(OBJS) $(MAIN) $(DOTS) $(PNGS)
clean_dots:
	$(RM) $(DOTS)