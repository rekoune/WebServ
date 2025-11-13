#!/usr/bin/env python3
import os
import datetime

def main():
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    
    # Get session ID from your server's environment
    session_id = os.environ.get('HTTP_SESSION_ID', 'Not set')
    
    # If no session ID in environment, check cookies
    # if session_id == 'Not set' and 'SESSION_ID=' in cookie_header:
    #     session_id = cookie_header.split('SESSION_ID=')[1].split(';')[0]
    
    # Simple visit counter
    visits = 1
    if 'visits=' in cookie_header:
        for cookie in cookie_header.split(';'):
            if 'visits=' in cookie:
                visits = int(cookie.split('=')[1]) + 1
                break
    
    print("Content-Type: text/html")
    
    # SET SESSION COOKIE HERE
    if session_id != 'Not set':
        print(f"Set-Cookie: SESSION_ID={session_id}; Path=/; HttpOnly")
    
    print(f"Set-Cookie: visits={visits}; Path=/")
    print()  # Blank line after headers
    
    print(f"""<html><body>
    <h2>Session Demo</h2>
    <p><strong>Session ID:</strong> {session_id}</p>
    <p><strong>Visits:</strong> {visits}</p>
    <p><strong>Time:</strong> {datetime.datetime.now().strftime('%H:%M:%S')}</p>
    <button onclick="location.reload()">Refresh</button>
    <p><em>Refresh the page - visits should increase, session ID should stay the same!</em></p>
    </body></html>""")

if __name__ == '__main__':
    main()