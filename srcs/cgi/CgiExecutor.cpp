#include "../../includes/RequestParser.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"


CgiExecutor::CgiExecutor() :  pid(-1), result_fd(-1), done(false) 
{}

CgiExecutor::~CgiExecutor()
{
	if (this->pid > 0)
		if (waitpid(this->pid, NULL, WNOHANG))
		{
			std::cout << "==================== IM KILLING ====================\n";
			kill(this->pid, SIGKILL);
			waitpid(this->pid, NULL, 1);
		}
		
	// if (this->result_fd > 0)
	// 	close (result_fd);
}

CgiExecutor::CgiExecutor(RequestContext& req_context)
	: req_context(req_context) , done(false)    						 //server_software  is which program/software this webserver is (nginx/apache...). ours be like "webserv/1.1" or some thing like that. I NEED IT IN THE ENVP FOR SCRIPT
{}


CgiExecutor& CgiExecutor::operator=(const CgiExecutor& other)
{
	req_context = other.req_context;
	result = other.result;
	pid = other.pid;
	result_fd = other.result_fd;
	done  = other.done;
	
	return (*this);
}


void	CgiExecutor::setContext(RequestContext&	req_context)
{
	this->req_context = req_context;
}

bool	CgiExecutor::isDone()
{
	return done;
}


CgiResult	CgiExecutor::getResult()
{
	return result;
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
	env.push_back("QUERY_STRING=" + req_context.req_line.query);
	env.push_back("SCRIPT_NAME=" + Utils::getFileName(req_context.script_path));
	env.push_back("SERVER_PROTOCOL=" + req_context.req_line.httpVersion);
	env.push_back("SERVER_SOFTWARE=webserv/1.1");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_NAME=" + getServerName(host));
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
			env_name += "HTTP_";
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

			std::cout << "env_name==========="  << env_name << std::endl;
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


int	CgiExecutor::executeScript(std::vector<char>& body, HttpStatusCode&	status, char** envp, char **argv)
{
	(void)body;
	(void)status;

	int 	body_fd[2];
	int		reslt_fd[2];

	if (pipe(body_fd) == -1 ||  pipe(reslt_fd) == -1)
	{
		std::cerr << "pipe failed" << std::endl;
		this->result.status = INTERNAL_SERVER_ERROR;
		return -1 ;
	}

	this->pid = fork();
	if (pid == -1)
	{
		std::cerr << "fork failed" << std::endl;
		this->result.status = INTERNAL_SERVER_ERROR;
		return -1 ;
	}
	if (pid == 0)
	{
		//	child
		if (dup2(reslt_fd[1], 1) == -1)
		{
			std::cerr << "dup2 failed" << std::endl;
			exit(1);
		}
		close(reslt_fd[1]);
		close(reslt_fd[0]);

		if ( dup2(body_fd[0], 0) == -1 )
		{
			std::cerr << "dup2 failed" << std::endl;
			exit(1);
		}

		close (body_fd[0]);
		close (body_fd[1]);
		// 	execve
		execve(const_cast<const char *>(req_context.script_path.c_str()), argv, envp);
		std::cerr << "execve failed" << std::endl;
		perror("====== > EXECVE: ");
		exit(1);
	}
	else 
	{
		//	parent
		// get the body ready into a char*
		// write the body in the pipe 
		char *body_buffer = &req_context.body[0];

		size_t	len = std::atoi(const_cast<const char *>(req_context.headers["content-length"].c_str()));
		

		// TO_CHECK
		/* CHECK IS LEN IN THE SAME AS LENGTH IN HEADERS (CONTENT-LENGTH) */
		// if ( len > req_context.body.size())
		// 	len = req_context.body.size();

		write (body_fd[1], body_buffer,  len);
		close(body_fd[1]);
		close(body_fd[0]);


		// HANDLE EXITING OF THE CHILD IN THE BEGINNIG
		int	status;
		if (waitpid(pid, &status, WNOHANG) == pid)
		{
			result.status = INTERNAL_SERVER_ERROR;
			return -1 ;
		}
		
		
		
		close (reslt_fd[1]);
		this->result_fd = reslt_fd[0];
		return (reslt_fd[0]);	
	}	
}



CgiResult	CgiExecutor::readResult(size_t buffer_size)
{
	// read from the restl_pipe()
	std::vector<char> body(buffer_size);
	

	//if read is done close the FD or if it fails
	int read_return = read(this->result_fd, &body[0], buffer_size);
	if (read_return == 0)
	{
		close (result_fd);
		done = true;
		if (waitpid(pid, NULL, WNOHANG) == 0)
		{
			kill (pid, SIGKILL);
			waitpid(pid, NULL, 1);
		}
	}
	else if (read_return == -1)
	{
		std::cerr << "hahaha\n";
		result.status = INTERNAL_SERVER_ERROR;
		return result;
	}
	if (buffer_size > static_cast<size_t>(read_return))
		body.erase(body.begin() + read_return, body.end());
	result.body = body;
	result.status = OK;
	return (result);
}


int	CgiExecutor::run()
{
	if ( access(req_context.script_path.c_str(), X_OK) == -1)
	{
		result.status = FORBIDDEN;
		return -1;
	}
	std::vector<std::string>	env_vec = buildEnv();
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

	// Executing script
	result_fd = executeScript(result.body, result.status, envp, argv);
	
	return result_fd;
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







