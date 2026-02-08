# WebServ

A custom HTTP/1.1 web server implementation written in C++98, designed to handle HTTP requests, CGI execution, file uploads, and directory listing.

## Overview

WebServ is a lightweight, efficient HTTP server that supports multiple server configurations, location-based routing, CGI script execution, and file management operations. Built from scratch following HTTP/1.1 specifications.

## Features

- **HTTP/1.1 Protocol Support**
  - GET, POST, DELETE methods
  - Keep-alive connections
  - Chunked transfer encoding
  - Content-Length based transfers

- **Multi-Server Configuration**
  - Multiple server blocks with different ports
  - Virtual host support
  - Custom error pages
  - Configurable client body size limits

- **CGI Support**
  - Execute scripts (Python, PHP, Shell)
  - Environment variable setup
  - Session handling
  - Configurable timeout (10 seconds)

- **File Operations**
  - File uploads with multipart/form-data
  - File deletion
  - Directory listing with autoindex
  - Static file serving

- **Advanced Features**
  - Non-blocking I/O with kqueue/epoll
  - Request parsing and validation
  - Resource resolution
  - MIME type detection
  - URL decoding

## 🏗️ Architecture

```
WebServ/
├── main.cpp                    
├── Makefile                    
├── conf.conf                   
├── config/                     
├── includes/                   
│   ├── client.hpp             
│   ├── server.hpp             
│   ├── RequestParser.hpp      
│   ├── Response.hpp           
│   ├── HttpHandler.hpp        
│   ├── ResourceResolver.hpp   
│   ├── UploadHandler.hpp      
│   ├── GetHandler.hpp         
│   ├── CgiExecutor.hpp        
│   └── Enums.hpp
│   ├── Structs.hpp
│   ├── Utils.hpp
└── srcs/                       
    ├── http/                  
    │   ├── request/           
    │   └── response/          
    ├── server/
    ├── config/                
    ├── cgi/                   
    └── utils/
```

## Getting Started

### Prerequisites

- C++ compiler with C++98 support
- Make

### Compilation

```bash
make
```

This will compile the project and create the `webserv` executable.

### Running the Server

```bash
# Using default configuration
./webserv

# Using custom configuration file
./webserv path/to/config.conf
```

## Configuration

The server uses a configuration file similar to NGINX syntax. Here's an example:

```nginx
server {
    server_name localhost;
    listen 0.0.0.0:8080;
    root /path/to/www;
    error_page 404 /errors/404.html;
    error_page 405 /errors/405.html;
    client_max_body_size 5g;

    location / {
        autoindex on;
        index index.html;
        root /path/to/www;
        allowed_methods GET;
    }

    location /upload {
        root /path/to/www/upload;
        allowed_methods POST GET;
        autoindex on;
        upload_store ../content;
        index index.html;
    }

    location /cgi-bin {
        root /path/to/www/cgi-bin;
        cgi_extension .py;
        cgi_extension .sh;
        allowed_methods POST GET;
        autoindex on;
    }

    location /content {
        root /path/to/www/content;
        allowed_methods GET DELETE;
        autoindex on;
    }
}
```

### Configuration Directives

#### Server Block
- `server_name` - Server hostname
- `listen` - IP address and port (e.g., 0.0.0.0:8080)
- `root` - Document root directory
- `error_page` - Custom error page (e.g., `404 /errors/404.html`)
- `client_max_body_size` - Maximum request body size (e.g., 5g, 10m)

#### Location Block
- `root` - Directory for this location
- `index` - Default index file
- `autoindex` - Enable/disable directory listing (`on`/`off`)
- `allowed_methods` - Allowed HTTP methods (GET, POST, DELETE)
- `upload_store` - Directory to store uploaded files
- `cgi_extension` - File extensions to execute as CGI scripts

## Development

### Build Commands

```bash
# Compile the project
make

# Clean object files
make clean

# Clean all generated files
make fclean

# Rebuild
make re
```

### Project Structure

The codebase is organized into logical modules:

- **HTTP Module**: Request parsing, response generation, and HTTP method handlers
- **Server Module**: Core server loop, client management, and event handling
- **CGI Module**: CGI script execution, session handling, and environment setup
- **Utils**: Helper functions for string manipulation, file operations

## Limitations

- HTTP/1.1 only (no HTTP/2 support)
- No HTTPS/TLS support
- Single-threaded event loop
- Limited to C++98 standard
