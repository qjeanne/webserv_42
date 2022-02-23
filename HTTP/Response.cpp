#include "Response.hpp"

Response::Response(Request const &request, Serv const &conf) : _request(request), _location(), _serv(conf), _response(""), _type(""), _protocol_version(request.getProtocol_version()), _code(request.getCode())
{
	defineMethods();
	takeLocation();
	initStatusMessage();
	fillHeaders();

	if (_request.getMethod() == "PUT")
		_code = 200;
	if (_code == 505 || _code == 400 || _code == 404)
		handleError();
	else
	{
		startMethod();
		if (_code == 405 || _code == 413 || _code == 404)
			handleError();
		else
		{
			fillHeaders();
			_response = _protocol_version + " " + std::to_string(_code) + " " + _status_message[_code] + "\r\n" + writeHeaders() + "\r\n" + _response;
		}
	}
}

Response::~Response() {}

Response &Response::operator =(Response const &other)
{
	this->_body = other._body;
	this->_code = other._code;
	this->_headers = other._headers;
	this->_protocol_version = other._protocol_version;
	this->_response = other._response;
	this->_status_message = other._status_message;

	return *this;
}

void	Response::handleError()
{
	_response = readHtml(_serv.error_page + "/" + std::to_string(_code) + ".html");
	fillHeaders();
	_response = _protocol_version + " " + std::to_string(_code) + " " + _status_message[_code] + "\r\n" + writeHeaders() + "\r\n" + _response;
}

void	Response::initStatusMessage()
{
	_status_message[200] = "OK";
	_status_message[201] = "Created";
	_status_message[204] = "No Content";
	_status_message[400] = "Bad Request";
	_status_message[403] = "Forbidden";
	_status_message[404] = "Not Found";
	_status_message[405] = "Method Not Allowed";
	_status_message[413] = "Payload Too Large";
	_status_message[505] = "HTTP Version Not Supported";
}

void	Response::defineMethods()
{
	_define_methods["GET"] = 0;
	_define_methods["POST"] = 1;
	_define_methods["DELETE"] = 2;
	_define_methods["PUT"] = 3;
}

void	Response::startMethod()
{
	if (std::find(_location.methods.begin(), _location.methods.end(), _define_methods[_request.getMethod()]) == _location.methods.end())
		this->_code = 405;
	else if (_location.max_body != -1 && int(_request.getBody().size()) > _location.max_body)
		this->_code = 413;
	else
	{
		if (_request.getMethod() == "GET")
			Response::Get();
		else if (_request.getMethod() == "POST")
			Response::Post();
		else if (_request.getMethod() == "DELETE")
			Response::Delete();
		else
			Response::Put();
	}
}

void	Response::takeLocation()
{
	std::vector<Location>::iterator it;
	std::string						path;

	path = _request.getPath() + " ";
	it = _serv.locations.begin();
	while (it != _serv.locations.end())
	{
		if ((*it).location == path)
		{
			_location = *it;
			break;
		}
		it++;
	}
	if (isFile(_request.getPath().substr(1)) || _request.getMethod() == "PUT")
	{
		_location.methods.push_back(0);
		_location.methods.push_back(1);
		_location.methods.push_back(2);
		_location.methods.push_back(3);
		_location.index = _request.getPath().substr(1);
		_location.root = "";
		_location.max_body = -1;
		_location.autoindex = 0;
		_location.exec = "";
	}
	else if (_location.location == "")
		this->_code = 404;
}

void	Response::fillHeaders()
{
	_headers["Allow"] = makeAllow();
	_headers["Content-Language"] = _request.getHeaders().at("Content-Language");
	if (_response.size() > 0)
		_headers["Content-Length"] = std::to_string(_response.size());
	_headers["Content-Type"] = makeContentType();
	_headers["Date"] = makeDate();
	_headers["Location"] = "";
	if (_location.root != "")
		_headers["Location"] = _location.root + "/";
	_headers["Location"] += _location.index;
	_headers["Server"] = "webserv/1.0.0 (Unix)";
}

const std::string	Response::makeAllow() const
{
	std::string		answer;
	std::vector<size_t>::const_iterator	it;

	answer = "";
	it = _location.methods.begin();
	while (it != _location.methods.end())
	{
		if (answer != "")
			answer += ", ";
		if (*it == 0)
			answer += "GET";
		else if (*it == 1)
			answer += "POST";
		else if (*it == 2)
			answer += "DELETE";
		else
			answer += "PUT";
		it++;
	}

	return answer;
}

const std::string	Response::makeContentType()
{
	std::string		file;

	file = _location.index;
	if (_type != "")
		return _type;
	else
		_type = file.substr(file.rfind(".") + 1, file.size() - file.rfind("."));
	if (_type == "html")
		_type = "text/html";
	else if (_type == "css")
		_type = "text/css";
	else if (_type == "js")
		_type = "text/javascript";
	else if (_type == "jpeg" || _type == "jpg")
		_type = "image/jpeg";
	else if (_type == "png")
		_type = "image/png";
	else if (_type == "gif")
		_type = "image/gif";
	else if (_type == "bmp")
		_type = "image/bmp";
	else
		_type = "text/plain";
	return _type;
}

std::string		Response::writeHeaders() const
{
	std::map<std::string, std::string>::const_iterator	it;
	std::string											answer;

	it = _headers.begin();
	while (it != _headers.end())
	{
		if (it->second != "")
			answer += it->first + ": " + it->second + "\r\n";
		it++;
	}
	return answer;
}

void	Response::Get()
{
	std::string		str;
	bool			cgi_bool;

	cgi_bool = false;
	if (_location.exec != "")
		cgi_bool = cgi(&_request, _location, _serv.path);
	if (cgi_bool)
	{
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_type = "text/html";
		_response = _request.getBody();

		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else if  (_code == 200)
		readContent();
	else
		_response = readHtml(_serv.error_page + "/" + std::to_string(_code) + ".html");
}

void	Response::Post()
{
	bool			cgi_bool;

	cgi_bool = cgi(&_request, _location, _serv.path);
	if (cgi_bool)
	{
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_type = "text/html";
		_response = _request.getBody();

		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else
	{
		_code = 204;
		_response = "";
	}
}

void	Response::Delete()
{
	(void)_request;
	std::string		path;

	path = _location.root + "/" + _location.index;
	if (isFile(path))
	{
		if (remove(path.c_str()) == 0)
			_code = 204;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 403 || _code == 404)
		_response = this->readHtml(_serv.error_page + "/" + std::to_string(_code) + ".html");
}

void	Response::Put()
{
	std::string		str;

	str = _request.getBody();
	_response = "";
	writeContent(str);
	if (_code != 201 && _code != 204)
		_response = readHtml(_status_message[_code]);
}

void				Response::readContent()
{
	std::ifstream		file;
	std::string			path;
	std::string			root;
	std::stringstream	buf;

	_response = "";
	path = _location.root;
	root = _headers["Location"];

	if (isFile(root))
	{
		file.open(root.c_str(), std::ifstream::in | std::ifstream::binary);
		if (file.is_open() == false)
		{
			_response = readHtml(_serv.error_page + "/403.html");
			_code = 403;
			return;
		}
		buf << file.rdbuf();
		_response = buf.str();
		file.close();
	}
	else if (_location.autoindex)
	{
		_response = getAutoIndexPage(path, _request.getHeaders().at("Host"), _request.getPort());
		_type = "text/html";
	}
	else
	{
		_response = this->readHtml(_serv.error_page + "/404.html");
		_code = 404;
		return;
	}

	_code = 200;
}

const std::string	Response::getAutoIndexPage(std::string &path, const std::string &host, std::string const &port) const
{
	DIR			*dir;
	std::string	page;
	std::string link;

	page = "";
	dir = opendir(path.c_str());
	page = "<!DOCTYPE html>\n"
		   "<html>\n"
		   "<head>\n"
		   "<title>" + path + "</title>\n"
			"<head>\n"
			"<body>\n"
			"<h1>INDEX</h1>\n"
			"<p>\n";

	if (dir == NULL)
	{
		std::cerr << "Error: could not open [" << path << "]" << std::endl;
		return "";
	}

	if (path[0] != '/')
		path = "/" + path;

	for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
	{
		link = "\t\t<p><a href=\"http://" + host + ":" + port + path + "/" + dirEntry->d_name + "\">" + dirEntry->d_name + "</a></p>\n";
		page += link;
	}

	page += "</p>\n"
			"</body>\n"
			"</html>\n";
	closedir(dir);

	return page;
}

void			Response::writeContent(std::string const &str)
{
	std::ofstream	file;

	if (isFile(_request.getPath().substr(1)))
	{
		file.open(_request.getPath().substr(1).c_str());
		file << str;
		file.close();
		_code = 204;
		return;
	}
	else
	{
		file.open(_request.getPath().substr(1).c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
		{
			_code = 403;
			return;
		}

		file << str;
		file.close();
		_code = 201;
	}
}

std::string		Response::readHtml(std::string const &path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	_type = "text/html";
	if (isFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return "<!DOCTYPE html>\n<html><title>404</title><body><h1>Error page not found</h1>\n<p>There was an error finding your error page</p></body></html>\n";

		buffer << file.rdbuf();
		file.close();

		return buffer.str();
	}
	else
		return "<!DOCTYPE html>\n<html><title>404</title><body><h1>Error page not found</h1>\n<p>There was an error finding your error page</p></body></html>\n";
}

const std::string	Response::makeDate() const
{
	time_t		now;
	struct tm	tm;
	char		buf[100];
	std::string answer;

	now = time(0);
	tm = *gmtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %X", &tm);
	answer = buf;
	answer += " GMT";
	return answer;
}

int		Response::isFile(std::string const &path) const
{
	struct stat		s;

	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
			return 0;
		else if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

const std::string	&Response::getResponse() const { return _response; }