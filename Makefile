all: client_test server_test

client_test: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o client_test FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -L/usr/local/lib/ -I/usr/local/include -lboost_date_time -lzmq

server_test: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o server_test FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -L/usr/local/lib/ -I/usr/local/include -lboost_date_time -lzmq
