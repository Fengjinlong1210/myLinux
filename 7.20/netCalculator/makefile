.PHONY:all
all:server client

server:server.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp
client:client.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp

.PHONY:clean
clean:
	rm -f server client