#ifndef CGI_HPP 
#define CGI_HPP

#include <unistd.h>

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


		int	run();		
		CgiResult	getResult(size_t buffer_size);
		bool	isDone();


};



#endif