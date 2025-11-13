#!/usr/bin/env python3
import os
import sys

def get_current_theme(cookie_header):
    """Extract theme from cookie"""
    if not cookie_header:
        return "light"
    
    cookies = {}
    for cookie in cookie_header.split(';'):
        cookie = cookie.strip()
        if '=' in cookie:
            key, value = cookie.split('=', 1)
            cookies[key] = value
    
    return cookies.get('theme', 'light')

def main():
    # Get cookies from environment
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    
    # Get query parameters
    query_string = os.environ.get('QUERY_STRING', '')
    params = {}
    if query_string:
        for param in query_string.split('&'):
            if '=' in param:
                key, value = param.split('=', 1)
                params[key] = value
    
    current_theme = get_current_theme(cookie_header)
    new_theme = current_theme
    set_cookie = False
    
    # Handle theme change
    if 'theme' in params:
        if params['theme'] in ['light', 'dark']:
            new_theme = params['theme']
            set_cookie = True
    elif 'toggle' in params:
        new_theme = 'dark' if current_theme == 'light' else 'light'
        set_cookie = True
    
    # Output headers
    if set_cookie:
        print(f"Set-Cookie: theme={new_theme}; Path=/; Max-Age=3600")
    
    print("Content-Type: text/html")
    print()
    
    # HTML content with theme-based styling
    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Theme Switcher</title>
    <style>
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            margin: 0;
            padding: 2rem;
            transition: all 0.3s ease;
            min-height: 100vh;
        }}
        
        .light-mode {{
            background-color: #ffffff;
            color: #333333;
        }}
        
        .dark-mode {{
            background-color: #1a1a1a;
            color: #ffffff;
        }}
        
        .container {{
            max-width: 600px;
            margin: 0 auto;
            padding: 2rem;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        }}
        
        .light-mode .container {{
            background: #f8f9fa;
            border: 1px solid #e9ecef;
        }}
        
        .dark-mode .container {{
            background: #2d2d2d;
            border: 1px solid #404040;
        }}
        
        .theme-buttons {{
            display: flex;
            gap: 1rem;
            margin: 2rem 0;
        }}
        
        button {{
            padding: 0.75rem 1.5rem;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 1rem;
            transition: all 0.2s ease;
        }}
        
        .light-mode button {{
            background: #007bff;
            color: white;
        }}
        
        .light-mode button:hover {{
            background: #0056b3;
        }}
        
        .dark-mode button {{
            background: #0d6efd;
            color: white;
        }}
        
        .dark-mode button:hover {{
            background: #0b5ed7;
        }}
        
        .info-box {{
            padding: 1rem;
            border-radius: 4px;
            margin: 1.5rem 0;
            font-family: 'Courier New', monospace;
            font-size: 0.9rem;
        }}
        
        .light-mode .info-box {{
            background: #e9ecef;
            border: 1px solid #dee2e6;
        }}
        
        .dark-mode .info-box {{
            background: #343a40;
            border: 1px solid #495057;
        }}
        
        .current-theme {{
            display: inline-block;
            padding: 0.25rem 0.75rem;
            border-radius: 4px;
            font-weight: bold;
            margin-left: 0.5rem;
        }}
        
        .light-mode .current-theme {{
            background: #28a745;
            color: white;
        }}
        
        .dark-mode .current-theme {{
            background: #20c997;
            color: #1a1a1a;
        }}
    </style>
</head>
<body class="{new_theme}-mode">
    <div class="container">
        <h1>🎨 Theme Switcher Demo</h1>
        <p>This page demonstrates CGI cookie handling for theme persistence.</p>
        
        <div class="info-box">
            <strong>Current Theme:</strong> 
            <span class="current-theme">{new_theme.upper()}</span>
        </div>
        
        <div class="theme-buttons">
            <button onclick="setTheme('light')">🌞 Light Mode</button>
            <button onclick="setTheme('dark')">🌙 Dark Mode</button>
            <button onclick="toggleTheme()">🔄 Toggle Theme</button>
        </div>
        
        <div class="info-box">
            <h3>Debug Information:</h3>
            <p><strong>Cookie Header:</strong> {cookie_header or 'None'}</p>
            <p><strong>Query String:</strong> {query_string or 'None'}</p>
            <p><strong>Session ID:</strong> {os.environ.get('HTTP_SESSION_ID', 'Not set')}</p>
        </div>
        
        <p><small>Try refreshing the page - your theme preference will be remembered!</small></p>
    </div>
    
    <script>
        function setTheme(theme) {{
            window.location.href = `?theme=${{theme}}`;
        }}
        
        function toggleTheme() {{
            window.location.href = '?toggle=1';
        }}
    </script>
</body>
</html>"""
    
    print(html)

if __name__ == '__main__':
    main()