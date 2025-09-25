#ifndef CGI_HPP 
#define CGI_HPP

class CgiExecutor
{
	private :
		Request						request		;
		std::string 				script_path	;
		std::string					server_software;



		std::vector<std::string>	buildEnv();
		void						parseTarget(std::string& script_name, std::string& query );

	public :
		CgiExecutor();
		~CgiExecutor();
		CgiExecutor(Request& req, std::string script_path, std::string	server_software);

	
		bool	run(std::vector<char>& result, int&	cgi_status );


};



#endif