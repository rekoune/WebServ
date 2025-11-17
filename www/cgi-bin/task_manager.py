#!/usr/bin/env python3
import os
import json
import cgi
import cgitb
from datetime import datetime, timedelta
import sys

# Enable error reporting
cgitb.enable()

# Cookie configuration
COOKIE_CONFIG = {
    'theme': {'max_age': 365 * 24 * 60 * 60, 'path': '/'},
    'tasks_per_page': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'sort_preference': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'default_category': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'view_mode': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'sidebar_collapsed': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'completed_visible': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'auto_save': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'notifications': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'keyboard_shortcuts': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'language': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'visit_count': {'max_age': 365 * 24 * 60 * 60, 'path': '/'},
    'first_visit': {'max_age': 365 * 24 * 60 * 60, 'path': '/'},
    'last_visit': {'max_age': 365 * 24 * 60 * 60, 'path': '/'},
    'screen_resolution': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'timezone': {'max_age': 30 * 24 * 60 * 60, 'path': '/'}
}

def get_cookie(name, default=None):
    """Get cookie value by name"""
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    if not cookie_header:
        return default
    
    cookies = {}
    for cookie in cookie_header.split(';'):
        cookie = cookie.strip()
        if '=' in cookie:
            key, value = cookie.split('=', 1)
            cookies[key] = value
    
    return cookies.get(name, default)

def set_cookie(name, value, max_age=None, path='/', http_only=False):
    """Set a cookie with proper formatting"""
    cookie_parts = [f"{name}={value}"]
    
    if max_age:
        cookie_parts.append(f"Max-Age={max_age}")
    
    if path:
        cookie_parts.append(f"Path={path}")
    
    if http_only:
        cookie_parts.append("HttpOnly")
    
    # Always set Secure flag for HTTPS (commented out for local development)
    # cookie_parts.append("Secure")
    
    cookie_parts.append("SameSite=Lax")
    
    print("Set-Cookie: " + "; ".join(cookie_parts))

def get_session_id():
    """Get session ID from cookie - set by C++ server"""
    return get_cookie('SESSION_ID')

def initialize_user_cookies():
    """Initialize default values for user preference cookies"""
    cookies_to_set = []
    
    # Theme
    if not get_cookie('theme'):
        set_cookie('theme', 'light', COOKIE_CONFIG['theme']['max_age'])
        cookies_to_set.append('theme')
    
    # Tasks per page
    if not get_cookie('tasks_per_page'):
        set_cookie('tasks_per_page', '10', COOKIE_CONFIG['tasks_per_page']['max_age'])
        cookies_to_set.append('tasks_per_page')
    
    # Sort preference
    if not get_cookie('sort_preference'):
        set_cookie('sort_preference', 'created_desc', COOKIE_CONFIG['sort_preference']['max_age'])
        cookies_to_set.append('sort_preference')
    
    # Default category
    if not get_cookie('default_category'):
        set_cookie('default_category', 'personal', COOKIE_CONFIG['default_category']['max_age'])
        cookies_to_set.append('default_category')
    
    # View mode
    if not get_cookie('view_mode'):
        set_cookie('view_mode', 'list', COOKIE_CONFIG['view_mode']['max_age'])
        cookies_to_set.append('view_mode')
    
    # UI State
    if not get_cookie('sidebar_collapsed'):
        set_cookie('sidebar_collapsed', 'false', COOKIE_CONFIG['sidebar_collapsed']['max_age'])
        cookies_to_set.append('sidebar_collapsed')
    
    if not get_cookie('completed_visible'):
        set_cookie('completed_visible', 'true', COOKIE_CONFIG['completed_visible']['max_age'])
        cookies_to_set.append('completed_visible')
    
    # Functional preferences
    if not get_cookie('auto_save'):
        set_cookie('auto_save', 'true', COOKIE_CONFIG['auto_save']['max_age'])
        cookies_to_set.append('auto_save')
    
    if not get_cookie('notifications'):
        set_cookie('notifications', 'true', COOKIE_CONFIG['notifications']['max_age'])
        cookies_to_set.append('notifications')
    
    if not get_cookie('keyboard_shortcuts'):
        set_cookie('keyboard_shortcuts', 'true', COOKIE_CONFIG['keyboard_shortcuts']['max_age'])
        cookies_to_set.append('keyboard_shortcuts')
    
    # Language
    if not get_cookie('language'):
        set_cookie('language', 'en', COOKIE_CONFIG['language']['max_age'])
        cookies_to_set.append('language')
    
    # Visit tracking
    visit_count = int(get_cookie('visit_count', '0')) + 1
    set_cookie('visit_count', str(visit_count), COOKIE_CONFIG['visit_count']['max_age'])
    
    if not get_cookie('first_visit'):
        first_visit = datetime.now().isoformat()
        set_cookie('first_visit', first_visit, COOKIE_CONFIG['first_visit']['max_age'])
        cookies_to_set.append('first_visit')
    
    last_visit = datetime.now().isoformat()
    set_cookie('last_visit', last_visit, COOKIE_CONFIG['last_visit']['max_age'])
    
    return cookies_to_set

def get_user_preferences():
    """Get all user preferences from cookies"""
    return {
        'theme': get_cookie('theme', 'light'),
        'tasks_per_page': int(get_cookie('tasks_per_page', '10')),
        'sort_preference': get_cookie('sort_preference', 'created_desc'),
        'default_category': get_cookie('default_category', 'personal'),
        'view_mode': get_cookie('view_mode', 'list'),
        'sidebar_collapsed': get_cookie('sidebar_collapsed', 'false') == 'true',
        'completed_visible': get_cookie('completed_visible', 'true') == 'true',
        'auto_save': get_cookie('auto_save', 'true') == 'true',
        'notifications': get_cookie('notifications', 'true') == 'true',
        'keyboard_shortcuts': get_cookie('keyboard_shortcuts', 'true') == 'true',
        'language': get_cookie('language', 'en'),
        'visit_count': int(get_cookie('visit_count', '1')),
        'first_visit': get_cookie('first_visit', datetime.now().isoformat()),
        'last_visit': get_cookie('last_visit', datetime.now().isoformat())
    }

def load_tasks(session_id):
    """Load tasks from session file"""
    if not session_id:
        return []
    
    task_file = f"/tmp/tasks_{session_id}.json"
    try:
        with open(task_file, 'r') as f:
            return json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        return []

def save_tasks(session_id, tasks):
    """Save tasks to session file"""
    if not session_id:
        return False
    
    task_file = f"/tmp/tasks_{session_id}.json"
    try:
        with open(task_file, 'w') as f:
            json.dump(tasks, f, indent=2)
        return True
    except Exception:
        return False

def generate_task_id(tasks):
    """Generate unique task ID"""
    if not tasks:
        return 1
    return max(task['id'] for task in tasks) + 1

def sort_tasks(tasks, sort_preference):
    """Sort tasks based on user preference"""
    if sort_preference == 'created_desc':
        return sorted(tasks, key=lambda x: x.get('created_at', ''), reverse=True)
    elif sort_preference == 'created_asc':
        return sorted(tasks, key=lambda x: x.get('created_at', ''))
    elif sort_preference == 'priority':
        priority_order = {'urgent': 0, 'work': 1, 'personal': 2, 'shopping': 3, 'health': 4}
        return sorted(tasks, key=lambda x: priority_order.get(x.get('category', 'personal'), 5))
    elif sort_preference == 'name':
        return sorted(tasks, key=lambda x: x.get('text', '').lower())
    elif sort_preference == 'completed':
        return sorted(tasks, key=lambda x: (x.get('completed', False), x.get('created_at', '')), reverse=True)
    else:
        return tasks

def get_category_emoji(category):
    """Get emoji for category"""
    emojis = {
        'personal': '👤',
        'work': '💼', 
        'shopping': '🛒',
        'health': '🏥',
        'urgent': '🚨'
    }
    return emojis.get(category, '📝')

def generate_task_html(tasks, prefs):
    """Generate HTML for task list based on user preferences"""
    if not tasks:
        return '''
                <div class="empty-state">
                    <h3>🎉 No tasks yet!</h3>
                    <p>Add your first task using the form above to get started.</p>
                </div>
                '''
    
    # Filter tasks based on visibility preference
    display_tasks = tasks if prefs['completed_visible'] else [t for t in tasks if not t['completed']]
    
    # Sort tasks
    display_tasks = sort_tasks(display_tasks, prefs['sort_preference'])
    
    # Paginate tasks
    tasks_per_page = prefs['tasks_per_page']
    total_pages = (len(display_tasks) + tasks_per_page - 1) // tasks_per_page
    
    task_html = []
    for task in display_tasks[:tasks_per_page]:  # Simple pagination for now
        completed_class = 'completed' if task['completed'] else ''
        checked = 'checked' if task['completed'] else ''
        category_emoji = get_category_emoji(task['category'])
        
        created_date = task.get("created_at", "").split("T")[0] if task.get("created_at") else ""
        
        task_html.append(f'''
                <div class="task-item {completed_class}">
                    <form method="POST" style="display: inline;">
                        <input type="hidden" name="action" value="toggle">
                        <input type="hidden" name="task_id" value="{task['id']}">
                        <input type="checkbox" class="task-checkbox" {checked} 
                               onchange="this.form.submit()">
                    </form>
                    <span class="task-text">{task['text']}</span>
                    <span class="task-category category-{task['category']}">
                        {category_emoji} {task['category'].title()}
                    </span>
                    <div class="task-meta">
                        <small>{created_date}</small>
                    </div>
                    <form method="POST" style="display: inline;">
                        <input type="hidden" name="action" value="delete">
                        <input type="hidden" name="task_id" value="{task['id']}">
                        <button type="submit" class="delete-btn" onclick="return confirm('Delete this task?')">🗑️</button>
                    </form>
                </div>
                ''')
    
    return ''.join(task_html)

def handle_preference_update(form):
    """Handle user preference updates from forms"""
    updated_prefs = {}
    
    if form.getvalue('update_theme'):
        theme = form.getvalue('theme', 'light')
        set_cookie('theme', theme, COOKIE_CONFIG['theme']['max_age'])
        updated_prefs['theme'] = theme
    
    if form.getvalue('update_tasks_per_page'):
        tasks_per_page = form.getvalue('tasks_per_page', '10')
        set_cookie('tasks_per_page', tasks_per_page, COOKIE_CONFIG['tasks_per_page']['max_age'])
        updated_prefs['tasks_per_page'] = tasks_per_page
    
    if form.getvalue('update_sort_preference'):
        sort_pref = form.getvalue('sort_preference', 'created_desc')
        set_cookie('sort_preference', sort_pref, COOKIE_CONFIG['sort_preference']['max_age'])
        updated_prefs['sort_preference'] = sort_pref
    
    if form.getvalue('update_default_category'):
        default_cat = form.getvalue('default_category', 'personal')
        set_cookie('default_category', default_cat, COOKIE_CONFIG['default_category']['max_age'])
        updated_prefs['default_category'] = default_cat
    
    if form.getvalue('update_view_mode'):
        view_mode = form.getvalue('view_mode', 'list')
        set_cookie('view_mode', view_mode, COOKIE_CONFIG['view_mode']['max_age'])
        updated_prefs['view_mode'] = view_mode
    
    if form.getvalue('toggle_sidebar'):
        current = get_cookie('sidebar_collapsed', 'false')
        new_value = 'true' if current == 'false' else 'false'
        set_cookie('sidebar_collapsed', new_value, COOKIE_CONFIG['sidebar_collapsed']['max_age'])
        updated_prefs['sidebar_collapsed'] = new_value
    
    if form.getvalue('toggle_completed_visible'):
        current = get_cookie('completed_visible', 'true')
        new_value = 'true' if current == 'false' else 'false'
        set_cookie('completed_visible', new_value, COOKIE_CONFIG['completed_visible']['max_age'])
        updated_prefs['completed_visible'] = new_value
    
    if form.getvalue('toggle_auto_save'):
        current = get_cookie('auto_save', 'true')
        new_value = 'true' if current == 'false' else 'false'
        set_cookie('auto_save', new_value, COOKIE_CONFIG['auto_save']['max_age'])
        updated_prefs['auto_save'] = new_value
    
    if form.getvalue('toggle_notifications'):
        current = get_cookie('notifications', 'true')
        new_value = 'true' if current == 'false' else 'false'
        set_cookie('notifications', new_value, COOKIE_CONFIG['notifications']['max_age'])
        updated_prefs['notifications'] = new_value
    
    if form.getvalue('toggle_keyboard_shortcuts'):
        current = get_cookie('keyboard_shortcuts', 'true')
        new_value = 'true' if current == 'false' else 'false'
        set_cookie('keyboard_shortcuts', new_value, COOKIE_CONFIG['keyboard_shortcuts']['max_age'])
        updated_prefs['keyboard_shortcuts'] = new_value
    
    if form.getvalue('update_language'):
        language = form.getvalue('language', 'en')
        set_cookie('language', language, COOKIE_CONFIG['language']['max_age'])
        updated_prefs['language'] = language
    
    return updated_prefs

def main():
    # Initialize user cookies and get session ID
    initialized_cookies = initialize_user_cookies()
    session_id = get_session_id()
    method = os.environ.get('REQUEST_METHOD', 'GET')
    
    # Load current tasks
    tasks = load_tasks(session_id)
    
    # Handle POST requests
    if method == 'POST':
        form = cgi.FieldStorage()
        action = form.getvalue('action', '')
        
        # Handle preference updates first
        updated_prefs = handle_preference_update(form)
        
        # Then handle task actions
        if action == 'add' and form.getvalue('task_text'):
            task_text = form.getvalue('task_text').strip()
            if task_text:  # Only add non-empty tasks
                new_task = {
                    'id': generate_task_id(tasks),
                    'text': task_text,
                    'completed': False,
                    'created_at': datetime.now().isoformat(),
                    'category': form.getvalue('category', get_cookie('default_category', 'personal'))
                }
                tasks.append(new_task)
                save_tasks(session_id, tasks)
                
        elif action == 'toggle':
            task_id = form.getvalue('task_id')
            if task_id:
                task_id = int(task_id)
                for task in tasks:
                    if task['id'] == task_id:
                        task['completed'] = not task['completed']
                        break
                save_tasks(session_id, tasks)
            
        elif action == 'delete':
            task_id = form.getvalue('task_id')
            if task_id:
                task_id = int(task_id)
                tasks = [t for t in tasks if t['id'] != task_id]
                save_tasks(session_id, tasks)
            
        elif action == 'clear_completed':
            tasks = [t for t in tasks if not t['completed']]
            save_tasks(session_id, tasks)
            
        elif action == 'clear_all':
            tasks = []
            save_tasks(session_id, tasks)
        
        # After POST, use JavaScript redirect instead of HTTP redirect
        # This avoids the browser warning
        print("Content-Type: text/html")
        print()
        print(f"""
        <html>
        <head>
            <script>
                window.location.href = '/cgi-bin/task_manager.py';
            </script>
        </head>
        <body>
            <p>Redirecting... <a href="/cgi-bin/task_manager.py">Click here if not redirected</a></p>
        </body>
        </html>
        """)
        return
    
    # Get user preferences for display
    prefs = get_user_preferences()
    
    # Count stats for GET requests
    total_tasks = len(tasks)
    completed_tasks = len([t for t in tasks if t['completed']])
    pending_tasks = total_tasks - completed_tasks
    
    # Generate task HTML based on preferences
    task_html = generate_task_html(tasks, prefs)
    
    # Output HTML for GET requests
    print("Content-Type: text/html")
    print()
    
    # Calculate some user stats
    first_visit_date = prefs['first_visit'].split('T')[0] if 'T' in prefs['first_visit'] else prefs['first_visit']
    visit_streak = prefs['visit_count']
    
    print(f"""<!DOCTYPE html>
<html lang="{prefs['language']}">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Task Manager - Server Session</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 2rem;
            color: #333;
            transition: all 0.3s ease;
        }}

        body.dark-theme {{
            background: linear-gradient(135deg, #2c3e50 0%, #34495e 100%);
            color: #ecf0f1;
        }}

        .container {{
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            overflow: hidden;
            transition: all 0.3s ease;
        }}

        .dark-theme .container {{
            background: #2c3e50;
            color: #ecf0f1;
        }}

        .header {{
            background: linear-gradient(45deg, #2c3e50, #3498db);
            color: white;
            padding: 2rem;
            text-align: center;
            position: relative;
        }}

        .user-stats {{
            position: absolute;
            top: 1rem;
            right: 1rem;
            background: rgba(255,255,255,0.2);
            padding: 0.5rem 1rem;
            border-radius: 8px;
            font-size: 0.8rem;
        }}

        .header h1 {{
            font-size: 2.5rem;
            margin-bottom: 0.5rem;
        }}

        .session-panel {{
            background: #34495e;
            color: white;
            padding: 1rem 2rem;
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 1rem;
        }}

        .session-id {{
            font-family: 'Courier New', monospace;
            background: #2c3e50;
            padding: 0.5rem 1rem;
            border-radius: 5px;
            font-size: 0.9rem;
        }}

        .stats {{
            display: flex;
            gap: 1rem;
        }}

        .stat {{
            background: rgba(255,255,255,0.1);
            padding: 0.5rem 1rem;
            border-radius: 5px;
        }}

        .progress-bar {{
            width: 100%;
            height: 8px;
            background: #ecf0f1;
            border-radius: 4px;
            margin-top: 0.5rem;
            overflow: hidden;
        }}

        .dark-theme .progress-bar {{
            background: #34495e;
        }}

        .progress-fill {{
            height: 100%;
            background: #27ae60;
            width: {completed_tasks / total_tasks * 100 if total_tasks > 0 else 0}%;
            transition: width 0.3s ease;
        }}

        .content {{
            padding: 2rem;
            display: grid;
            grid-template-columns: {'250px 1fr' if not prefs['sidebar_collapsed'] else '0px 1fr'};
            gap: 2rem;
            transition: all 0.3s ease;
        }}

        .sidebar {{
            background: #f8f9fa;
            padding: 1.5rem;
            border-radius: 8px;
            transition: all 0.3s ease;
        }}

        .dark-theme .sidebar {{
            background: #34495e;
        }}

        .sidebar.collapsed {{
            width: 0;
            padding: 0;
            overflow: hidden;
        }}

        .main-content {{
            flex: 1;
        }}

        .add-task {{
            display: grid;
            grid-template-columns: 1fr auto auto auto;
            gap: 1rem;
            margin-bottom: 2rem;
        }}

        .task-input {{
            padding: 1rem;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            font-size: 1rem;
            background: white;
            color: #333;
        }}

        .dark-theme .task-input {{
            background: #34495e;
            border-color: #4a6278;
            color: #ecf0f1;
        }}

        .task-input:focus {{
            outline: none;
            border-color: #3498db;
        }}

        .category-select {{
            padding: 1rem;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            font-size: 1rem;
            background: white;
            color: #333;
        }}

        .dark-theme .category-select {{
            background: #34495e;
            border-color: #4a6278;
            color: #ecf0f1;
        }}

        button {{
            padding: 1rem 1.5rem;
            border: none;
            border-radius: 8px;
            background: #3498db;
            color: white;
            cursor: pointer;
            font-size: 1rem;
            transition: all 0.3s ease;
        }}

        button:hover {{
            background: #2980b9;
            transform: translateY(-2px);
        }}

        .task-list {{
            margin-bottom: 2rem;
        }}

        .task-item {{
            display: grid;
            grid-template-columns: auto 1fr auto auto auto;
            align-items: center;
            gap: 1rem;
            padding: 1rem;
            border: 1px solid #e9ecef;
            border-radius: 8px;
            margin-bottom: 0.5rem;
            transition: all 0.3s ease;
            background: white;
        }}

        .dark-theme .task-item {{
            background: #34495e;
            border-color: #4a6278;
            color: #ecf0f1;
        }}

        .task-item:hover {{
            background: #f8f9fa;
            transform: translateX(5px);
        }}

        .dark-theme .task-item:hover {{
            background: #3c5066;
        }}

        .task-item.completed {{
            opacity: 0.6;
            background: #f8f9fa;
        }}

        .dark-theme .task-item.completed {{
            background: #2c3e50;
        }}

        .task-checkbox {{
            width: 20px;
            height: 20px;
            cursor: pointer;
        }}

        .task-text {{
            font-size: 1.1rem;
        }}

        .task-item.completed .task-text {{
            text-decoration: line-through;
        }}

        .task-category {{
            padding: 0.25rem 0.75rem;
            border-radius: 20px;
            font-size: 0.8rem;
            font-weight: bold;
            white-space: nowrap;
        }}

        .category-personal {{ background: #e3f2fd; color: #1976d2; }}
        .category-work {{ background: #f3e5f5; color: #7b1fa2; }}
        .category-shopping {{ background: #e8f5e8; color: #388e3c; }}
        .category-health {{ background: #fff3e0; color: #f57c00; }}
        .category-urgent {{ background: #ffebee; color: #d32f2f; }}

        .task-meta {{
            font-size: 0.8rem;
            color: #666;
        }}

        .dark-theme .task-meta {{
            color: #bdc3c7;
        }}

        .task-actions {{
            display: flex;
            gap: 0.5rem;
        }}

        .delete-btn {{
            background: #e74c3c;
            padding: 0.5rem;
            font-size: 0.9rem;
        }}

        .delete-btn:hover {{
            background: #c0392b;
        }}

        .controls {{
            display: flex;
            gap: 1rem;
            margin-bottom: 2rem;
            flex-wrap: wrap;
        }}

        .btn-danger {{
            background: #e74c3c;
        }}

        .btn-success {{
            background: #27ae60;
        }}

        .btn-warning {{
            background: #f39c12;
        }}

        .btn-info {{
            background: #3498db;
        }}

        .empty-state {{
            text-align: center;
            padding: 3rem;
            color: #666;
        }}

        .dark-theme .empty-state {{
            color: #bdc3c7;
        }}

        .empty-state h3 {{
            margin-bottom: 1rem;
            color: #999;
        }}

        .dark-theme .empty-state h3 {{
            color: #95a5a6;
        }}

        .preference-section {{
            margin-bottom: 1.5rem;
        }}

        .preference-section h3 {{
            margin-bottom: 0.5rem;
            color: #2c3e50;
            border-bottom: 2px solid #3498db;
            padding-bottom: 0.25rem;
        }}

        .dark-theme .preference-section h3 {{
            color: #ecf0f1;
            border-color: #2980b9;
        }}

        .preference-item {{
            margin-bottom: 0.75rem;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }}

        .preference-label {{
            flex: 1;
            font-weight: 500;
        }}

        .preference-control {{
            flex-shrink: 0;
        }}

        select, input[type="number"] {{
            padding: 0.5rem;
            border: 1px solid #bdc3c7;
            border-radius: 4px;
            background: white;
            color: #333;
        }}

        .dark-theme select, .dark-theme input[type="number"] {{
            background: #2c3e50;
            border-color: #4a6278;
            color: #ecf0f1;
        }}

        .toggle-switch {{
            position: relative;
            display: inline-block;
            width: 50px;
            height: 24px;
        }}

        .toggle-switch input {{
            opacity: 0;
            width: 0;
            height: 0;
        }}

        .toggle-slider {{
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 24px;
        }}

        .toggle-slider:before {{
            position: absolute;
            content: "";
            height: 16px;
            width: 16px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }}

        input:checked + .toggle-slider {{
            background-color: #2196F3;
        }}

        input:checked + .toggle-slider:before {{
            transform: translateX(26px);
        }}

        @keyframes fadeIn {{
            from {{ opacity: 0; transform: translateY(10px); }}
            to {{ opacity: 1; transform: translateY(0); }}
        }}

        .task-item {{
            animation: fadeIn 0.3s ease;
        }}

        @media (max-width: 768px) {{
            .container {{
                margin: 1rem;
            }}
            
            .header h1 {{
                font-size: 2rem;
            }}
            
            .add-task {{
                grid-template-columns: 1fr;
            }}
            
            .task-item {{
                grid-template-columns: auto 1fr auto;
            }}
            
            .task-meta {{
                display: none;
            }}
            
            .content {{
                grid-template-columns: 1fr;
            }}
            
            .sidebar {{
                display: none;
            }}
        }}
    </style>
</head>
<body class="{ 'dark-theme' if prefs['theme'] == 'dark' else '' }">
    <div class="container">
        <div class="header">
            <h1>✅ Task Manager</h1>
            <p>Server Session-Based Task Management</p>
            <div class="user-stats">
                👤 Visit #{prefs['visit_count']} | 🎯 {pending_tasks} pending
            </div>
        </div>
        
        <div class="session-panel">
            <div class="session-info">
                <strong>Session:</strong> 
                <span class="session-id">{session_id or "No session"}</span>
            </div>
            <div class="stats">
                <div class="stat">Total: {total_tasks}</div>
                <div class="stat">Pending: {pending_tasks}</div>
                <div class="stat">Completed: {completed_tasks}</div>
            </div>
        </div>

        {f'''
        <div style="padding: 0 2rem;">
            <div class="progress-bar">
                <div class="progress-fill"></div>
            </div>
            <div style="text-align: center; color: #666; font-size: 0.9rem; margin-top: 0.25rem;">
                {completed_tasks}/{total_tasks} tasks completed ({int(completed_tasks/total_tasks*100) if total_tasks > 0 else 0}%)
            </div>
        </div>
        ''' if total_tasks > 0 else ''}
        
        <div class="content">
            <!-- Sidebar with Preferences -->
            <div class="sidebar {'collapsed' if prefs['sidebar_collapsed'] else ''}" id="sidebar">
                <h2>⚙️ Preferences</h2>
                
                <div class="preference-section">
                    <h3>🎨 Appearance</h3>
                    
                    <div class="preference-item">
                        <span class="preference-label">Theme</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_theme" value="1">
                                <select name="theme" onchange="this.form.submit()">
                                    <option value="light" {'selected' if prefs['theme'] == 'light' else ''}>Light</option>
                                    <option value="dark" {'selected' if prefs['theme'] == 'dark' else ''}>Dark</option>
                                </select>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-item">
                        <span class="preference-label">View Mode</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_view_mode" value="1">
                                <select name="view_mode" onchange="this.form.submit()">
                                    <option value="list" {'selected' if prefs['view_mode'] == 'list' else ''}>List</option>
                                    <option value="grid" {'selected' if prefs['view_mode'] == 'grid' else ''}>Grid</option>
                                </select>
                            </form>
                        </div>
                    </div>
                </div>
                
                <div class="preference-section">
                    <h3>📊 Display</h3>
                    
                    <div class="preference-item">
                        <span class="preference-label">Tasks per page</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_tasks_per_page" value="1">
                                <select name="tasks_per_page" onchange="this.form.submit()">
                                    <option value="5" {'selected' if prefs['tasks_per_page'] == 5 else ''}>5</option>
                                    <option value="10" {'selected' if prefs['tasks_per_page'] == 10 else ''}>10</option>
                                    <option value="25" {'selected' if prefs['tasks_per_page'] == 25 else ''}>25</option>
                                    <option value="50" {'selected' if prefs['tasks_per_page'] == 50 else ''}>50</option>
                                </select>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-item">
                        <span class="preference-label">Sort by</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_sort_preference" value="1">
                                <select name="sort_preference" onchange="this.form.submit()">
                                    <option value="created_desc" {'selected' if prefs['sort_preference'] == 'created_desc' else ''}>Newest First</option>
                                    <option value="created_asc" {'selected' if prefs['sort_preference'] == 'created_asc' else ''}>Oldest First</option>
                                    <option value="priority" {'selected' if prefs['sort_preference'] == 'priority' else ''}>Priority</option>
                                    <option value="name" {'selected' if prefs['sort_preference'] == 'name' else ''}>Name</option>
                                    <option value="completed" {'selected' if prefs['sort_preference'] == 'completed' else ''}>Completion</option>
                                </select>
                            </form>
                        </div>
                    </div>
                </div>
                
                <div class="preference-section">
                    <h3>🔧 Features</h3>
                    
                    <div class="preference-item">
                        <span class="preference-label">Show Completed</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="toggle_completed_visible" value="1">
                                <label class="toggle-switch">
                                    <input type="checkbox" {'checked' if prefs['completed_visible'] else ''} onchange="this.form.submit()">
                                    <span class="toggle-slider"></span>
                                </label>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-item">
                        <span class="preference-label">Auto Save</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="toggle_auto_save" value="1">
                                <label class="toggle-switch">
                                    <input type="checkbox" {'checked' if prefs['auto_save'] else ''} onchange="this.form.submit()">
                                    <span class="toggle-slider"></span>
                                </label>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-item">
                        <span class="preference-label">Notifications</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="toggle_notifications" value="1">
                                <label class="toggle-switch">
                                    <input type="checkbox" {'checked' if prefs['notifications'] else ''} onchange="this.form.submit()">
                                    <span class="toggle-slider"></span>
                                </label>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-item">
                        <span class="preference-label">Keyboard Shortcuts</span>
                        <div class="preference-control">
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="toggle_keyboard_shortcuts" value="1">
                                <label class="toggle-switch">
                                    <input type="checkbox" {'checked' if prefs['keyboard_shortcuts'] else ''} onchange="this.form.submit()">
                                    <span class="toggle-slider"></span>
                                </label>
                            </form>
                        </div>
                    </div>
                </div>
                
                <div class="preference-section">
                    <h3>👤 User Info</h3>
                    <p><small>First visit: {first_visit_date}</small></p>
                    <p><small>Visit streak: {visit_streak} days</small></p>
                    <p><small>Language: {prefs['language'].upper()}</small></p>
                </div>
            </div>
            
            <!-- Main Content -->
            <div class="main-content">
                <!-- Add Task Form -->
                <form method="POST" class="add-task" id="taskForm">
                    <input type="hidden" name="action" value="add">
                    <input type="text" name="task_text" class="task-input" 
                           placeholder="What needs to be done?" required id="taskInput"
                           value="">
                    <select name="category" class="category-select">
                        <option value="personal" {'selected' if prefs['default_category'] == 'personal' else ''}>👤 Personal</option>
                        <option value="work" {'selected' if prefs['default_category'] == 'work' else ''}>💼 Work</option>
                        <option value="shopping" {'selected' if prefs['default_category'] == 'shopping' else ''}>🛒 Shopping</option>
                        <option value="health" {'selected' if prefs['default_category'] == 'health' else ''}>🏥 Health</option>
                        <option value="urgent" {'selected' if prefs['default_category'] == 'urgent' else ''}>🚨 Urgent</option>
                    </select>
                    <button type="submit" style="background: #27ae60;">➕ Add Task</button>
                </form>
                
                <!-- Controls -->
                <div class="controls">
                    <form method="POST">
                        <input type="hidden" name="action" value="clear_completed">
                        <button type="submit" class="btn-warning">🗑️ Clear Completed</button>
                    </form>
                    <form method="POST">
                        <input type="hidden" name="action" value="clear_all">
                        <button type="submit" class="btn-danger" onclick="return confirm('Clear ALL tasks? This cannot be undone!')">💣 Clear All</button>
                    </form>
                    <form method="POST" style="display: inline;">
                        <input type="hidden" name="toggle_sidebar" value="1">
                        <button type="submit" class="btn-info">
                            {'📖 Show Sidebar' if prefs['sidebar_collapsed'] else '📕 Hide Sidebar'}
                        </button>
                    </form>
                    <button onclick="location.reload()">🔄 Refresh</button>
                    <button onclick="showSessionInfo()">🔍 Session Info</button>
                    <a href="/cgi-bin/session_debug.py" style="padding: 1rem 1.5rem; background: #95a5a6; color: white; text-decoration: none; border-radius: 8px;">🐛 Debug</a>
                </div>
                
                <!-- Task List -->
                <div class="task-list">
                    {f'<h2>Your Tasks ({total_tasks})</h2>' if tasks else ''}
                    {task_html}
                </div>
                
                <!-- Debug Info -->
                <div style="margin-top: 2rem; padding: 1rem; background: #f8f9fa; border-radius: 8px; display: none;" id="debug-info">
                    <h3>Debug Information</h3>
                    <p><strong>Session ID:</strong> {session_id}</p>
                    <p><strong>Tasks File:</strong> /tmp/tasks_{session_id}.json</p>
                    <p><strong>Total Tasks:</strong> {total_tasks}</p>
                    <p><strong>Request Method:</strong> {method}</p>
                    <button onclick="hideSessionInfo()">Hide Debug</button>
                </div>
            </div>
        </div>
    </div>

    <script>
        function showSessionInfo() {{
            document.getElementById('debug-info').style.display = 'block';
        }}
        
        function hideSessionInfo() {{
            document.getElementById('debug-info').style.display = 'none';
        }}
        
        // Auto-focus on task input and clear it after submission
        const taskInput = document.getElementById('taskInput');
        if (taskInput) {{
            taskInput.focus();
            
            // Clear input after form submission to prevent duplicate submissions
            document.getElementById('taskForm').addEventListener('submit', function() {{
                setTimeout(() => {{
                    taskInput.value = '';
                }}, 100);
            }});
        }}
    </script>
</body>
</html>""")

if __name__ == "__main__":
    main()