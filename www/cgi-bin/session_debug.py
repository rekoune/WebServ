#!/usr/bin/env python3
import os
import json

def get_session_info():
    """Get detailed session information"""
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    session_id = None
    cookies = {}
    
    if cookie_header:
        for cookie in cookie_header.split(';'):
            cookie = cookie.strip()
            if '=' in cookie:
                key, value = cookie.split('=', 1)
                cookies[key] = value
                if key == 'SESSION_ID':
                    session_id = value
    
    # Check if task file exists
    task_file_exists = False
    task_count = 0
    if session_id:
        task_file = f"/tmp/tasks_{session_id}.json"
        task_file_exists = os.path.exists(task_file)
        if task_file_exists:
            try:
                with open(task_file, 'r') as f:
                    tasks = json.load(f)
                    task_count = len(tasks)
            except:
                task_count = 0
    
    return {
        'session_id': session_id,
        'cookies_received': cookies,
        'task_file_exists': task_file_exists,
        'task_count': task_count,
        'task_file_path': f"/tmp/tasks_{session_id}.json" if session_id else None
    }

def main():
    session_info = get_session_info()
    
    print("Content-Type: text/html")
    print()
    
    print(f"""
<!DOCTYPE html>
<html>
<head>
    <title>Session Debug</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 2rem; }}
        .info {{ background: #f0f0f0; padding: 1rem; margin: 1rem 0; border-radius: 5px; }}
        .success {{ color: green; }}
        .error {{ color: red; }}
        pre {{ background: #333; color: white; padding: 1rem; border-radius: 5px; }}
    </style>
</head>
<body>
    <h1>Session Debug Information</h1>
    
    <div class="info">
        <h2>Session Status</h2>
        {"<p class='success'>✓ Session ID found!</p>" if session_info['session_id'] 
         else "<p class='error'>✗ No SESSION_ID found</p>"}
        <p><strong>Session ID:</strong> {session_info['session_id'] or 'None'}</p>
        <p><strong>Task File:</strong> {session_info['task_file_path'] or 'N/A'}</p>
        <p><strong>Task File Exists:</strong> {'Yes' if session_info['task_file_exists'] else 'No'}</p>
        <p><strong>Tasks in Session:</strong> {session_info['task_count']}</p>
    </div>
    
    <div class="info">
        <h2>All Cookies Received</h2>
        <pre>{json.dumps(session_info['cookies_received'], indent=2)}</pre>
    </div>
    
    <div class="info">
        <h2>Environment Variables</h2>
        <ul>
    """)
    
    for key in sorted(os.environ.keys()):
        if key.startswith('HTTP_') or key in ['REQUEST_METHOD', 'QUERY_STRING']:
            print(f"<li><strong>{key}:</strong> {os.environ[key]}</li>")
    
    print("""
        </ul>
    </div>
    
    <div style="margin-top: 2rem;">
        <a href="/cgi-bin/task_manager.py" class="btn">✅ Task Manager</a>
        <a href="/cgi-bin/task_api.py" class="btn">🔧 Task API</a>
        <button onclick="location.reload()">🔄 Refresh</button>
    </div>
</body>
</html>
""")

if __name__ == '__main__':
    main()