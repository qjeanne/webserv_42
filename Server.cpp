//
// Created by Voncile Maricruz on 12/10/21.
//

#include "Server.hpp"

Server::Server()
{
	memset(fds, 0, sizeof(fds));
	rc = -1;
	len = 0; i = 0; j = 0;
	c_size = 0;
	timeout = (3 * 60 * 1000);
	close_conn = -1;
	end_server = FALSE, compress_array = FALSE;
	bzero(buffer, 65536);
	nfds = 0;


}

Server::~Server()
{}

int Server::poll_process()
{
	rc = poll(fds, nfds, timeout);

	if (rc < 0)
	{
		std::cerr << "poll() error";
		return(-1);
	}
	if (rc == 0)
	{
		std::cout << "timeout";
		return(-1);
	}


	c_size =  nfds;
	for (i = 0; i < c_size; i++)
	{
		if (fds[i].revents == 0)
			continue;

		if (fds[i].revents  != (fds[i].revents & (POLLIN | POLLHUP)))
		{
			printf("  Error! revents = %d\n", fds[i].revents);
			end_server = TRUE;
			break;
		}

		int checkListen = 0;
		ServerUnit *tmp;
		for (std::vector<ServerUnit>::iterator unit = servers.begin(); unit < servers.end(); ++unit)
		{
			if (fds[i].fd == unit->getListenFD())
			{
				checkListen = 1;
				tmp = &(*unit);
				break;
			}
			for(int d = 0; d < (int)unit->getClientFD().size(); ++d)
			{
				if (fds[i].fd == unit->getClientFD()[d])
					tmp = &(*unit);
			}
		}

		if (checkListen == 1)
		{
			std::cout << "Listening socket is readable" << std::endl;
			accept_process(*tmp);
		}
		else
		{
			close_conn = FALSE;
			do
			{
				if (read_process(*tmp) == -1)
				{
					break;
				}
				Request parse(tmp->getRequestStr());

				if (!tmp->getConf().redirect.empty())
				{
					for (std::vector<ServerUnit>::iterator unit = servers.begin(); unit < servers.end(); ++unit)
					{
						if (unit->getPort() == std::stoi(tmp->getConf().redirect.substr(tmp->getConf().redirect.find(":") + 1)))
						{
							Response resp(parse, unit->getConf());
							tmp->setResponse(resp.getResponse());
							break;
						}
					}
				}
				else
				{
					Response resp(parse, tmp->getConf());
					tmp->setResponse(resp.getResponse());
				}
				if (send_process(*tmp) == -1)
					break;
			} while (TRUE);
			if (close_conn)
			{
				close(fds[i].fd);
				fds[i].fd = -1;
				compress_array = TRUE;
			}
		}
	}
	return(1);
}

int Server::accept_process(ServerUnit &unit)
{
	int tmp = -1;
	do
	{
		tmp = accept(fds[i].fd, NULL, NULL);
		if(tmp > -1)
			fcntl(tmp, F_SETFL, O_NONBLOCK);
		std::cout << "Doing accept..."  << tmp << std::endl;
		if (tmp < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				std::cerr << "accept() failed" << std::endl;
				end_server = TRUE;
			}
			return (-1);
		}
		client_socketFD.push_back(tmp);
		fds[nfds].fd = tmp;
		fds[nfds].events = POLLIN;
		nfds++;
		unit.addClient(tmp);
	} while (tmp != -1);
	return 1;
}

int Server::read_process(ServerUnit &unit)
{
	size_t readed_bytes = 0;
	unit.cleanRequestStr();
	do
	{
	rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
	std::string parse_buf = static_cast<std::string>(buffer);
	if (rc < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			std::cerr << "recv failed" << std::endl;
			close_conn = TRUE;
		}
		return (-1);
	}
	if (rc == 0)
	{
		std::cout << "connection is closed" << std::endl;
		close_conn = TRUE;
		return (-1);
	}

	unit.addRequestStr(parse_buf);
	readed_bytes  = unit.getRequestStr().find("\r\n\r\n");

	if (readed_bytes != std::string::npos)
	{
		if (unit.getRequestStr().find("Content-Length: ") == std::string::npos)
		{
			if(unit.getRequestStr().find("Transfer-Encoding: chunked") != std::string::npos)
			{
				if (unit.getRequestStr().rfind("0\r\n\r\n") == std::string::npos)
					return (-1);
				else
					return (1);
			}
			return (1);
		}
		len = std::atoi(unit.getRequestStr().substr(unit.getRequestStr().find("Content-Length: ") + 16, 10).c_str());
		return(1);
	}
	else
		continue;
	} while (true);
}

void Server::ft_compress_array()
{
	for (i = 0; i < nfds; i++)
	{
		if (fds[i].fd == -1)
		{
			for(j = i; j < nfds; j++)
			{
				fds[j].fd = fds[j+1].fd;
			}
			i--;
			nfds--;
		}
	}
}

int Server::send_process(ServerUnit &unit)
{
	int sended_bytes = 0;

	do
	{
		rc = send(fds[i].fd, unit.getResponse().c_str(), unit.getResponse().length(), 0);
		if (rc > 0)
			sended_bytes += rc;
		if (rc < 0)
		{
			perror("  send() failed");
			close_conn = TRUE;
			return (-1);
		}
	} while (sended_bytes != int(unit.getResponse().size()));
	return (1);
}



void Server::close_fds()
{
	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}

int Server::get_end_server() const
{
	return (end_server);
}

int Server::preprocess()
{
	int q = 0;

	while((int)listen_socketFD.size() > q)
	{
		fds[q].fd = listen_socketFD[q];
		fds[q].events = POLLIN;
		fcntl(fds[q].fd, F_SETFL, O_NONBLOCK);
		q++;
	}
	timeout = (3 * 60 * 1000);
	return(1);
}

void Server::addServerUnit(ServerUnit &serv)
{
	servers.push_back(serv);
	listen_socketFD.push_back(serv.getListenFD());
	nfds++;
}

int Server::getCompressArr() const
{
	return compress_array;
}

void Server::setCompressArr(int t)
{
	compress_array = t;
}

void Server::setConfigs(std::vector<Serv> const &conf)
{
	s_configs = conf;
}