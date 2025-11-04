#ifndef SESSIONHANDLER_HPP
#define SESSIONHANDLER_HPP


#include "../Utils.hpp"
#include "../Headers.hpp"


class   SessionHandler
{
	private :
		std::map<std::string, std::map< std::string, std::string> >     data;

	public  :

		SessionHandler();
		SessionHandler(const SessionHandler& other);
		~SessionHandler();
		SessionHandler& operator=(const SessionHandler& other);

		// NEW CLIENT ?
		// 			  |
		//		 	  V
		void	addSession();

		// EMPTY COOKIES FROM SESSION_DATA INTO HEADERS
		void	fetchData();
		void	appendHeaders(std::map<std::string, std::string> headers); // UTILS OF FETCH
		
		// PUT THE COOKIES INTO SESSION_DATA
		void	fillData(std::map<std::string, std::string> headers);


};



#endif