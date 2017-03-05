g++ -c communication.cpp -fpic -lpthread -Wall -Werror -ggdb -std=c++11 

g++ -c main-comms.cpp -fpic -lpthread -Wall -Werror -ggdb -std=c++11 
g++ communication.o main-comms.o -o comms-test-fpic -lpthread -Wall -Werror -ggdb -std=c++11 
