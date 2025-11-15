#!/usr/bin/env python3
import os

print("Content-Type: text/html")
print()

cookie_header = os.environ.get('HTTP_COOKIE', '')
session_id = None

# Extract SESSION_ID from cookies
if 'SESSION_ID=' in cookie_header:
    for cookie in cookie_header.split(';'):
        cookie = cookie.strip()
        if cookie.startswith('SESSION_ID='):
            session_id = cookie.split('=', 1)[1]
            break

print(f"""
<!DOCTYPE html>
<html>
<head><title>Session Reader</title></head>
<body>
    <h1>Session Debug Info</h1>
    <h2>Cookies Received:</h2>
    <pre>{cookie_header or 'No cookies received'}</pre>
    
    <h2>Session ID:</h2>
    <pre>{session_id or 'No SESSION_ID found'}</pre>
    
    <h2>All Environment Variables:</h2>
    <ul>
""")

for key in sorted(os.environ.keys()):
    if key.startswith('HTTP_') or key in ['REQUEST_METHOD', 'QUERY_STRING']:
        print(f"<li><strong>{key}:</strong> {os.environ[key]}</li>")

print("""
    </ul>
    
    <h2>Test Links:</h2>
    <ul>
        <li><a href="/cgi-bin/session_demo.py">Session Demo</a></li>
        <li><a href="/cgi-bin/test.py">CGI Test</a></li>
        <li><a href="/">Home</a></li>
    </ul>
</body>
</html>
""")