//
// Created by Voncile Maricruz on 12/13/21.
//

#include "ServerUnit.hpp"
#include <exception>

ServerUnit::ServerUnit()
{}

ServerUnit::ServerUnit(int port, Serv conf)
{
	this->port = port;
	this->conf = conf;
	int rc = 0;
	int on;


	listen_socketFD = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_socketFD < 0)
	{
		std::cerr << "socket() failed" << std::endl;
		throw std::exception();
	}

	rc = setsockopt(listen_socketFD, SOL_SOCKET, SO_REUSEADDR,
					(char *) &on, sizeof(on));
	if (rc < 0)
	{
		std::cerr << "setsockopt() failed" << std::endl;
		close(listen_socketFD);
		throw std::exception();
	}

	rc = ioctl(listen_socketFD, FIONBIO, (char *) &on);
	if (rc < 0)
	{
		std::cerr << "ioctl() failed" << std::endl;
		close(listen_socketFD);
		throw std::exception();
	}


	memset(&stSockAddr, 0, sizeof(stSockAddr));
	stSockAddr.sin6_family = AF_INET6;
	memcpy(&stSockAddr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	stSockAddr.sin6_port = htons(port);
	rc = bind(listen_socketFD,
			  (struct sockaddr *) &stSockAddr, sizeof(stSockAddr));
	if (rc < 0)
	{
		std::cerr << "bind() failed" << std::endl;
		close(listen_socketFD);
		throw std::exception();
	}


	rc = listen(listen_socketFD, 32);
	if (rc < 0)
	{
		std::cerr << "listen() failed" << std::endl;
		close(listen_socketFD);
		throw std::exception();
	}

}

ServerUnit &ServerUnit::operator=(ServerUnit const  &src)
{
	if (this != &src)
	{
		listen_socketFD = src.listen_socketFD;
		port = src.port;
		_request = src._request;
		conf = src.conf;
	}
	return *this;
}

ServerUnit::~ServerUnit()
{}

int ServerUnit::getListenFD()
{
	return listen_socketFD;
}

void ServerUnit::addClient(int fd)
{
	client_socket.push_back(fd);
}

void ServerUnit::cleanRequestStr()
{
	_request.erase();
}

void ServerUnit::addRequestStr(std::string str)
{
	_request +=str;
}

std::string ServerUnit::getRequestStr()
{
	return _request;
}

ServerUnit::ServerUnit(const ServerUnit &src)
{
	listen_socketFD = src.listen_socketFD;
	port = src.port;
	conf = src.conf;
	_request = src._request;
}

std::vector<int> ServerUnit::getClientFD()
{
	return (client_socket);
}

Serv ServerUnit::getConf()
{
	return conf;
}

void ServerUnit::setResponse(std::string resp)
{
	_response = resp;
}

std::string ServerUnit::getResponse()
{
	return _response;
}

int ServerUnit::getPort()
{
	return port;
}