#ifndef SESSIONHANDLER_HPP
#define SESSIONHANDLER_HPP


#include "../Utils.hpp"
#include <sstream>
#include <iostream>
#include <string>



class   SessionHandler
{
	private :
		std::map<std::string, std::map< std::string, std::string> >     data;

		std::string		 		generateID();
	public  :

		SessionHandler();
		SessionHandler(const SessionHandler& other);
		~SessionHandler();
		SessionHandler& operator=(const SessionHandler& other);


		std::map<std::string, std::map< std::string, std::string> >&   getData();
		void	addSession(std::map<std::string, std::string>& headers);

		void    fetchDataToHeaders(std::map<std::string, std::string>& headers);		
		void	fillDataFromHeaders(std::string ID, std::map<std::string, std::string>& headers);
		void	appendDataCookies(std::string ID, std::map<std::string, std::string>& headers_cookies);


		std::string current_session_id;
		

		void    printSessionData();
		
		bool					is_new_client;

};

std::map<std::string, std::string>  splitCookieIntoMap(std::string cookie);
void        						parseCookieDirective(std::map<std::string, std::string>& map, std::string cookie_directive);
void 								printMapStr(std::map<std::string, std::string> map);
std::string 						cleanLineUtil(std::string line);
bool 								isitspaceUtil(char c);



#endif