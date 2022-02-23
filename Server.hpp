//
// Created by Voncile Maricruz on 12/10/21.
//

#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <fcntl.h>



#define TRUE             1
#define FALSE            0

#include "ServerUnit.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"

class Server {
private:

	struct pollfd fds[200];


	int nfds;
	char buffer[65536];//возможно /0 в конце

	//char * answer;
	int len, i, j, rc,  c_size;
	int    end_server, compress_array;
	int timeout;
	int close_conn;


	std::vector<int> client_socketFD;
	std::vector<int> listen_socketFD;

	std::vector<Serv> s_configs;
	std::vector<ServerUnit> servers;

	int accept_process(ServerUnit &unit);
	int read_process(ServerUnit &unit);
	int send_process(ServerUnit &unit);
public:
	Server();
	~Server();
	Server(Server const &src) ;


	int preprocess();
	int poll_process();



	void ft_compress_array();
	//void()

	void close_fds();

	int get_end_server() const;
	int getCompressArr() const;
	void setCompressArr(int t);
	void addServerUnit(ServerUnit &serv);
	void setConfigs(std::vector<Serv> const &conf);
};


#endif //TESTHTTP_SERVER_HPP


