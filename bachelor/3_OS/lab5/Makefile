CC = gcc
FLAGS = -pthread -w -pipe -O2 -Wextra -Werror -Wall -Wno-sign-compare -pedantic -lm
all: run

run: libmy_array.so main5.o main5static.o
	$(CC) $(FLAGS) -o run-stat main5static.o -L. -lmy_array -Wl,-rpath,.
	$(CC) $(FLAGS) -o run-dyn main5.o -ldl
	
main5static.o: main5static.c
	$(CC) -c $(FLAGS) main5static.c

main5.o: main5.c
	$(CC) -c $(FLAGS) main5.c
arr.o: arr.c
	$(CC) -c -fPIC $(FLAGS) arr.c

libmy_array.so: arr.o
	$(CC) $(FLAGS) -shared -o libmy_array.so arr.o

clean:
	rm -f *.o run-stat run-dyn *.so