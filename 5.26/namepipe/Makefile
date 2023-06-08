.PHONY:all
all:newserver newclient

newserver:newserver.cc
	g++ -o $@ $^ -std=c++11
newclient:newclient.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f newnewserver newclient