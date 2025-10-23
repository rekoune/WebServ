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


std::string	getServerName(std::string	host)
{
	size_t			ddot_pos = host.find(":");
	std::string 	server_name;
	if (ddot_pos != std::string::npos)
		server_name = host.substr(0, ddot_pos);
	else 
		return (host);
	return (server_name);
}

std::string	CgiExecutor::getServerPort(std::string	host)
{
	size_t			ddot_pos = host.find(":");
	std::string 	port;
	if (ddot_pos != std::string::npos)
		port = host.substr(ddot_pos + 1);
	else if (req_context.req_line.httpVersion == "HTTP/1.1")
		return ("80");
	else 
		return ("443");
	return (port);
}

std::vector<std::string>	CgiExecutor::buildEnv()
{
	std::map<std::string, std::string> 		headers = req_context.headers;
	std::vector<std::string>				env;
	std::string								host;
	if ( headers.count("Host"))
		host = headers["Host"];
	// std::cout << "host: " << host << std::endl;
	
	// std::cout << "script_name: " << req_context.script_name << std::endl;
	// std::cout << "query: " << req_context.query << std::endl;


	env.push_back("REQUEST_METHOD=" + req_context.req_line.method);
	env.push_back("QUERY_STRING=" + req_context.query);
	env.push_back("SCRIPT_NAME=" + req_context.script_name);
	env.push_back("SERVER_PROTOCOL=" + req_context.req_line.httpVersion);
	env.push_back("SERVER_SOFTWARE=webserv/1.1");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_NAME=" + getServerName(host));
	std::cout << "================" << std::endl;
	std::cout << getServerName(host) << std::endl;
	env.push_back("SERVER_PORT=" + getServerPort(host));

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
				if ( c >= 'a' && c <= 'z')
					env_name += (c - 32);
				else if (c == '-')
					env_name += '_';
				else
					env_name += (c);
			}
			env_name += "=";
			env_name += value;
			env.push_back(env_name);
		}

	}
	return env;
}

char ** 	vectorToEnvp(std::vector<std::string>& env_vec, std::vector <char*>& env_char_ptr_vec)
{

	for (std::vector<std::string>::iterator i = env_vec.begin(); i != env_vec.end(); i++)
	{
		env_char_ptr_vec.push_back(const_cast<char*>(i->c_str()));
	}
	env_char_ptr_vec.push_back(NULL);
	
	return &env_char_ptr_vec[0];

}

bool	CgiExecutor::executeScript(std::vector<char>& result, int&	cgi_status, char** envp, char **argv)
{
	(void)cgi_status;
	(void)result;
	pid_t	pid;
	int 	body_fd[2];
	int		reslt_fd[2];

	if (pipe(body_fd) == -1)
	{
		std::cerr << "pipe failed" << std::endl;
		return false ;
	}

	if ( pipe(reslt_fd) == -1 )
	{
		std::cerr << "pipe failed" << std::endl;
		return false ;
	}

	pid = fork();
	if (pid == -1)
	{
		std::cerr << "fork failed" << std::endl;
		return false ;
	}
	if (pid == 0)
	{
		//	child
		if (dup2(reslt_fd[1], 1) == -1)
		{
			std::cerr << "dup2 failed" << std::endl;
			return false ;
		}
		close(reslt_fd[1]);
		close(reslt_fd[0]);

		if ( dup2(body_fd[0], 0) == -1 )
		{
			std::cerr << "dup2 failed" << std::endl;
			return false ;
		}

		close (body_fd[0]);
		close (body_fd[1]);
		// 	execve
		execve(const_cast<const char *>(req_context.script_path.c_str()), argv, envp);
		std::cerr << "execve failed" << std::endl;
		exit(1);
	}
	else 
	{
		//	parent
		// get the body ready into a char*
		// write the body in the pipe 
		char *body_buffer = &req_context.body[0];

		size_t	len = std::atoi(const_cast<const char *>(req_context.headers["content-length"].c_str()));
		if ( len > req_context.body.size())
			len = req_context.body.size();

		write (body_fd[1], body_buffer,  len);
		close(body_fd[1]);
		close(body_fd[0]);

		// read from the restl_pipe()
		char	buffer;

		close (reslt_fd[1]);
		while (read(reslt_fd[0], &buffer, 1) > 0)
		{
			result.push_back(buffer);
		}
		std::cout << std::endl;

		close (reslt_fd[0]);
		if ( waitpid(pid, &cgi_status, 0) == -1)
		{
			std::cout << "error in child process : " << std::endl;
		}
		WEXITSTATUS(cgi_status);
	}
	return true;

}



bool	CgiExecutor::run(std::vector<char>& result, int&	cgi_status )
{
	std::vector<std::string>	env_vec = buildEnv();
	(void)result, (void)cgi_status;
	// for ( std::vector< std::string>::iterator iter = env_vec.begin(); iter != env_vec.end(); iter++)
	// {
	// 	std::cout << *iter << std::endl;
	// }

	std::vector <char*> env_char_ptr_vec;
	char **envp = vectorToEnvp(env_vec,  env_char_ptr_vec);

	// * preparing argv
	std::vector<char *> args_vector;
	args_vector.push_back(const_cast<char*>(req_context.script_path.c_str()));
	args_vector.push_back(NULL);
	char **argv = &args_vector[0];

	if ( !executeScript(result, cgi_status, envp, argv))
		return false;

	
	(void)envp;


	// execve();
	return true;
}


// REQUEST_METHOD 		√
// SCRIPT_NAME			√
// QUERY_STRING			√
// SERVER_PROTOCOL		√
// GATEWAY_INTERFACE	√
// SERVER_SOFTWARE		√
// SERVER_NAME	/* \ */
// SERVER_PORT  - >	from config file
// REMOTE_ADDR	/
// CONTENT_TYPE (for POST/PUT)		√
// CONTENT_LENGTH (for POST/PUT)	√
// All HTTP headers as HTTP_*		







