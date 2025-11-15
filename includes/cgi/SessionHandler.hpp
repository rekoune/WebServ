#ifndef SESSIONHANDLER_HPP
#define SESSIONHANDLER_HPP


#include "../Utils.hpp"
// #include "../Headers.hpp"
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
		// NEW CLIENT, WE ADD SESSION
		void	addSession(std::map<std::string, std::string>& headers);

		// EMPTY COOKIES FROM SESSION_DATA INTO HEADERS
		void    fetchDataToHeaders(std::map<std::string, std::string>& headers);
		// void	appendHeaders(std::map<std::string, std::string> headers); // UTILS OF FETCH
		
		// PUT THE COOKIES INTO SESSION_DATA
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