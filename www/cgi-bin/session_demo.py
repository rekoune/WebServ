#!/usr/bin/env python3
import os
import datetime

def main():
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    
    # Extract session ID from cookies (set by your C++ server)
    session_id = 'Not set'
    if 'SESSION_ID=' in cookie_header:
        session_id = cookie_header.split('SESSION_ID=')[1].split(';')[0]
    
    # Simple visit counter
    visits = 1
    if 'visits=' in cookie_header:
        for cookie in cookie_header.split(';'):
            cookie = cookie.strip()
            if cookie.startswith('visits='):
                visits = int(cookie.split('=')[1]) + 1
                break
    
    # Get other CGI environment info for debugging
    request_method = os.environ.get('REQUEST_METHOD', 'Unknown')
    query_string = os.environ.get('QUERY_STRING', '')
    server_name = os.environ.get('SERVER_NAME', 'Unknown')
    server_port = os.environ.get('SERVER_PORT', 'Unknown')
    
    print("Content-Type: text/html")
    
    # Set visit counter cookie (this is separate from session cookie)
    print(f"Set-Cookie: visits={visits}; Path=/; Max-Age=3600")
    print()  # Blank line after headers
    
    print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Session Demo - WebServer</title>
    <style>
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            margin: 0;
            padding: 2rem;
            background-color: #f5f5f5;
            color: #333;
        }}
        .container {{
            max-width: 800px;
            margin: 0 auto;
            background: white;
            padding: 2rem;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        }}
        h1 {{
            color: #2c3e50;
            border-bottom: 2px solid #3498db;
            padding-bottom: 0.5rem;
        }}
        .info-card {{
            background: #f8f9fa;
            padding: 1rem;
            border-radius: 4px;
            margin: 1rem 0;
            border-left: 4px solid #3498db;
        }}
        .session-id {{
            font-family: 'Courier New', monospace;
            background: #2c3e50;
            color: white;
            padding: 0.5rem;
            border-radius: 4px;
            word-break: break-all;
        }}
        .stat {{
            display: flex;
            justify-content: space-between;
            margin: 0.5rem 0;
        }}
        .stat-value {{
            font-weight: bold;
            color: #2c3e50;
        }}
        button {{
            background: #3498db;
            color: white;
            border: none;
            padding: 0.75rem 1.5rem;
            border-radius: 4px;
            cursor: pointer;
            font-size: 1rem;
            margin: 0.5rem;
        }}
        button:hover {{
            background: #2980b9;
        }}
        .debug {{
            background: #fff3cd;
            border: 1px solid #ffeaa7;
            padding: 1rem;
            border-radius: 4px;
            margin-top: 1rem;
            font-size: 0.9rem;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🔐 Session Management Demo</h1>
        
        <div class="info-card">
            <h2>Session Information</h2>
            <div class="stat">
                <span>Session ID:</span>
                <span class="session-id">{session_id}</span>
            </div>
            <div class="stat">
                <span>Visit Count:</span>
                <span class="stat-value">{visits}</span>
            </div>
            <div class="stat">
                <span>Current Time:</span>
                <span class="stat-value">{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</span>
            </div>
        </div>

        <div class="info-card">
            <h2>Server Information</h2>
            <div class="stat">
                <span>Request Method:</span>
                <span class="stat-value">{request_method}</span>
            </div>
            <div class="stat">
                <span>Server:</span>
                <span class="stat-value">{server_name}:{server_port}</span>
            </div>
            <div class="stat">
                <span>Query String:</span>
                <span class="stat-value">{query_string or 'None'}</span>
            </div>
        </div>

        <div>
            <button onclick="location.reload()">🔄 Refresh Page</button>
            <button onclick="location.href='?test=123'">🧪 Test with Query</button>
        </div>

        <div class="debug">
            <h3>Debug Information</h3>
            <p><strong>Cookie Header:</strong> {cookie_header or 'No cookies'}</p>
            <p><strong>Expected Behavior:</strong> Session ID should remain constant across refreshes, visit count should increment.</p>
        </div>

        <p><em>💡 This demo shows  WebServer's session management in action. The session ID is managed by your C++ server and should persist across requests.</em></p>
    </div>

    <script>
        function clearVisits() {{
            document.cookie = "visits=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;";
            location.reload();
        }}
    </script>
</body>
</html>""")

if __name__ == '__main__':
    main()