//
// Created by Voncile Maricruz on 12/24/21.
//

#ifndef TESTHTTP_CGI_HPP
#define TESTHTTP_CGI_HPP

#include "HTTP//Request.hpp"
#include "parser.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sys/fcntl.h>
#include <string>

char **envp_creator(Request *req, Location loc, char *path);
bool		reclose(int tmp, int save1, char **env);
bool cgi(Request *req, Location loc, char *path);
std::string stringConverter(std::string str);

#endif //TESTHTTP_CGI_HPP
