all:
	g++ server.cpp -o server.out
	g++ client.cpp -o client.out
clean:
	rm -f *.out
