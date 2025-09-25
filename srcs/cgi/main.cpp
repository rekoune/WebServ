#include "../../includes/Request.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"


int	main()
{
	std::string raw_req = 
	"GET /test.py?name=esmo&age=20 HTTP/1.1\r\n"
	"Host: localhost:8080\r\n"
	"User-Agent: curl/7.68.0\r\n"
	"Accept: */*\r\n"
	"\r\n";

	Request req;
	req.appendData(raw_req.c_str(), raw_req.size());




	int status;
	std::vector<char> result ;


	CgiExecutor cgi_executor(req, "www/cgi-bin/test.py", "webserv/1.1");
	cgi_executor.run(result, status);

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
