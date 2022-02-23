//
// Created by Voncile Maricruz on 12/13/21.
//

#ifndef TESTHTTP_SERVERUNIT_HPP
#define TESTHTTP_SERVERUNIT_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <sys/ioctl.h>
#include <iostream>
#include "vector"
#include "parser.hpp"


class ServerUnit {
private:
	int port;
	Serv conf;
	struct sockaddr_in6 stSockAddr;
	int listen_socketFD;
	std::vector<int> client_socket;
	std::string _request;
	std::string _response;

public:
	ServerUnit();
	ServerUnit(int port, Serv conf);
	~ServerUnit();
	ServerUnit(ServerUnit const & src);
	ServerUnit &operator=(ServerUnit const  &src);

	int getListenFD();
	std::vector<int> getClientFD();

	void addClient(int fd);
	void cleanRequestStr();
	void addRequestStr(std::string str);
	std::string getRequestStr();
	void setResponse(std::string resp);
	std::string getResponse();
	Serv getConf();
	int getPort();
};


#endif //TESTHTTP_SERVERUNIT_HPP
