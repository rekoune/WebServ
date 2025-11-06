#include "../../includes/RequestParser.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"
#include "../../includes/cgi/SessionHandler.hpp"
 # include <fcntl.h>
int main()
{
	RequestLine req_line;
	req_line.httpVersion = "HTTP/1.1";
	req_line.method = "POST"; 
	RequestContext req_context;
	req_context.req_line = req_line;
	req_context.script_path = "../../www/cgi-bin/test.py";

	/// #HEADERS
	req_context.headers.insert(std::make_pair("content-type", "text/html"));
	req_context.headers.insert(std::make_pair("content-length", "12"));
	req_context.headers.insert(std::make_pair("Accept", "*/*"));
	req_context.headers.insert(std::make_pair("Host", "10.10.10.1"));
	req_context.headers.insert(std::make_pair("Cookie", "SESSION_ID=abc123xyz; theme=dark; blabla=bloblo"));
	req_context.headers.insert(std::make_pair("Set-Cookie", "nigga=dark"));


	req_context.req_line.query = "name=esmo&age=20";
	req_context.body.push_back('T');
	req_context.body.push_back('H');
	req_context.body.push_back('I');
	req_context.body.push_back('S');
	req_context.body.push_back(' ');
	req_context.body.push_back('I');
	req_context.body.push_back('S');
	req_context.body.push_back(' ');
	req_context.body.push_back('B');
	req_context.body.push_back('O');
	req_context.body.push_back('D');
	req_context.body.push_back('Y');

	// for (std::vector<char>::iterator i = req_context.body.begin(); i != req_context.body.end(); i++)
	// {
	// 	std::cout << *i << std::endl;
	// }


// **MAIN FOR SESSION FETCHING AND FILLING 
	// check if the client first time ;
	// 
	// if ( !client )
	// 		addSession { generateId(); }
	// 
	// 
	// 
	// 
	// 1) fist time -> generate id, and set-setcookie 
	// 2) (> (second)) time fetch the id (saveit some were)
	// in the 2 cases : parse the cookies into the SessionData with the id detected 
	// in the case of the cgi: should fetch the cookie in SessionData into the headers the scrip behave according to the cookies 
	// and when reading from the scrip should fetch or save the cookies generated or writed from the script into the DataSession 

	std::array<SessionHandler, 3> arrayofsesshandler = {SessionHandler(), SessionHandler(), SessionHandler()};
	arrayofsesshandler[0].addSession(req_context.headers);
	arrayofsesshandler[1].addSession(req_context.headers);
	arrayofsesshandler[2].addSession(req_context.headers);

// // **MAIN FOR CGI RESULT READING 
	// int  buffer = 4 * 1024;
	// CgiExecutor cgi_executor;
	// CgiResult result;

	// cgi_executor.setContext(req_context);
	// int fd = cgi_executor.run();
	// // std::cout << fd << std::endl;
	// if (fd == -1 )
	// {
	// 	switch(cgi_executor.getResult().status)
	// 	{
	// 		case FORBIDDEN :
	// 			std::cout << "FORBIDDEN\n";
	// 			break ;
	// 		case INTERNAL_SERVER_ERROR : 
	// 			std::cout << "INTERNAL_SERVER_ERROR\n";
	// 			break ;
	// 		default : 
	// 			std::cout << "UNKNOWN\n";
	// 	}
	// 	return 0;
	// }
	// std::vector<size_t> size_log;
	// std::vector<char> body;
	// while (!cgi_executor.isDone())
	// {

	// 	result = cgi_executor.readResult(buffer);
	// 	body.insert(body.end(), result.body.begin(), result.body.end());
	// 	std::cout << "<<<<<<<<<<<< body >>>>>>>>>>>>>>>>\n";
	// 	std::cout.write(result.body.data(), result.body.size()) ;
		
		
	// 	size_log.push_back(result.body.size());
	// 	std::cout << "result.body.size()" << result.body.size() << std::endl;

	// }
	// std::cout << "~~~~~~~~~~~~~~~~~~~ RESULT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n";
	// int flags = fcntl(fd, F_GETFL, 0);
	// fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	// // print body 
	// std::cout << "FD == " << fd << std::endl;
	// std::cout << "===================== status ====================" << std::endl;
	// std::cout << "status" << result.status << std::endl;
	// std::cout << "===================== body ====================" << std::endl;
	// std::cout.write(body.data(), body.size());
	// std::cout << "---------------------------------------------------------------" << std::endl;;
	// std::cout << "===================== headers ====================" << std::endl;
	// std::cout << Utils::mapToString(result.headers) << std::endl;
	// std::cout  <<  "REQUEST_METHOD" << ": " << req.getRequestLine().target << " " << req.getRequestLine().method <<" " << req.getRequestLine().httpVersion << std::endl;
	// // std::cout << "QUERY_STRING";
	// // std::cout << "SCRIPT_NAME";
	// // std::cout << "SERVER_PROTOCOL";
}

// REQUEST_METHOD → GET
// QUERY_STRING → user=test
// SCRIPT_NAME → /hello.html
// SERVER_PROTOCOL → HTTP/1.1
// HTTP_USER_AGENT, HTTP_ACCEPT from headers
// HOST header for server name
