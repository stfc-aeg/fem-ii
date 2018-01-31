CC_static=arm-xilinx-linux-gnueabi-g++ -static
CC_dyn=arm-xilinx-linux-gnueabi-g++

fem-ii_client_mac: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o fem-ii_client_mac FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

fem-ii_server_mac: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o fem-ii_server_mac FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST  -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

fem-ii_client_arm: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	$(CC_dyn) -std=c++11 -o fem-ii_client_ar FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib -lzmq -lboost_date_time

fem-ii_server_arm: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp mem_reader.cpp
	$(CC_dyn) -std=c++11 -o fem-ii_server_arm FemServerTest.cpp Fem2ControlMsg.cpp mem_reader.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib -lzmq -lboost_date_time

fem-ii_client_linux: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp
	g++ -std=c++11 -o fem-ii_client_linux FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp -DMSGPACK_USE_BOOST -I/data/projects/fem-ii/host/include -L/data/projects/fem-ii/host/lib -lzmq -lboost_date_time