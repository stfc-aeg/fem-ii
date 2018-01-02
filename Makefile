all: client_test server_test Fem2ControlTESTS

client_test: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -o client_test FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

server_test: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -o server_test FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

Fem2ControlTESTS: Fem2ControlTESTS.cpp Fem2ControlMsg.cpp
	g++ -o Fem2ControlTESTS Fem2ControlTESTS.cpp Fem2ControlMsg.cpp -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c