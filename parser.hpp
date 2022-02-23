#ifndef PARSER
#define PARSER

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include "unistd.h"

#include "parser.hpp"

#define FILE_OPEN "conf.conf"
#define OPEN_FIGURE 1
#define CLOS_FIGURE 0

# define GET 0
# define POST 1
# define DELETE 2
# define PUT 3


struct	Location{
	std::string			location;
	std::string			index;
	std::vector<size_t>	methods;
	std::string			root;
	bool				autoindex;
	int					max_body;
	std::string			exec;
};

struct	Serv{
	std::string				ip;
	int						port;
	std::string				server_name;
	std::string				error_page;
	std::vector<Location>	locations;
	std::string				redirect;
	char* 					path;
};


int num_space(std::string str);
int checker_meth(char *buf);
void parse_location(std::string line, bool openServ, bool openLoc, Location &loc);
void parse_server(bool isOpen, std::string line, Serv &serv);
void servCleaner(Serv &serv);
void locCleaner(Location &loc);
std::vector<Serv> parser();


#endif