CC_static=arm-xilinx-linux-gnueabi-g++ -static
CC_dyn=arm-xilinx-linux-gnueabi-g++

fem-ii_client_mac: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp
	g++ -std=c++11 -o ../bin/fem-ii_client_mac FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp -DMSGPACK_USE_BOOST -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

fem-ii_server_mac: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp
	g++ -std=c++11 -o ../bin/fem-ii_server_mac FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp -DMSGPACK_USE_BOOST  -L/usr/local/lib/boost_1_61_0 -L/usr/local/lib/ -L/usr/local/lib/boost_1_61_0/stage/lib -lboost_date_time -lzmq -L/usr/local/lib/msgpack-c

fem-ii_client_arm: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp
	$(CC_dyn) -std=c++11 -o ../bin/fem-ii_client_arm FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp -DMSGPACK_USE_BOOST -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib -lzmq -lboost_date_time

fem-ii_server_arm: FemServerTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp mem_reader.cpp i2c_reader.cpp PayloadClasses.cpp
	$(CC_dyn) -std=c++11 -o ../bin/fem-ii_server_arm FemServerTest.cpp Fem2ControlMsg.cpp mem_reader.cpp i2c_reader.cpp MsgPackEncoder.cpp PayloadClasses.cpp -DMSGPACK_USE_BOOST -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib -lzmq -lboost_date_time

fem-ii_client_linux: FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp
	g++ -std=c++11 -o ../bin/fem-ii_client_linux FemClientTest.cpp Fem2ControlMsg.cpp MsgPackEncoder.cpp PayloadClasses.cpp -DMSGPACK_USE_BOOST -I${BOOST_ROOT}/include -I${ZEROMQ_ROOT}/include -I${MSGPACK_ROOT}/include -L${BOOST_ROOT}/lib -L${ZEROMQ_ROOT}/lib -L${MSGPACK_ROOT}/lib -lzmq -lboost_date_time

i2c-test_arm: i2c_reader.cpp i2c_tester.cpp
	$(CC_dyn) -std=c++11 -o ../bin/i2c-test_arm i2c_tester.cpp i2c_reader.cpp -I/u/dbu19518/develop/projects/xilinx/target/usr/include -L/u/dbu19518/develop/projects/xilinx/target/usr/lib

