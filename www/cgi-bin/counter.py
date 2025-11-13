#!/usr/bin/env python3
import os
import datetime

def main():
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    
    # Extract visit count from cookies
    visit_count = 1
    last_visit = 'First visit!'
    
    if 'visits' in cookie_header:
        for cookie in cookie_header.split(';'):
            cookie = cookie.strip()
            if cookie.startswith('visits='):
                visit_count = int(cookie.split('=')[1]) + 1
            elif cookie.startswith('last_visit='):
                last_visit = cookie.split('=')[1].replace('%20', ' ')
    
    current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    # Output headers with updated cookies
    print("Content-Type: text/html")
    print(f"Set-Cookie: visits={visit_count}; Path=/; Max-Age=3600")
    print(f"Set-Cookie: last_visit={current_time.replace(' ', '%20')}; Path=/; Max-Age=3600")
    print()

    print(f"""
<!DOCTYPE html>
<html>
<head>
    <title>Visitor Counter</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 2rem; }}
        .counter {{ font-size: 3rem; color: #007bff; text-align: center; }}
        .info {{ text-align: center; color: #666; }}
    </style>
</head>
<body>
    <div class="counter">👋 {visit_count}</div>
    <div class="info">
        <p>Number of visits</p>
        <p>Last visit: {last_visit}</p>
        <p>Current time: {current_time}</p>
    </div>
</body>
</html>
""")

if __name__ == '__main__':
    main()