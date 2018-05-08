CXX = clang++
SOURCES = main.cpp Breaker.cpp Brick.cpp Map.cpp
OBJS = $(SOURCES:.cpp=.o)
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
PARAMS = -g

all: breaker

release: PARAMS = -O3
release: breaker

breaker: $(OBJS)
	$(CXX) $(PARAMS) $(LIBS) $(OBJS) -o breaker

%.cpp: %.o

%.o: %.cpp
	$(CXX) -c $(PARAMS) $< -o $@

clean:
	rm -f *.o breaker
