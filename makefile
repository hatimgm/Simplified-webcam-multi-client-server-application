all: client serveur

#compilation coté client
client : client.o sock.o sockdist.o
	g++ -o client client.o sock.o sockdist.o

client.o : client.cpp 
	g++ -c client.cpp

sock.o : sock.cc 
	g++ -c sock.cc

sockdist.o : sockdist.cc 
	g++ -c sockdist.cc

#compilateur coté serveur
serveur : serveur.o sock.o sockdist.o
	g++ -o serveur serveur.o sock.o sockdist.o -lpthread

serveur.o : serveur.cpp
	g++ -c serveur.cpp -lpthread

clean:
	rm *.o; rm client; rm serveur

