#include "../../includes/Request.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"


CgiExecutor::CgiExecutor()
{}

CgiExecutor::~CgiExecutor()
{}

CgiExecutor::CgiExecutor(Request& req, std::string script_path, std::string	server_software) 
	: request(req), script_path(script_path), server_software(server_software) //server_software  is which program/software this webserver is (nginx/apache...). ours be like "webserv/1.1" or some thing like that. I NEED IT IN THE ENVP FOR SCRIPT
{}


void	CgiExecutor::parseTarget(std::string& script_name, std::string& query )
{
	std::string	target = request.getRequestLine().target;
	size_t qpos = target.find('?');
	std::cout << qpos << std::endl;
	if (qpos != std::string::npos)
	{
		script_name = target.substr(0, qpos);
		query  = target.substr(qpos + 1) ;
	}
	else 
	{
		script_name = target;
		query = "";
	}

}

std::vector<std::string>	CgiExecutor::buildEnv()
{
	std::string target = request.getRequestLine().target;
	std::string					query;
	std::string					script_name;
	std::vector<std::string>	env;
	
	parseTarget(script_name, query);
	std::cout << script_name << std::endl;
	std::cout << query << std::endl;
	env.push_back("REQUEST_METHOD=" + request.getRequestLine().method);
	env.push_back("QUERY_STRING=" + query);
	env.push_back("SCRIPT_NAME=" + script_name);
	env.push_back("SERVER_PROTOCOL=" + request.getRequestLine().httpVersion);
	env.push_back("SERVER_SOFTWARE=" + server_software);
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	if (request.getHeaders().count("content-type"))
		env.push_back("CONTENT_TYPE=" + request.getHeaders()["content-type"]);
	if ( request.getHeaders().count("content-length"))
		env.push_back("CONTENT_LENGTH=" + request.getHeaders()["content-length"]);

	return env;
}






bool	CgiExecutor::run(std::vector<char>& result, int&	cgi_status )
{
	std::vector<std::string>	env = buildEnv();
	(void)result, (void)cgi_status;
	for ( std::vector< std::string>::iterator iter = env.begin(); iter != env.end(); iter++)
	{
		std::cout << *iter << std::endl;
	}

	return true;
}

// √ SERVER_SOFTWARE	webserv/1.1 (your server string)	class member √
// GATEWAY_INTERFACE	CGI/1.1	hardcoded
// REMOTE_ADDR	client IP address	from socket (if you have it)
// SERVER_NAME	Host header (or config)	request.getHeaders()["Host"]
// SERVER_PORT	Port server is listening on	from config/socket
// CONTENT_TYPE	For POST/PUT: Content-Type header	request.getHeaders()["Content-Type"]
// CONTENT_LENGTH	For POST/PUT: Content-Length header	request.getHeaders()["Content-Length"]
// HTTP_*	All HTTP headers as env vars	All headers, uppercase, '-'→'_'