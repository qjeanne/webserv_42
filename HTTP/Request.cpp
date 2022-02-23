#include "Request.hpp"

Request::Request(std::string const &str) : _method(""), _protocol_version(""), _path(""), _port("80"), _param(""), _headers(), _body(""), _line(str), _code(200)
{
	unsigned long		i;
	unsigned long		j;

	i = _line.find("\r\n");
	if (i == std::string::npos)
		this->_code = 400;
	else
	{
		takeMethod(_line.substr(0, i));
		j = _line.find("\r\n\r\n");
		if (j == std::string::npos)
			this->_code = 400;
		else
		{
			createHeaders();
			takeHeaders(_line.substr(i + 2, j - (i + 2)));
			takeBody(_line.substr(j + 4));
		}
	}
	takePort();
	if (!(checkMethod() && checkProtocolVersion()))
		this->_code = 400;
}
Request::~Request() {}

const std::string	&Request::getMethod() const { return this->_method; }
const std::string	&Request::getPath() const { return this->_path; }
const std::string	&Request::getPort() const { return this->_port; }
const std::string	&Request::getParam() const { return this->_param; }
const std::string	&Request::getProtocol_version() const { return this->_protocol_version; }
const std::map<std::string, std::string>	&Request::getHeaders() const { return this->_headers; }
const std::string	&Request::getBody() const { return this->_body; }
const std::string	&Request::getLine() const { return this->_line; }
int		Request::getCode() const { return this->_code; }
void	Request::setBody(std::string const &str) { this->_body = str; }

Request		&Request::operator =(Request const &other)
{
	this->_port = other.getPort();
	this->_param = other.getParam();
	this->_headers = other.getHeaders();
	this->_method = other.getMethod();
	this->_protocol_version = other.getProtocol_version();
	this->_path = other.getPath();
	this->_body = other.getBody();
	this->_line = other.getLine();

	return *this;
}

Request::Request(Request const &other)
{
	*this = other;
}

void	Request::takeMethod(std::string line)
{
	unsigned long		i;
	unsigned long		j;

	i = line.find(" /");
	if (i == std::string::npos)
		this->_code = 400;
	else
	{
		this->_method = _line.substr(0, i);
		j = line.find(" HTTP/");
		if (j == std::string::npos)
			this->_code = 400;
		else
		{
			takePath(line.substr(i + 1, j - (i + 1)));
			takeProtocolVersion(line.substr(j + 1));
		}
	}
}

void	Request::takeProtocolVersion(std::string line)
{
	this->_protocol_version = line;
}

void	Request::takePath(std::string line)
{
	unsigned long		i;

	i = line.find("?");
	if (i != std::string::npos)
	{
		this->_path = line.substr(0, i);
		takeParam(line.substr(i + 1));
	}
	else
		this->_path = line;
}

void	Request::takeParam(std::string line)
{
	this->_param = line;
}

void	Request::takeHeaders(std::string line)
{
	int				j;
	unsigned long	n;
	std::string		str;

	str = line;
	while (!str.empty())
	{
		j = str.find("\r\n");
		n = str.substr(0, j).find(": ");
		if (n == std::string::npos)
		{
			this->_code = 400;
			break;
		}
		if (this->_headers.find(str.substr(0, n)) != this->_headers.end())
			this->_headers[str.substr(0, n)] = str.substr(n + 2, j - (n + 2));
		if (j == -1)
			break;
		str = str.substr(j + 2);
	}
}

void	Request::takeBody(std::string line)
{
	this->_body = line;
}

void	Request::takePort()
{
	if (this->_headers["Host"] != "" && this->_headers["Host"].find(":") != std::string::npos && this->_headers["Host"].substr(this->_headers["Host"].rfind(":") + 1).size() > 0)
		this->_port = this->_headers["Host"].substr(this->_headers["Host"].rfind(":") + 1);
}

void	Request::createHeaders()
{
	this->_headers["Accept-Charsets"] = "";
	this->_headers["Accept-Language"] = "";
	this->_headers["Allow"] = "";
	this->_headers["Content-Language"] = "";
	this->_headers["Content-Length"] = "";
	this->_headers["Content-Location"] = "";
	this->_headers["Content-Type"] = "";
	this->_headers["Date"] = "";
	this->_headers["Host"] = "";
	this->_headers["Last-Modified"] = "";
	this->_headers["Location"] = "";
	this->_headers["Referer"] = "";
	this->_headers["User-Agent"] = "";
	this->_headers["Connection"] = "keep-alive";
}

bool	Request::checkMethod()
{
	std::vector<std::string>	array;

	array.push_back("GET");
	array.push_back("POST");
	array.push_back("DELETE");
	array.push_back("PUT");

	if (std::find(array.begin(), array.end(), this->_method) == array.end())
		return 0;
	return 1;
}

bool	Request::checkProtocolVersion()
{
	if (this->_protocol_version == "HTTP/1.1")
		return 1;
	this->_code = 505;
	return 0;
}
