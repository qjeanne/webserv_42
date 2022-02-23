#ifndef PARSERHTML_RESPONSE_HPP
#define PARSERHTML_RESPONSE_HPP

#include "../Server.hpp"
#include "Request.hpp"
#include "functional"
#include "sstream"
#include "sys/stat.h"
#include "dirent.h"
#include "unistd.h"
#include "../parser.hpp"
#include "../Cgi.hpp"


class Response
{
private:
	Request										_request;
	Location									_location;
	Serv										_serv;
	std::string									_response;
	std::string									_type;
	std::string									_protocol_version;
	int 										_code;
	std::map<int, std::string>					_status_message;
	std::map<std::string, int>					_define_methods;
	std::map<std::string, std::string>			_headers;
	std::string									_body;

	Response();
	Response(Response const &other);

	void										initStatusMessage();
	void										defineMethods();
	void										takeLocation();
	void										fillHeaders();
	void										handleError();
	const std::string							makeAllow() const;
	const std::string							makeContentType();
	void										startMethod();
	std::string									writeHeaders() const;
	void										Get();
	void										Post();
	void										Delete();
	void										Put();
	void										readContent();
	const std::string							getAutoIndexPage(std::string &path, std::string const &host, std::string const &port) const;
	void										writeContent(std::string const &str);
	std::string									readHtml(std::string const &path);
	const std::string							makeDate() const;
	int											isFile(std::string const &path) const;

public:
	Response(Request const &request, Serv const &conf);
	~Response();
	Response &operator =(Response const &other);

	const std::string							&getResponse() const;
};

#endif
