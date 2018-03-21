target:
	clear
	gcc -o main -pthread main.c

clean:
	rm main

run:
	./main 2 data