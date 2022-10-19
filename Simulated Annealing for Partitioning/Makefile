saprog: saprog.o
	g++ -o saprog saprog.o
	
saprog.o: saprog.cpp
	g++ -c -O3 -fpermissive -std=gnu++0x saprog.cpp
	
clean:
	rm saprog.o saprog
