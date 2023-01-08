all:
	gcc -Wall server.c -o server.out -lm
	gcc -Wall client.c -o client.out -lm
clean:
	rm -f *.out
