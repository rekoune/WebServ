#!/usr/bin/env python3

import datetime
import random
import socket
import os

print("Content-Type: text/html")
print()

html = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python Dynamic Page</title>
    <style>
        body {{ 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }}
        .container {{
            max-width: 800px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }}
        h1 {{
            color: #333;
            text-align: center;
            margin-bottom: 30px;
            border-bottom: 3px solid #667eea;
            padding-bottom: 10px;
        }}
        .card {{
            background: #f8f9fa;
            padding: 20px;
            margin: 15px 0;
            border-radius: 10px;
            border-left: 5px solid #667eea;
        }}
        .stats {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }}
        .stat-item {{
            background: #e3f2fd;
            padding: 15px;
            border-radius: 8px;
            text-align: center;
        }}
        .random-color {{
            padding: 10px;
            border-radius: 5px;
            color: white;
            text-align: center;
            margin: 10px 0;
            font-weight: bold;
        }}
        .footer {{
            text-align: center;
            margin-top: 30px;
            color: #666;
            font-style: italic;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🐍 Python Dynamic Page Generator</h1>
        
        <div class="card">
            <h2>📊 Server Information</h2>
            <div class="stats">
                <div class="stat-item">
                    <strong>Current Time</strong><br>
                    {datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
                </div>
                <div class="stat-item">
                    <strong>Hostname</strong><br>
                    {socket.gethostname()}
                </div>
                <div class="stat-item">
                    <strong>Python Version</strong><br>
                    {os.sys.version.split()[0]}
                </div>
                <div class="stat-item">
                    <strong>Working Directory</strong><br>
                    {os.getcwd()}
                </div>
            </div>
        </div>

        <div class="card">
            <h2>🎲 Dynamic Content</h2>
            <p><strong>Random Number:</strong> {random.randint(1, 1000)}</p>
            <p><strong>Random Choice:</strong> {random.choice(['Python', 'Bash', 'JavaScript', 'C++', 'Java'])}</p>
            <div class="random-color" style="background: #{random.randint(0, 0xFFFFFF):06x}">
                Random Color: #{random.randint(0, 0xFFFFFF):06x}
            </div>
        </div>

        <div class="card">
            <h2>📈 Environment Stats</h2>
            <p><strong>User ID:</strong> {os.getuid()}</p>
            <p><strong>Process ID:</strong> {os.getpid()}</p>
            <p><strong>Platform:</strong> {os.sys.platform}</p>
            <p><strong>Script Path:</strong> {os.path.abspath(__file__)}</p>
        </div>

        <div class="footer">
            Generated with ❤️ using Python on {datetime.datetime.now().strftime("%A, %B %d, %Y")}
        </div>
    </div>
</body>
</html>
"""

print(html)