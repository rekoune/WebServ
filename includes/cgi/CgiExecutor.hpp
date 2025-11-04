#ifndef CGIEXECUTOR_HPP 
#define CGIEXECUTOR_HPP

#include <unistd.h>
#include <string>
#include <signal.h>

class CgiExecutor
{
	private :
		RequestContext				req_context;
		CgiResult					result;
		pid_t						pid;
		// size_t						start_time;
		int							result_fd;
		bool						done;



		// INTERNAL UTILS
		std::vector<std::string>	buildEnv();
		int							executeScript(std::vector<char>& result, HttpStatusCode& status,  char** envp, char **argv);


	public :
		// Utils 
		std::string	getServerPort(std::string	host);
		// Class
		CgiExecutor();
		~CgiExecutor();
		CgiExecutor(RequestContext& req_context);
		void	setContext(RequestContext&	req_context);


		int			run();
		CgiResult	readResult(size_t buffer_size);
		CgiResult	getResult();
		bool		isDone();


};

// TODO:
	// 1) build a session data struct (contains session id, and map < std::string, map<key, value> >);
	// 
	// 
	// 

#endif