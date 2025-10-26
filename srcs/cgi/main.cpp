#include "../../includes/RequestParser.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"
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


	int  buffer = 4 * 1024;
	CgiExecutor cgi_executor;
	cgi_executor.setContext(req_context);
	int fd = cgi_executor.run();
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	sleep(1);
	CgiResult result;
	result = cgi_executor.getResult(buffer);

	// print body 
	std::cout << "FD == " << fd << std::endl;
	std::cout << "===================== status ====================" << std::endl;
	std::cout << "status" << result.status << std::endl;
	std::cout << "===================== body ====================" << std::endl;
	std::cout.write(&result.body[0], result.body.size());
	std::cout << "---------------------------------------------------------------" << std::endl;;
	std::cout << "===================== headers ====================" << std::endl;
	std::cout << Utils::mapToString(result.headers) << std::endl;
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
