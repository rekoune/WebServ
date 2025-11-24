#include "../../includes/RequestParser.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"


void CgiExecutor::cgiClean()
{
	int ret;
	if (this->pid > 0)
	{
		if ((ret = waitpid(this->pid, NULL, WNOHANG)) == 0)
		{
			if (kill(this->pid, SIGKILL) == -1)
			{
				std::cerr << "kill failed: " << strerror(errno) << std::endl;
			}

			if (waitpid(this->pid, NULL, 0) == -1)
			{
				std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
			}
		}
		else if (ret == -1)
		{
			std::cerr << "waitpid(WNOHANG) failed: " << strerror(errno) << std::endl;
		}
	}
	if (this->result_fd > 0)
	{
		if (close (result_fd) == -1)
		{
				std::cerr << "close() failed: " << strerror(errno) << std::endl;
		}
	}
}


CgiExecutor::CgiExecutor() :  pid(-1), result_fd(-1), done(false) 
{}

CgiExecutor::~CgiExecutor()
{}

CgiExecutor::CgiExecutor(RequestContext& req_context)
	: req_context(req_context) , done(false)    			
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
	std::map<std::string, std::string> 		headers = req_context.headers;;
	session.fetchDataToHeaders(headers);
	std::vector<std::string>				env;
	std::string								host;
	if ( headers.count("Host"))
		host = headers["Host"];


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


int	CgiExecutor::executeScript(char** envp, char **argv)
{

	int 	body_fd[2];
	int		reslt_fd[2];

	if (pipe(body_fd) == -1 ||  pipe(reslt_fd) == -1)
	{
		std::cerr << "pipe failed" << std::endl;
		this->result.status = INTERNAL_SERVER_ERROR;
		done = true;
		return -1 ;
	}

	this->pid = fork();
	if (pid == -1)
	{
		std::cerr << "fork failed" << std::endl;
		this->result.status = INTERNAL_SERVER_ERROR;
		done = true;
		return -1 ;
	}
	if (pid == 0)
	{
		//	CHILD
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
		
		execve(const_cast<const char *>(req_context.script_path.c_str()), argv, envp);
		std::cerr << "execve failed: " << strerror(errno) <<  std::endl;
		exit(1);
	}
	else 
	{
		char *body_buffer = &req_context.body[0];

		size_t	len = req_context.body.size();
		


		write (body_fd[1], body_buffer,  len);
		close(body_fd[1]);
		close(body_fd[0]);


		int	status;
		if (waitpid(pid, &status, WNOHANG) == pid)
		{
			result.status = INTERNAL_SERVER_ERROR;
			done = true;
			return -1 ;
		}
		
		
		
		close (reslt_fd[1]);
		this->result_fd = reslt_fd[0];
		return (reslt_fd[0]);	
	}	
}


void parseSetCookie(std::map<std::string, std::string>& session_cookies, std::string header_line)
{
	header_line = cleanLineUtil(header_line.substr(11, header_line.size() - 11));

	long semi_colon  = header_line.find_first_of(";");
	std::string cookie = header_line.substr(0, semi_colon);


	parseCookieDirective(session_cookies, cookie);
}

void	replaceCookieHeaders(std::map<std::string, std::string>& session_cookies, std::vector<char>& body)
{
	long	header_pos = Utils::isContainStr(&body[0], body.size(), "\n\n", 2);
	if (header_pos == -1)
		return ;

	std::stringstream old_headers_stream(std::string(body.begin(), body.begin() + header_pos));

	std::string header_line;

	std::vector<char>	new_body;
	while (getline(old_headers_stream, header_line))
	{
		header_line += "\n";
		if (header_line.find("Set-Cookie") == 0)
		{
			parseSetCookie(session_cookies, header_line);
		}
		else 
		{
			Utils::pushInVector(new_body, header_line);
		}
	}
	Utils::pushInVector(new_body, "\n");

	Utils::pushInVector(new_body, &body[header_pos + 2], body.size() - header_pos - 2);

	body = new_body;
}


CgiResult	CgiExecutor::readResult(size_t buffer_size)
{

	std::vector<char> body(buffer_size);


	int read_return = read(this->result_fd, &body[0], buffer_size);
	if (read_return == 0)
	{
		std::map<std::string, std::string>& session_map = session.getData()[session.current_session_id];
		replaceCookieHeaders(session_map, result.body);
		result.headers.insert(std::make_pair("Set-Cookie",  "SESSION_ID=" + session.current_session_id + "; Path=/; HttpOnly"));

		close (result_fd);
		done = true;
		int ret;
		if ((ret = waitpid(pid, NULL, WNOHANG)) == 0)
		{
			if (kill (pid, SIGKILL) == -1)
				std::cerr << "kill failed in readResult " << strerror(errno) << std::endl;
			if (waitpid(pid, NULL, 0) == -1 )
				std::cerr << "waitpid failed in readResult " << strerror(errno) << std::endl;

		}
		else if (ret == -1)
			std::cerr << "waitpid(WNOHANG) failed: " << strerror(errno) << std::endl;

	}
	else if (read_return == -1)
	{
		std::cerr << "read failed in readResult" << std::endl;
		result.status = INTERNAL_SERVER_ERROR;
		done = true;
		return result;
	}
	Utils::pushInVector(result.body, &body[0], read_return);

	result.status = OK;
	return (result);
}


int	CgiExecutor::run()
{
	if ( access(req_context.script_path.c_str(), X_OK) == -1)
	{
		result.status = FORBIDDEN;
		done = true;
		return -1;
	}

	session.addSession(req_context.headers);

	std::vector<std::string>	env_vec = buildEnv();

	std::vector <char*> env_char_ptr_vec;
	char **envp = vectorToEnvp(env_vec,  env_char_ptr_vec);

	std::vector<char *> args_vector;
	args_vector.push_back(const_cast<char*>(req_context.script_path.c_str()));
	args_vector.push_back(NULL);
	char **argv = &args_vector[0];

	result_fd = executeScript(envp, argv);
	
	return result_fd;
}





