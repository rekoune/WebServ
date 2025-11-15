#!/usr/bin/env python3
import os
import cgi
from session_utils import get_session_manager

def main():
    session = get_session_manager()
    form = cgi.FieldStorage()
    
    # Handle profile updates
    if form.getvalue('update_profile'):
        session.set('user_name', form.getvalue('user_name', ''))
        session.set('user_email', form.getvalue('user_email', ''))
        session.set('theme', form.getvalue('theme', 'light'))
        session.set('notifications', 'on' if form.getvalue('notifications') else 'off')
    
    # Get current profile
    profile = {
        'name': session.get('user_name', ''),
        'email': session.get('user_email', ''),
        'theme': session.get('theme', 'light'),
        'notifications': session.get('notifications', 'off')
    }
    
    visit_count = session.increment_visit_count()
    
    print("Content-Type: text/html")
    print(f"Set-Cookie: SESSION_ID={session.session_id}; Path=/; HttpOnly")
    print()
    
    print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Profile</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
            background: {'#f5f5f5' if profile['theme'] == 'light' else '#333'};
            color: {'#333' if profile['theme'] == 'light' else '#fff'};
        }}
        .profile-card {{
            background: {'white' if profile['theme'] == 'light' else '#444'};
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        .session-stats {{
            background: {'#e9ecef' if profile['theme'] == 'light' else '#555'};
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
        }}
    </style>
</head>
<body>
    <div class="session-stats">
        <strong>Session Stats:</strong> 
        Visit #{visit_count} | 
        Session: {session.session_id[:12]}...
    </div>
    
    <div class="profile-card">
        <h1>User Profile</h1>
        
        <form method="POST">
            <input type="hidden" name="update_profile" value="1">
            
            <div style="margin-bottom: 15px;">
                <label>Name:</label><br>
                <input type="text" name="user_name" value="{profile['name']}" 
                       style="width: 100%; padding: 8px;">
            </div>
            
            <div style="margin-bottom: 15px;">
                <label>Email:</label><br>
                <input type="email" name="user_email" value="{profile['email']}" 
                       style="width: 100%; padding: 8px;">
            </div>
            
            <div style="margin-bottom: 15px;">
                <label>Theme:</label><br>
                <select name="theme" style="padding: 8px;">
                    <option value="light" {'selected' if profile['theme'] == 'light' else ''}>Light</option>
                    <option value="dark" {'selected' if profile['theme'] == 'dark' else ''}>Dark</option>
                </select>
            </div>
            
            <div style="margin-bottom: 15px;">
                <label>
                    <input type="checkbox" name="notifications" 
                           {'checked' if profile['notifications'] == 'on' else ''}>
                    Enable notifications
                </label>
            </div>
            
            <button type="submit" style="padding: 10px 20px;">Save Profile</button>
        </form>
    </div>
    
    <div style="margin-top: 20px; text-align: center;">
        <a href="/cgi-bin/task_manager_session.py">Task Manager</a> | 
        <a href="/cgi-bin/session_demo.py">Session Demo</a> | 
        <a href="/cgi-bin/user_profile.py">Refresh</a>
    </div>
</body>
</html>
""")

if __name__ == '__main__':
    main()