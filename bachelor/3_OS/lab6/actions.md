//последовательность команд в терминале для сборки

clang -Wall -c message.c -o message.o -L/usr/local/opt/ -lzmq

gcc -c -o vocabulary.o vocabulary.c

clang -Wall -c -fPIC knot.c -o knot.o 

clang -shared -Wall -o libknot.so knot.o vocabulary.o message.o -L/usr/local/opt/ -lzmq

gcc -c -o server.o server.c

cp libknot.so /usr/local/opt/

gcc -o client.o -c client.c 

clang -Wall -o client client.o -L/usr/local/opt/ -lzmq -lknot message.o

clang -Wall -o server server.o -L/usr/local/opt/ -lzmq -lknot message.o
