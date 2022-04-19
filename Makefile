CXX=g++
CXXFLAGS=-std=c++11 -g

asl: asl.o
	$(CXX) $(CXXFLAGS) -o asl $^

asl.o: asl.cpp

clean : 
	rm *.o