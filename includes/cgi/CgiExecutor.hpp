#ifndef CGI_HPP 
#define CGI_HPP

class CgiExecutor
{
	private :
		RequestContext				req_context;

		std::vector<std::string>	buildEnv();
		void						executeScript(std::vector<char>& result, int&	cgi_status );


		// void						parseTarget(std::string& script_name, std::string& query );

	public :
		CgiExecutor();
		~CgiExecutor();
		CgiExecutor(RequestContext& req_context);
		void	setContext(RequestContext&	req_context);

		std::string	getServerPort(std::string	host);

		bool	run(std::vector<char>& result, int&	cgi_status );


};



#endif