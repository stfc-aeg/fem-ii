all: client_test server_test

client_test: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -o client_test FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

server_test: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -o server_test FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST  -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c