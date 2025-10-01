#include "../../includes/RequestParser.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"


CgiExecutor::CgiExecutor()
{}

CgiExecutor::~CgiExecutor()
{}

CgiExecutor::CgiExecutor(RequestContext& req_context)
	: req_context(req_context)     						 //server_software  is which program/software this webserver is (nginx/apache...). ours be like "webserv/1.1" or some thing like that. I NEED IT IN THE ENVP FOR SCRIPT
{}

void	CgiExecutor::setContext(RequestContext&	req_context)
{
	this->req_context = req_context;
}



std::vector<std::string>	CgiExecutor::buildEnv()
{
	std::map<std::string, std::string> 		headers = req_context.headers;
	std::vector<std::string>				env;
	
	std::cout << "script_name: " << req_context.script_name << std::endl;
	std::cout << "query: " << req_context.query << std::endl;


	env.push_back("REQUEST_METHOD=" + req_context.req_line.method);
	env.push_back("QUERY_STRING=" + req_context.query);
	env.push_back("SCRIPT_NAME=" + req_context.script_name);
	env.push_back("SERVER_PROTOCOL=" + req_context.req_line.httpVersion);
	env.push_back("SERVER_SOFTWARE=webserv/1.1");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	// env.push_back("SERVER_NAME=" + /* getServerName() */);

	for (std::map<std::string, std::string>::iterator iter = headers.begin(); iter != headers.end(); iter++)
	{
		std::string	name = iter->first;
		std::string	value = iter->second;


		if ( name == "content-type")
			env.push_back("CONTENT_TYPE=" + headers["content-type"]);
		else if (name == "content-length")
			env.push_back("CONTENT_LENGTH=" + headers["content-length"]);
		else 
		{
			std::string env_name;
			char 		c;
			for (size_t i = 0; i < name.size(); i++)
			{
				c = name[i];
				if ( c > 'a' && c < 'z')
					env_name += (c - 32);
				else if (c == '-')
					env_name += '_';
				else
					env_name += (c - 32);
			}
			env_name += "=";
			env_name += value;
			env.push_back(env_name);
		}

	}

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


// REQUEST_METHOD 		√
// SCRIPT_NAME			√
// QUERY_STRING			√
// SERVER_PROTOCOL		√
// GATEWAY_INTERFACE	√
// SERVER_SOFTWARE		√
// SERVER_NAME	\
// SERVER_PORT  - >	from config file
// REMOTE_ADDR	/
// CONTENT_TYPE (for POST/PUT)		√
// CONTENT_LENGTH (for POST/PUT)	√
// All HTTP headers as HTTP_*		







