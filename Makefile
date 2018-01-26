CC_static=arm-xilinx-linux-gnueabi-g++ -static
CC_dyn=arm-xilinx-linux-gnueabi-g++

all: client_test server_test

client_test: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o client_test FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

server_test: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o server_test FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST  -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-

fem-ii_client: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	$(CC_dyn) -std=c++11 -o fem-ii_client FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib -lzmq -lboost_date_time

fem-ii_server: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	$(CC_dyn) -std=c++11 -o fem-ii_server FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib -lzmq -lboost_date_time