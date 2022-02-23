#include "parser.hpp"

int num_space(std::string str){
	int i = 0;

	while (str[i] == ' ' || str[i] == '\t')
		i++;
	return i;
}

int checker_meth(char *buf){
	if (strstr(buf, "GET"))
		return GET;
	if (strstr(buf, "POST"))
		return POST;
	if (strstr(buf, "DELETE"))
		return DELETE;
	if (strstr(buf, "PUT"))
		return PUT;
	return -1;
}

void parse_location(std::string line, bool openServ, bool openLoc, Location &loc){
	int start_f, meth;
	char *buf, *del;
	std::string str;
	char		tmp[1000];

	getcwd(tmp, 1000);
	if (openLoc && openServ){
		if ((start_f = line.find("location")) != -1){
			loc.location = line.substr(start_f + 9, line.find("{") - (start_f + 9));
		}
		if ((start_f = line.find("methods")) != -1){
			del = (char *)line.substr(start_f + 7).c_str();
			buf = strtok(del," \t");
			if ((meth = checker_meth(buf)) != -1)
				loc.methods.push_back(meth);
			while (buf){
				buf = strtok(0, " \t");
				if (buf && (meth = checker_meth(buf)) != -1)
					loc.methods.push_back(meth);
			}
		}
		if ((start_f = line.find("index")) != -1){
			start_f += num_space(line.substr(start_f + 5));
			loc.index = line.substr(start_f + 5);
		}
		if ((start_f = line.find("root")) != -1){
			start_f += num_space(line.substr(start_f + 4));
			loc.root = tmp + line.substr(start_f + 4);
		}
		if ((start_f = line.find("autoindex")) != -1){
			if (line.find("on") != line.npos)
				loc.autoindex = true;
			else if (line.find("off") != line.npos)
				loc.autoindex = false;
		}
		if ((start_f = line.find("max_body")) != -1){
			start_f += num_space(line.substr(start_f + 8));
			loc.max_body = std::stoi(line.substr(start_f + 8));
		}
		if ((start_f = line.find("exec")) != -1){
			start_f += num_space(line.substr(start_f + 4));
			loc.exec = line.substr(start_f + 4);
		}
	}
}

void parse_server(bool isOpen, std::string line, Serv &serv){
	int start_f, start_port;
	char		tmp[1000];

	getcwd(tmp, 1000);
	if (isOpen){
		if ((start_f = line.find("listen")) != -1){
			start_port = line.find(":");
			serv.ip = line.substr(start_f + 7, start_port - (start_f + 7));
			serv.port = std::stoi(line.substr(start_port + 1, line.find(";", start_port) - start_port - 1));
		} else if ((start_f = line.find("return")) != -1){
			serv.redirect = line.substr(start_f + 7);
		} else if ((start_f = line.find("error_page")) != -1){
			serv.error_page = tmp + line.substr(start_f + 11);
		} else if ((start_f = line.find("server_name")) != -1){
			serv.server_name = line.substr(start_f + 12);
		}
	}
}

void servCleaner(Serv &serv){
	serv.ip = "";
	serv.port = 0;
	serv.server_name = "";
	serv.error_page = "";
	serv.locations.erase(serv.locations.begin(), serv.locations.end());
	serv.redirect = "";
}

void locCleaner(Location &loc){
	loc.location = "";
	loc.index = "";
	loc.methods.erase(loc.methods.begin(), loc.methods.end());
	loc.root = "";
	loc.autoindex = false;
	loc.max_body = -1;
	loc.exec = "";
}

std::vector<Serv>	parser(){
	std::ifstream conf(FILE_OPEN);
	std::vector<Serv> vServs;
	std::string line;
	Serv serv;
	Location loc;
	bool check_loc = CLOS_FIGURE;
	bool check_serv = CLOS_FIGURE;

	locCleaner(loc);
	servCleaner(serv);
	while (getline(conf, line)) {
		if (line.find("server {") != line.npos)
			check_serv = OPEN_FIGURE;
		else if (line.find("server }") != line.npos)
			check_serv = CLOS_FIGURE;
		parse_server(check_serv, line, serv);
		if (line.find("location") != line.npos)
			check_loc = OPEN_FIGURE;
		else if (line.find("locend }") != line.npos)
			check_loc = CLOS_FIGURE;
		parse_location(line, check_serv, check_loc, loc);
		if (!check_loc && !loc.methods.empty()){
			serv.locations.push_back(loc);
			locCleaner(loc);
		}
		if (!check_serv && !serv.ip.empty()){
			vServs.push_back(serv);
			servCleaner(serv);
		}
	}
	return vServs;
}
