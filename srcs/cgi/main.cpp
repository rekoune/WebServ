#include "../../includes/RequestParser.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/cgi/CgiExecutor.hpp"

int main()
{
	RequestLine req_line;
	req_line.httpVersion = "HTTP/1.1";
	req_line.method = "GET"; 
	RequestContext req_context;
	req_context.req_line = req_line;
	req_context.script_path = "www/cgi-bin/test.py";

	/// #HEADERS
	req_context.headers.insert(std::make_pair("content-type", "text/html"));
	req_context.headers.insert(std::make_pair("content-length", "68137"));
	req_context.headers.insert(std::make_pair("Accept", "*/*"));
	req_context.headers.insert(std::make_pair("Host", "10.10.10.1"));

	req_context.query = "name=esmo&age=20";
	req_context.script_name = "test.py";


	int status;
	std::vector<char> result;

	CgiExecutor cgi_executor(req_context);
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
