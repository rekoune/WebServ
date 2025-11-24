#ifndef CGIEXECUTOR_HPP 
#define CGIEXECUTOR_HPP

#include <signal.h>
#include "SessionHandler.hpp"

static SessionHandler session = SessionHandler();

class CgiExecutor
{
	private :
		RequestContext				req_context;
		CgiResult					result;
		pid_t						pid;
		int							result_fd;
		bool						done;


		std::vector<std::string>	buildEnv();
		int							executeScript(char** envp, char **argv);


	public :
		std::string	getServerPort(std::string	host);

		CgiExecutor();
		CgiExecutor& operator=(const CgiExecutor& other);
		~CgiExecutor();
		CgiExecutor(RequestContext& req_context);
		void	setContext(RequestContext&	req_context);


		int			run();
		CgiResult	readResult(size_t buffer_size);
		CgiResult	getResult();
		bool		isDone();

		void cgiClean();


};

#endif