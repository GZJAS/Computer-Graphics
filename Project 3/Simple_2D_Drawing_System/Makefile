OBJS = main.o Line.o Polygon.o Geometry.o threeD.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -w -c -std=c++11 $(DEBUG)

p2.out: $(OBJS)
	$(CC) -Wall -std=c++11 -o p2.out $(OBJS) -framework OpenGL -framework GLUT

main.o: main.cpp Bucket.hpp Point.hpp Edge.hpp Line.hpp Polygon.hpp threeD.hpp
	$(CC) $(CFLAGS) -o $@ main.cpp

Geometry.o: Geometry.cpp Geometry.hpp Point.hpp Edge.hpp Bucket.hpp
	$(CC) $(CFLAGS) -o $@ Geometry.cpp

Line.o: Line.cpp Line.hpp Geometry.hpp
	$(CC) $(CFLAGS) -o $@ Line.cpp 

Polygon.o: Polygon.cpp Polygon.hpp Geometry.hpp Line.hpp
	$(CC) $(CFLAGS) -o $@ Polygon.cpp

threeD.o: threeD.cpp threeD.hpp Geometry.hpp Line.hpp
	$(CC) $(CFLAGS) -o $@ threeD.cpp

clean: 
	rm -rf *.out *.o

tar: 
	tar cfv p2.tar Makefile Bucket.hpp Edge.hpp Geometry.hpp Geometry.cpp Line.cpp Line.hpp main.cpp Point.hpp Polygon.hpp Polygon.cpp threeD.cpp threeD.hpp shapes.jpg README.md
