klprog: klprog.o
	g++ -o klprog klprog.o
	
klprog.o: klprog.cpp
	g++ -c -Ofast -fpermissive -std=gnu++0x klprog.cpp
	
clean:
	rm klprog.o klprog
