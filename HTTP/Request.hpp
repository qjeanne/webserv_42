#ifndef PARSERHTML_REQUEST_HPP
#define PARSERHTML_REQUEST_HPP

#include "iostream"
#include "string"
#include "algorithm"
#include "map"
#include "vector"

class Request
{
private:
	std::string									_method;
	std::string									_protocol_version;
	std::string									_path;
	std::string									_port;
	std::string									_param;
	std::map<std::string, std::string>			_headers;
	std::string									_body;
	std::string									_line;
	int											_code;

	Request();

	void										takeMethod(std::string line);
	void										takeProtocolVersion(std::string line);
	void										takePath(std::string line);
	void										takeParam(std::string line);
	void										takeHeaders(std::string line);
	void										takeBody(std::string line);
	void										takePort();

	bool										checkMethod();
	bool										checkProtocolVersion();

	void										createHeaders();

public:
	Request(std::string const &str);
	~Request();
	Request(Request const &other);
	Request &operator =(Request const &other);

	const std::string							&getMethod() const;
	const std::string							&getPath() const;
	const std::string							&getPort() const;
	const std::string							&getParam() const;
	const std::string							&getProtocol_version() const;
	const std::map<std::string, std::string>	&getHeaders() const;
	const std::string							&getBody() const;
	const std::string							&getLine() const;
	int											getCode() const;
	void										setBody(std::string const &str);
};

#endif
