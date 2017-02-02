graphics.out: polygon.o line.o geometry.o main.o
	g++ -w -g -std=c++11 -o graphics.out polygon.o line.o geometry.o main.o -framework OpenGL -framework GLUT

polygon.o: Polygon.cpp Polygon.hpp Geometry.hpp Line.hpp Bucket.hpp Point.hpp
	g++ -w -g -c -std=c++11 -o polygon.o Polygon.cpp

line.o: Line.hpp Line.cpp Geometry.hpp Point.hpp
	g++ -w -g -c -std=c++11 -o line.o Line.cpp

geometry.o: Geometry.hpp Geometry.cpp Point.hpp Bucket.hpp
	g++ -w -g -c -std=c++11 -o geometry.o Geometry.cpp

main.o: main.cpp Bucket.hpp Point.hpp Line.hpp Polygon.hpp Geometry.hpp
	g++ -w -g -c -std=c++11 -o main.o main.cpp -framework OpenGL -framework GLUT

clean: 
	rm -rf *.out *.o