#!/usr/bin/env python3
import os
import json
import cgi
import cgitb
from datetime import datetime, timedelta
import sys
import random

# Enable error reporting
cgitb.enable()

# Cookie configuration
COOKIE_CONFIG = {
    'theme': {'max_age': 365 * 24 * 60 * 60, 'path': '/'},
    'tasks_per_page': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'sort_preference': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'default_category': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'sidebar_collapsed': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'completed_visible': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'auto_save': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'language': {'max_age': 30 * 24 * 60 * 60, 'path': '/'},
    'first_visit': {'max_age': 365 * 24 * 60 * 60, 'path': '/'},
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
        set_cookie('theme', 'cyber', COOKIE_CONFIG['theme']['max_age'])
        cookies_to_set.append('theme')
    
    # Tasks per page
    if not get_cookie('tasks_per_page'):
        set_cookie('tasks_per_page', '15', COOKIE_CONFIG['tasks_per_page']['max_age'])
        cookies_to_set.append('tasks_per_page')
    
    # Sort preference
    if not get_cookie('sort_preference'):
        set_cookie('sort_preference', 'created_desc', COOKIE_CONFIG['sort_preference']['max_age'])
        cookies_to_set.append('sort_preference')
    
    # Default category
    if not get_cookie('default_category'):
        set_cookie('default_category', 'personal', COOKIE_CONFIG['default_category']['max_age'])
        cookies_to_set.append('default_category')
    
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
    
    # Language
    if not get_cookie('language'):
        set_cookie('language', 'en', COOKIE_CONFIG['language']['max_age'])
        cookies_to_set.append('language')
    
    # Visit tracking
    if not get_cookie('first_visit'):
        first_visit = datetime.now().isoformat()
        set_cookie('first_visit', first_visit, COOKIE_CONFIG['first_visit']['max_age'])
        cookies_to_set.append('first_visit')
    
    return cookies_to_set

def get_user_preferences():
    """Get all user preferences from cookies"""
    return {
        'theme': get_cookie('theme', 'cyber'),
        'tasks_per_page': int(get_cookie('tasks_per_page', '15')),
        'sort_preference': get_cookie('sort_preference', 'created_desc'),
        'default_category': get_cookie('default_category', 'personal'),
        'sidebar_collapsed': get_cookie('sidebar_collapsed', 'false') == 'true',
        'completed_visible': get_cookie('completed_visible', 'true') == 'true',
        'auto_save': get_cookie('auto_save', 'true') == 'true',
        'language': get_cookie('language', 'en'),
        'first_visit': get_cookie('first_visit', datetime.now().isoformat()),
    }

def load_tasks(session_id):
    """Load tasks from session file"""
    if not session_id:
        return []
    
    task_file = f"/tmp/tasks_{session_id}.json"
    try:
        with open(task_file, 'r') as f:
            tasks = json.load(f)
            # Ensure each task has all required fields
            for task in tasks:
                task.setdefault('created_at', datetime.now().isoformat())
                task.setdefault('category', 'personal')
            return tasks
    except (FileNotFoundError, json.JSONDecodeError):
        return []

def save_tasks(session_id, tasks):
    """Save tasks to session file"""
    if not session_id:
        return False
    
    task_file = f"/tmp/tasks_{session_id}.json"
    try:
        with open(task_file, 'w') as f:
            json.dump(tasks, f, indent=2, default=str)
        return True
    except Exception as e:
        print(f"Error saving tasks: {e}", file=sys.stderr)
        return False

def generate_task_id(tasks):
    """Generate unique task ID"""
    if not tasks:
        return 1
    return max(task.get('id', 0) for task in tasks) + 1

def sort_tasks(tasks, sort_preference):
    """Sort tasks based on user preference"""
    if not tasks:
        return []
        
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
        'urgent': '🚨',
        'ideas': '💡',
        'learning': '📚',
        'travel': '✈️',
        'finance': '💰',
        'fitness': '💪'
    }
    return emojis.get(category, '📝')

def get_category_color(category):
    """Get CSS color for category"""
    colors = {
        'personal': '#00ff88',
        'work': '#0099ff', 
        'shopping': '#ffaa00',
        'health': '#ff4444',
        'urgent': '#ff0066',
        'ideas': '#aa00ff',
        'learning': '#00ddff',
        'travel': '#ff8800',
        'finance': '#00ff44',
        'fitness': '#ff0088'
    }
    return colors.get(category, '#cccccc')

def format_relative_date(date_string):
    """Format date as relative time"""
    try:
        if 'T' in date_string:
            date_obj = datetime.fromisoformat(date_string.replace('Z', '+00:00'))
        else:
            date_obj = datetime.strptime(date_string, '%Y-%m-%d')
        
        now = datetime.now()
        diff = now - date_obj
        
        if diff.days == 0:
            return "Today"
        elif diff.days == 1:
            return "Yesterday"
        elif diff.days < 7:
            return f"{diff.days} days ago"
        elif diff.days < 30:
            weeks = diff.days // 7
            return f"{weeks} week{'s' if weeks > 1 else ''} ago"
        else:
            return date_obj.strftime('%b %d, %Y')
    except:
        return date_string

def generate_task_html(tasks, prefs):
    """Generate HTML for task list based on user preferences"""
    if not tasks:
        motivational_quotes = [
            "The future belongs to those who prepare for it today.",
            "Innovation distinguishes between a leader and a follower.",
            "Your time is limited, don't waste it living someone else's life.",
            "The only way to do great work is to love what you do.",
            "Stay hungry, stay foolish."
        ]
        quote = random.choice(motivational_quotes)
        
        return f'''
                <div class="empty-state">
                    <div class="empty-icon">🚀</div>
                    <h3>Mission Control Ready</h3>
                    <p>{quote}</p>
                    <div class="empty-actions">
                        <button onclick="document.getElementById('taskInput').focus()" class="btn-primary glow">
                            ⚡ Launch New Task
                        </button>
                    </div>
                </div>
                '''
    
    # Filter tasks based on visibility preference
    display_tasks = tasks if prefs['completed_visible'] else [t for t in tasks if not t['completed']]
    
    # Sort tasks
    display_tasks = sort_tasks(display_tasks, prefs['sort_preference'])
    
    # Apply pagination
    tasks_per_page = prefs['tasks_per_page']
    display_tasks = display_tasks[:tasks_per_page]
    
    task_html = []
    for task in display_tasks:
        completed_class = 'completed' if task['completed'] else ''
        checked = 'checked' if task['completed'] else ''
        category_emoji = get_category_emoji(task['category'])
        category_color = get_category_color(task['category'])
        
        created_date = format_relative_date(task.get("created_at", ""))
        
        # Add priority indicator for urgent tasks
        priority_badge = '🔴' if task['category'] == 'urgent' else ''
        
        task_html.append(f'''
                <div class="task-item {completed_class}" data-task-id="{task['id']}">
                    <div class="task-main">
                        <form method="POST" style="display: inline;">
                            <input type="hidden" name="action" value="toggle">
                            <input type="hidden" name="task_id" value="{task['id']}">
                            <label class="cyber-checkbox">
                                <input type="checkbox" class="task-checkbox" {checked} onchange="this.form.submit()">
                                <span class="checkmark"></span>
                            </label>
                        </form>
                        <div class="task-content">
                            <div class="task-text">{task['text']} {priority_badge}</div>
                            <div class="task-meta">
                                <span class="task-category" style="--category-color: {category_color};">
                                    {category_emoji} {task['category'].title()}
                                </span>
                                <span class="task-date">{created_date}</span>
                            </div>
                        </div>
                    </div>
                    <div class="task-actions">
                        <form method="POST" style="display: inline;">
                            <input type="hidden" name="action" value="delete">
                            <input type="hidden" name="task_id" value="{task['id']}">
                            <button type="submit" class="btn-icon delete-btn" onclick="return confirm('Delete this task?')" title="Delete">
                                <svg width="16" height="16" viewBox="0 0 24 24" fill="none">
                                    <path d="M6 7V18C6 19.1046 6.89543 20 8 20H16C17.1046 20 18 19.1046 18 18V7M6 7H5M6 7H8M18 7H19M18 7H16M10 11V16M14 11V16M8 7V5C8 3.89543 8.89543 3 10 3H14C15.1046 3 16 3.89543 16 5V7" stroke="currentColor" stroke-width="2"/>
                                </svg>
                            </button>
                        </form>
                    </div>
                </div>
                ''')
    
    return ''.join(task_html)

def handle_preference_update(form):
    """Handle user preference updates from forms"""
    updated_prefs = {}
    
    if form.getvalue('update_theme'):
        theme = form.getvalue('theme', 'cyber')
        set_cookie('theme', theme, COOKIE_CONFIG['theme']['max_age'])
        updated_prefs['theme'] = theme
    
    if form.getvalue('update_tasks_per_page'):
        tasks_per_page = form.getvalue('tasks_per_page', '15')
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
    
    if form.getvalue('update_language'):
        language = form.getvalue('language', 'en')
        set_cookie('language', language, COOKIE_CONFIG['language']['max_age'])
        updated_prefs['language'] = language
    
    return updated_prefs

def get_productivity_score(tasks):
    """Calculate productivity score based on completed tasks"""
    if not tasks:
        return 100
    
    total_tasks = len(tasks)
    completed_tasks = len([t for t in tasks if t['completed']])
    
    if total_tasks == 0:
        return 100
    
    score = (completed_tasks / total_tasks) * 100
    
    # Bonus for having many completed tasks
    if completed_tasks >= 10:
        score = min(100, score + 10)
    
    return int(score)

def get_efficiency_level(score):
    """Get efficiency level based on productivity score"""
    if score >= 90:
        return ("Quantum", "#00ff88")
    elif score >= 75:
        return ("Optimal", "#00ddff")
    elif score >= 60:
        return ("Stable", "#ffaa00")
    else:
        return ("Critical", "#ff0066")

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
        
        # Redirect to avoid form resubmission
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
    
    # Count stats
    total_tasks = len(tasks)
    completed_tasks = len([t for t in tasks if t['completed']])
    pending_tasks = total_tasks - completed_tasks
    
    # Calculate productivity metrics
    productivity_score = get_productivity_score(tasks)
    efficiency_level, efficiency_color = get_efficiency_level(productivity_score)
    completion_rate = int((completed_tasks / total_tasks * 100)) if total_tasks > 0 else 0
    
    # Generate task HTML
    task_html = generate_task_html(tasks, prefs)
    
    # Calculate user stats
    first_visit_date = prefs['first_visit'].split('T')[0] if 'T' in prefs['first_visit'] else prefs['first_visit']
    
    # Output HTML
    print("Content-Type: text/html")
    print()
    
    # Build the HTML step by step to avoid f-string complexity
    html_parts = []
    
    # Start of HTML
    html_parts.append(f"""<!DOCTYPE html>
<html lang="{prefs['language']}">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Nexus Task Manager v2.0</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@400;500;700;900&family=Rajdhani:wght@300;400;500;600;700&display=swap');
        
        :root {{
            --primary-glow: #00ff88;
            --secondary-glow: #0099ff;
            --accent-glow: #ff0066;
            --cyber-blue: #00ddff;
            --cyber-green: #00ff88;
            --cyber-purple: #aa00ff;
            --cyber-orange: #ffaa00;
            --cyber-red: #ff0066;
            --dark-bg: #0a0a0f;
            --darker-bg: #050508;
            --card-bg: rgba(20, 20, 30, 0.7);
            --card-border: rgba(0, 255, 136, 0.3);
            --text-primary: #ffffff;
            --text-secondary: #a0a0c0;
            --text-accent: #00ff88;
        }}

        .light-theme {{
            --dark-bg: #f0f2ff;
            --darker-bg: #e0e4ff;
            --card-bg: rgba(255, 255, 255, 0.9);
            --card-border: rgba(0, 157, 255, 0.3);
            --text-primary: #1a1a2e;
            --text-secondary: #4a4a6e;
            --text-accent: #0099ff;
        }}

        .matrix-theme {{
            --primary-glow: #00ff41;
            --secondary-glow: #008f11;
            --cyber-green: #00ff41;
            --dark-bg: #001100;
            --darker-bg: #000800;
            --card-bg: rgba(0, 30, 0, 0.8);
            --card-border: rgba(0, 255, 65, 0.3);
            --text-primary: #00ff41;
            --text-secondary: #00a829;
        }}

        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        body {{
            font-family: 'Rajdhani', sans-serif;
            background: var(--dark-bg);
            color: var(--text-primary);
            min-height: 100vh;
            overflow-x: hidden;
            position: relative;
        }}

        body::before {{
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: 
                radial-gradient(circle at 20% 80%, rgba(0, 255, 136, 0.1) 0%, transparent 50%),
                radial-gradient(circle at 80% 20%, rgba(0, 157, 255, 0.1) 0%, transparent 50%),
                radial-gradient(circle at 40% 40%, rgba(170, 0, 255, 0.05) 0%, transparent 50%);
            pointer-events: none;
            z-index: -1;
        }}

        .cyber-grid {{
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-image: 
                linear-gradient(rgba(0, 255, 136, 0.1) 1px, transparent 1px),
                linear-gradient(90deg, rgba(0, 255, 136, 0.1) 1px, transparent 1px);
            background-size: 50px 50px;
            pointer-events: none;
            z-index: -1;
            opacity: 0.3;
        }}

        .container {{
            max-width: 1400px;
            margin: 0 auto;
            padding: 1rem;
            position: relative;
        }}

        /* Futuristic Header */
        .header {{
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 20px;
            padding: 2rem;
            margin-bottom: 2rem;
            position: relative;
            overflow: hidden;
            box-shadow: 
                0 0 50px rgba(0, 255, 136, 0.1),
                inset 0 1px 0 rgba(255, 255, 255, 0.1);
        }}

        .header::before {{
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(0, 255, 136, 0.1), transparent);
            animation: scan 3s linear infinite;
        }}

        @keyframes scan {{
            0% {{ left: -100%; }}
            100% {{ left: 100%; }}
        }}

        .header-content {{
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 2rem;
            position: relative;
            z-index: 2;
        }}

        .logo {{
            display: flex;
            align-items: center;
            gap: 1rem;
        }}

        .logo-icon {{
            font-size: 3rem;
            background: linear-gradient(45deg, var(--primary-glow), var(--secondary-glow));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            filter: drop-shadow(0 0 20px var(--primary-glow));
        }}

        .logo-text h1 {{
            font-family: 'Orbitron', monospace;
            font-size: 2.5rem;
            font-weight: 900;
            background: linear-gradient(45deg, var(--text-primary), var(--text-accent));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            text-shadow: 0 0 30px rgba(0, 255, 136, 0.5);
        }}

        .logo-text p {{
            color: var(--text-secondary);
            font-size: 1.1rem;
            letter-spacing: 2px;
            text-transform: uppercase;
        }}

        .efficiency-display {{
            text-align: center;
            padding: 1rem 2rem;
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--card-border);
            border-radius: 15px;
            backdrop-filter: blur(10px);
        }}

        .efficiency-level {{
            font-family: 'Orbitron', monospace;
            font-size: 1.5rem;
            font-weight: 700;
            color: {efficiency_color};
            text-shadow: 0 0 10px {efficiency_color};
            margin-bottom: 0.5rem;
        }}

        .efficiency-score {{
            font-size: 2.5rem;
            font-weight: 900;
            background: linear-gradient(45deg, {efficiency_color}, var(--secondary-glow));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }}

        /* Stats Grid */
        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 1.5rem;
            margin-bottom: 2rem;
        }}

        .stat-card {{
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 15px;
            padding: 1.5rem;
            text-align: center;
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }}

        .stat-card::before {{
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: linear-gradient(45deg, transparent, rgba(0, 255, 136, 0.05), transparent);
            opacity: 0;
            transition: opacity 0.3s ease;
        }}

        .stat-card:hover::before {{
            opacity: 1;
        }}

        .stat-card:hover {{
            transform: translateY(-5px);
            box-shadow: 
                0 10px 30px rgba(0, 255, 136, 0.2),
                0 0 0 1px rgba(0, 255, 136, 0.3);
        }}

        .stat-value {{
            font-family: 'Orbitron', monospace;
            font-size: 2.5rem;
            font-weight: 700;
            background: linear-gradient(45deg, var(--text-primary), var(--text-accent));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 0.5rem;
        }}

        .stat-label {{
            color: var(--text-secondary);
            font-size: 0.9rem;
            text-transform: uppercase;
            letter-spacing: 1px;
        }}

        /* Main Content Layout */
        .content {{
            display: grid;
            grid-template-columns: {'320px 1fr' if not prefs['sidebar_collapsed'] else '80px 1fr'};
            gap: 2rem;
            transition: grid-template-columns 0.4s cubic-bezier(0.4, 0, 0.2, 1);
        }}

        /* Enhanced Sidebar */
        .sidebar {{
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 20px;
            padding: 2rem;
            height: fit-content;
            position: sticky;
            top: 2rem;
            transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
            overflow: hidden;
        }}

        .sidebar.collapsed {{
            padding: 2rem 1rem;
            width: 80px;
        }}

        .sidebar.collapsed .sidebar-content {{
            opacity: 0;
            pointer-events: none;
        }}

        .sidebar-header {{
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 2rem;
        }}

        .sidebar.collapsed .sidebar-header {{
            justify-content: center;
        }}

        .sidebar-title {{
            font-family: 'Orbitron', monospace;
            font-size: 1.2rem;
            color: var(--text-accent);
            text-transform: uppercase;
            letter-spacing: 2px;
        }}

        .sidebar.collapsed .sidebar-title {{
            display: none;
        }}

        .toggle-sidebar {{
            background: rgba(0, 255, 136, 0.1);
            border: 1px solid var(--card-border);
            border-radius: 10px;
            padding: 0.5rem;
            cursor: pointer;
            transition: all 0.3s ease;
            color: var(--text-accent);
        }}

        .toggle-sidebar:hover {{
            background: rgba(0, 255, 136, 0.2);
            transform: scale(1.1);
            box-shadow: 0 0 20px rgba(0, 255, 136, 0.3);
        }}

        .sidebar-content {{
            transition: opacity 0.3s ease;
        }}

        .sidebar.collapsed .sidebar-content {{
            display: none;
        }}

        .preference-section {{
            margin-bottom: 2rem;
            padding-bottom: 1.5rem;
            border-bottom: 1px solid rgba(0, 255, 136, 0.2);
        }}

        .preference-section:last-child {{
            border-bottom: none;
        }}

        .preference-section h3 {{
            font-family: 'Orbitron', monospace;
            font-size: 0.9rem;
            color: var(--text-accent);
            text-transform: uppercase;
            letter-spacing: 1px;
            margin-bottom: 1rem;
        }}

        .preference-item {{
            margin-bottom: 1rem;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }}

        .preference-label {{
            color: var(--text-secondary);
            font-size: 0.9rem;
        }}

        .cyber-select {{
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--card-border);
            border-radius: 8px;
            padding: 0.5rem;
            color: var(--text-primary);
            font-family: 'Rajdhani', sans-serif;
            min-width: 120px;
        }}

        .cyber-select:focus {{
            outline: none;
            border-color: var(--primary-glow);
            box-shadow: 0 0 10px rgba(0, 255, 136, 0.3);
        }}

        .cyber-toggle {{
            position: relative;
            display: inline-block;
            width: 50px;
            height: 24px;
        }}

        .cyber-toggle input {{
            opacity: 0;
            width: 0;
            height: 0;
        }}

        .cyber-slider {{
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--card-border);
            transition: .4s;
            border-radius: 24px;
        }}

        .cyber-slider:before {{
            position: absolute;
            content: "";
            height: 16px;
            width: 16px;
            left: 4px;
            bottom: 3px;
            background: var(--text-secondary);
            transition: .4s;
            border-radius: 50%;
        }}

        input:checked + .cyber-slider {{
            background: rgba(0, 255, 136, 0.2);
            border-color: var(--primary-glow);
        }}

        input:checked + .cyber-slider:before {{
            transform: translateX(26px);
            background: var(--primary-glow);
            box-shadow: 0 0 10px var(--primary-glow);
        }}

        /* Main Content */
        .main-content {{
            min-height: 500px;
        }}

        /* Quick Actions */
        .quick-actions {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 1rem;
            margin-bottom: 2rem;
        }}

        .action-card {{
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 15px;
            padding: 1.5rem;
            text-align: center;
            cursor: pointer;
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }}

        .action-card::before {{
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(0, 255, 136, 0.1), transparent);
            transition: left 0.5s ease;
        }}

        .action-card:hover::before {{
            left: 100%;
        }}

        .action-card:hover {{
            transform: translateY(-5px);
            box-shadow: 
                0 10px 30px rgba(0, 255, 136, 0.2),
                0 0 0 1px rgba(0, 255, 136, 0.3);
        }}

        .action-icon {{
            font-size: 2rem;
            margin-bottom: 0.5rem;
            filter: drop-shadow(0 0 10px rgba(0, 255, 136, 0.5));
        }}

        .action-card h4 {{
            color: var(--text-primary);
            margin-bottom: 0.5rem;
            font-family: 'Orbitron', monospace;
        }}

        .action-card p {{
            color: var(--text-secondary);
            font-size: 0.8rem;
        }}

        /* Add Task Form */
        .add-task-form {{
            display: grid;
            grid-template-columns: 1fr auto auto auto;
            gap: 1rem;
            margin-bottom: 2rem;
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 15px;
            padding: 1.5rem;
            position: relative;
            overflow: hidden;
        }}

        .add-task-form::before {{
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: linear-gradient(45deg, transparent, rgba(0, 255, 136, 0.05), transparent);
            opacity: 0.5;
        }}

        .task-input {{
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--card-border);
            border-radius: 10px;
            padding: 1rem 1.5rem;
            color: var(--text-primary);
            font-family: 'Rajdhani', sans-serif;
            font-size: 1rem;
            transition: all 0.3s ease;
            position: relative;
            z-index: 2;
        }}

        .task-input:focus {{
            outline: none;
            border-color: var(--primary-glow);
            box-shadow: 
                0 0 20px rgba(0, 255, 136, 0.3),
                inset 0 0 20px rgba(0, 255, 136, 0.1);
        }}

        .task-input::placeholder {{
            color: var(--text-secondary);
        }}

        .category-select {{
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--card-border);
            border-radius: 10px;
            padding: 1rem 1.5rem;
            color: var(--text-primary);
            font-family: 'Rajdhani', sans-serif;
            cursor: pointer;
            position: relative;
            z-index: 2;
            min-width: 140px;
        }}

        .btn {{
            background: linear-gradient(45deg, var(--primary-glow), var(--secondary-glow));
            border: none;
            border-radius: 10px;
            padding: 1rem 1.5rem;
            color: #000;
            font-family: 'Orbitron', monospace;
            font-weight: 700;
            cursor: pointer;
            transition: all 0.3s ease;
            position: relative;
            z-index: 2;
            text-transform: uppercase;
            letter-spacing: 1px;
        }}

        .btn:hover {{
            transform: translateY(-2px);
            box-shadow: 
                0 10px 30px rgba(0, 255, 136, 0.4),
                0 0 30px rgba(0, 255, 136, 0.2);
        }}

        .btn.glow {{
            animation: pulse 2s infinite;
        }}

        @keyframes pulse {{
            0%, 100% {{ box-shadow: 0 0 20px rgba(0, 255, 136, 0.4); }}
            50% {{ box-shadow: 0 0 40px rgba(0, 255, 136, 0.8), 0 0 60px rgba(0, 255, 136, 0.4); }}
        }}

        /* Task List */
        .task-list {{
            margin-bottom: 2rem;
        }}

        .task-section-title {{
            font-family: 'Orbitron', monospace;
            font-size: 1.5rem;
            color: var(--text-accent);
            margin-bottom: 1.5rem;
            text-transform: uppercase;
            letter-spacing: 2px;
            text-shadow: 0 0 10px rgba(0, 255, 136, 0.5);
        }}

        .task-item {{
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 15px;
            padding: 1.5rem;
            margin-bottom: 1rem;
            display: flex;
            justify-content: space-between;
            align-items: center;
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }}

        .task-item::before {{
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 4px;
            height: 100%;
            background: var(--category-color, var(--primary-glow));
            transition: width 0.3s ease;
        }}

        .task-item:hover::before {{
            width: 8px;
        }}

        .task-item:hover {{
            transform: translateX(10px);
            box-shadow: 
                0 10px 30px rgba(0, 255, 136, 0.2),
                0 0 0 1px rgba(0, 255, 136, 0.3);
        }}

        .task-item.completed {{
            opacity: 0.6;
            background: rgba(20, 20, 30, 0.5);
        }}

        .task-main {{
            display: flex;
            align-items: center;
            gap: 1rem;
            flex: 1;
        }}

        .cyber-checkbox {{
            position: relative;
            cursor: pointer;
        }}

        .cyber-checkbox input {{
            position: absolute;
            opacity: 0;
            cursor: pointer;
        }}

        .checkmark {{
            width: 24px;
            height: 24px;
            background: rgba(0, 0, 0, 0.5);
            border: 2px solid var(--card-border);
            border-radius: 6px;
            display: flex;
            align-items: center;
            justify-content: center;
            transition: all 0.3s ease;
        }}

        .cyber-checkbox input:checked ~ .checkmark {{
            background: rgba(0, 255, 136, 0.2);
            border-color: var(--primary-glow);
            box-shadow: 0 0 10px rgba(0, 255, 136, 0.5);
        }}

        .checkmark:after {{
            content: "✓";
            color: var(--primary-glow);
            font-weight: bold;
            opacity: 0;
            transition: opacity 0.3s ease;
            text-shadow: 0 0 10px var(--primary-glow);
        }}

        .cyber-checkbox input:checked ~ .checkmark:after {{
            opacity: 1;
        }}

        .task-content {{
            flex: 1;
        }}

        .task-text {{
            font-size: 1.1rem;
            font-weight: 500;
            margin-bottom: 0.5rem;
            color: var(--text-primary);
        }}

        .task-item.completed .task-text {{
            text-decoration: line-through;
            color: var(--text-secondary);
        }}

        .task-meta {{
            display: flex;
            gap: 1rem;
            align-items: center;
            flex-wrap: wrap;
        }}

        .task-category {{
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--category-color);
            border-radius: 20px;
            padding: 0.25rem 0.75rem;
            font-size: 0.8rem;
            font-weight: 600;
            color: var(--category-color);
            text-shadow: 0 0 10px var(--category-color);
        }}

        .task-date {{
            font-size: 0.8rem;
            color: var(--text-secondary);
        }}

        .task-actions {{
            display: flex;
            gap: 0.5rem;
        }}

        .btn-icon {{
            background: rgba(0, 0, 0, 0.5);
            border: 1px solid var(--card-border);
            border-radius: 8px;
            padding: 0.5rem;
            color: var(--text-secondary);
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
        }}

        .btn-icon:hover {{
            background: rgba(255, 0, 102, 0.2);
            border-color: var(--cyber-red);
            color: var(--cyber-red);
            transform: scale(1.1);
            box-shadow: 0 0 10px rgba(255, 0, 102, 0.3);
        }}

        /* Empty State */
        .empty-state {{
            text-align: center;
            padding: 4rem 2rem;
            background: var(--card-bg);
            backdrop-filter: blur(20px);
            border: 1px solid var(--card-border);
            border-radius: 20px;
        }}

        .empty-icon {{
            font-size: 4rem;
            margin-bottom: 1rem;
            filter: drop-shadow(0 0 20px rgba(0, 255, 136, 0.5));
        }}

        .empty-state h3 {{
            font-family: 'Orbitron', monospace;
            font-size: 1.5rem;
            color: var(--text-accent);
            margin-bottom: 1rem;
            text-transform: uppercase;
            letter-spacing: 2px;
        }}

        .empty-state p {{
            color: var(--text-secondary);
            margin-bottom: 2rem;
            font-size: 1.1rem;
        }}

        /* Control Buttons */
        .control-buttons {{
            display: flex;
            gap: 1rem;
            margin-bottom: 2rem;
            flex-wrap: wrap;
        }}

        .btn-warning {{
            background: linear-gradient(45deg, var(--cyber-orange), #ff6600);
        }}

        .btn-danger {{
            background: linear-gradient(45deg, var(--cyber-red), #ff0044);
        }}

        .btn-secondary {{
            background: linear-gradient(45deg, var(--text-secondary), #6b7280);
        }}

        /* Responsive Design */
        @media (max-width: 1024px) {{
            .content {{
                grid-template-columns: 1fr;
            }}
            
            .sidebar {{
                position: static;
            }}
            
            .sidebar.collapsed {{
                width: auto;
                padding: 1rem;
            }}
            
            .sidebar.collapsed .sidebar-content {{
                display: none;
            }}
        }}

        @media (max-width: 768px) {{
            .container {{
                padding: 0.5rem;
            }}
            
            .header-content {{
                flex-direction: column;
                text-align: center;
            }}
            
            .add-task-form {{
                grid-template-columns: 1fr;
            }}
            
            .task-item {{
                flex-direction: column;
                align-items: stretch;
                gap: 1rem;
            }}
            
            .task-main {{
                flex-direction: column;
                align-items: flex-start;
            }}
            
            .stats-grid {{
                grid-template-columns: 1fr;
            }}
            
            .quick-actions {{
                grid-template-columns: 1fr;
            }}
            
            .control-buttons {{
                flex-direction: column;
            }}
        }}

        /* Cyberpunk Animations */
        @keyframes float {{
            0%, 100% {{ transform: translateY(0px); }}
            50% {{ transform: translateY(-10px); }}
        }}

        .floating {{
            animation: float 3s ease-in-out infinite;
        }}

        @keyframes glow {{
            0%, 100% {{ opacity: 1; }}
            50% {{ opacity: 0.7; }}
        }}

        .glowing {{
            animation: glow 2s ease-in-out infinite;
        }}
    </style>
</head>""")
    
    # Body with theme class
    html_parts.append(f"""<body class="{prefs['theme']}-theme">
    <div class="cyber-grid"></div>
    <div class="container">
        <!-- Futuristic Header -->
        <div class="header">
            <div class="header-content">
                <div class="logo">
                    <div class="logo-icon">🚀</div>
                    <div class="logo-text">
                        <h1>NEXUS TASK MANAGER</h1>
                        <p>v2.0 - Quantum Interface</p>
                    </div>
                </div>
                <div class="efficiency-display">
                    <div class="efficiency-level">{efficiency_level} EFFICIENCY</div>
                    <div class="efficiency-score">{productivity_score}%</div>
                </div>
            </div>
        </div>

        <!-- Stats Grid -->
        <div class="stats-grid">
            <div class="stat-card">
                <div class="stat-value">{total_tasks}</div>
                <div class="stat-label">Active Missions</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{pending_tasks}</div>
                <div class="stat-label">Pending Tasks</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{completed_tasks}</div>
                <div class="stat-label">Completed</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{completion_rate}%</div>
                <div class="stat-label">Success Rate</div>
            </div>
        </div>""")
    
    # Content area
    html_parts.append(f"""        
        <div class="content">
            <!-- Enhanced Sidebar -->
            <div class="sidebar {'collapsed' if prefs['sidebar_collapsed'] else ''}" id="sidebar">
                <div class="sidebar-header">
                    <div class="sidebar-title">Control Panel</div>
                    <form method="POST" class="toggle-sidebar-form">
                        <input type="hidden" name="toggle_sidebar" value="1">
                        <button type="submit" class="toggle-sidebar" title="Toggle Sidebar">
                            {'→' if prefs['sidebar_collapsed'] else '←'}
                        </button>
                    </form>
                </div>
                <div class="sidebar-content">
                    <div class="preference-section">
                        <h3>Interface</h3>
                        <div class="preference-item">
                            <span class="preference-label">Theme</span>
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_theme" value="1">
                                <select name="theme" onchange="this.form.submit()" class="cyber-select">
                                    <option value="cyber" {'selected' if prefs['theme'] == 'cyber' else ''}>Cyber</option>
                                    <option value="light" {'selected' if prefs['theme'] == 'light' else ''}>Light</option>
                                    <option value="matrix" {'selected' if prefs['theme'] == 'matrix' else ''}>Matrix</option>
                                </select>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-section">
                        <h3>Display</h3>
                        <div class="preference-item">
                            <span class="preference-label">Tasks per view</span>
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_tasks_per_page" value="1">
                                <select name="tasks_per_page" onchange="this.form.submit()" class="cyber-select">
                                    <option value="5" {'selected' if prefs['tasks_per_page'] == 5 else ''}>5</option>
                                    <option value="10" {'selected' if prefs['tasks_per_page'] == 10 else ''}>10</option>
                                    <option value="15" {'selected' if prefs['tasks_per_page'] == 15 else ''}>15</option>
                                    <option value="25" {'selected' if prefs['tasks_per_page'] == 25 else ''}>25</option>
                                    <option value="50" {'selected' if prefs['tasks_per_page'] == 50 else ''}>50</option>
                                </select>
                            </form>
                        </div>
                        
                        <div class="preference-item">
                            <span class="preference-label">Sort by</span>
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="update_sort_preference" value="1">
                                <select name="sort_preference" onchange="this.form.submit()" class="cyber-select">
                                    <option value="created_desc" {'selected' if prefs['sort_preference'] == 'created_desc' else ''}>Newest</option>
                                    <option value="created_asc" {'selected' if prefs['sort_preference'] == 'created_asc' else ''}>Oldest</option>
                                    <option value="priority" {'selected' if prefs['sort_preference'] == 'priority' else ''}>Priority</option>
                                    <option value="name" {'selected' if prefs['sort_preference'] == 'name' else ''}>Name</option>
                                    <option value="completed" {'selected' if prefs['sort_preference'] == 'completed' else ''}>Status</option>
                                </select>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-section">
                        <h3>Features</h3>
                        <div class="preference-item">
                            <span class="preference-label">Show Completed</span>
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="toggle_completed_visible" value="1">
                                <label class="cyber-toggle">
                                    <input type="checkbox" {'checked' if prefs['completed_visible'] else ''} onchange="this.form.submit()">
                                    <span class="cyber-slider"></span>
                                </label>
                            </form>
                        </div>
                        
                        <div class="preference-item">
                            <span class="preference-label">Auto Save</span>
                            <form method="POST" style="display: inline;">
                                <input type="hidden" name="toggle_auto_save" value="1">
                                <label class="cyber-toggle">
                                    <input type="checkbox" {'checked' if prefs['auto_save'] else ''} onchange="this.form.submit()">
                                    <span class="cyber-slider"></span>
                                </label>
                            </form>
                        </div>
                    </div>
                    
                    <div class="preference-section">
                        <h3>System Info</h3>
                        <p style="color: var(--text-secondary); font-size: 0.8rem; margin-bottom: 0.5rem;">First access: {first_visit_date}</p>
                        <p style="color: var(--text-secondary); font-size: 0.8rem;">Session: {session_id or "No session"}</p>
                    </div>
                </div>
            </div>
            
            <!-- Main Content -->
            <div class="main-content">
                <!-- Quick Actions -->
                <div class="quick-actions">
                    <div class="action-card" onclick="document.getElementById('taskInput').focus()">
                        <div class="action-icon">⚡</div>
                        <h4>Quick Launch</h4>
                        <p>Add new task</p>
                    </div>
                    <form method="POST" class="action-card" onclick="this.submit()" style="cursor: pointer;">
                        <input type="hidden" name="action" value="clear_completed">
                        <div class="action-icon">✅</div>
                        <h4>Clean Up</h4>
                        <p>Clear completed</p>
                    </form>
                    <div class="action-card" onclick="showSessionInfo()">
                        <div class="action-icon">🔍</div>
                        <h4>System Scan</h4>
                        <p>Debug info</p>
                    </div>
                    <div class="action-card" onclick="location.reload()">
                        <div class="action-icon">🔄</div>
                        <h4>Refresh</h4>
                        <p>Reload interface</p>
                    </div>
                </div>

                <!-- Add Task Form -->
                <form method="POST" class="add-task-form" id="taskForm">
                    <input type="hidden" name="action" value="add">
                    <input type="text" name="task_text" class="task-input" 
                           placeholder="Enter new mission parameters..." required id="taskInput"
                           value="">
                    <select name="category" class="category-select">
                        <option value="personal" {'selected' if prefs['default_category'] == 'personal' else ''}>👤 Personal</option>
                        <option value="work" {'selected' if prefs['default_category'] == 'work' else ''}>💼 Work</option>
                        <option value="shopping" {'selected' if prefs['default_category'] == 'shopping' else ''}>🛒 Shopping</option>
                        <option value="health" {'selected' if prefs['default_category'] == 'health' else ''}>🏥 Health</option>
                        <option value="urgent" {'selected' if prefs['default_category'] == 'urgent' else ''}>🚨 Urgent</option>
                        <option value="ideas" {'selected' if prefs['default_category'] == 'ideas' else ''}>💡 Ideas</option>
                        <option value="learning" {'selected' if prefs['default_category'] == 'learning' else ''}>📚 Learning</option>
                        <option value="travel" {'selected' if prefs['default_category'] == 'travel' else ''}>✈️ Travel</option>
                        <option value="finance" {'selected' if prefs['default_category'] == 'finance' else ''}>💰 Finance</option>
                        <option value="fitness" {'selected' if prefs['default_category'] == 'fitness' else ''}>💪 Fitness</option>
                    </select>
                    <button type="submit" class="btn glow">🚀 Launch Task</button>
                </form>
                
                <!-- Control Buttons -->
                <div class="control-buttons">
                    <form method="POST">
                        <input type="hidden" name="action" value="clear_completed">
                        <button type="submit" class="btn btn-warning">🗑️ Clear Completed</button>
                    </form>
                    <form method="POST">
                        <input type="hidden" name="action" value="clear_all">
                        <button type="submit" class="btn btn-danger" onclick="return confirm('WARNING: This will purge ALL tasks. Continue?')">💣 Purge All</button>
                    </form>
                    <a href="/cgi-bin/session_debug.py" class="btn btn-secondary">🐛 Debug Console</a>
                </div>
                
                <!-- Task List -->
                <div class="task-list">
                    {f'<div class="task-section-title">Active Missions ({total_tasks})</div>' if tasks else ''}
                    {task_html}
                </div>
                
                <!-- Debug Info -->
                <div style="margin-top: 2rem; padding: 1.5rem; background: var(--card-bg); border: 1px solid var(--card-border); border-radius: 12px; display: none;" id="debug-info">
                    <h3 style="margin-bottom: 1rem; color: var(--text-accent); font-family: \\'Orbitron\\', monospace;">System Diagnostics</h3>
                    <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1rem;">
                        <div><strong>Session ID:</strong> {session_id}</div>
                        <div><strong>Data File:</strong> /tmp/tasks_{session_id}.json</div>
                        <div><strong>Total Tasks:</strong> {total_tasks}</div>
                        <div><strong>Request Method:</strong> {method}</div>
                    </div>
                    <button onclick="hideSessionInfo()" class="btn" style="margin-top: 1rem; background: #6b7280;">Close Diagnostics</button>
                </div>
            </div>
        </div>
    </div>""")
    
    # JavaScript
    html_parts.append("""
    <script>
        function showSessionInfo() {
            document.getElementById('debug-info').style.display = 'block';
        }
        
        function hideSessionInfo() {
            document.getElementById('debug-info').style.display = 'none';
        }
        
        // Auto-focus on task input
        const taskInput = document.getElementById('taskInput');
        if (taskInput) {
            taskInput.focus();
            
            // Clear input after form submission
            document.getElementById('taskForm').addEventListener('submit', function() {
                setTimeout(() => {
                    taskInput.value = '';
                }, 100);
            });
        }

        // Add keyboard shortcuts
        document.addEventListener('keydown', function(e) {
            if (e.ctrlKey || e.metaKey) {
                switch(e.key) {
                    case 'k':
                        e.preventDefault();
                        if (taskInput) taskInput.focus();
                        break;
                    case 'n':
                        e.preventDefault();
                        const addButton = document.querySelector('.add-task-form button');
                        if (addButton) addButton.click();
                        break;
                }
            }
            
            // Quick shortcuts without modifier
            if (e.key === '/' && !e.ctrlKey && !e.metaKey) {
                e.preventDefault();
                if (taskInput) taskInput.focus();
            }
        });

        // Add smooth animations for task items
        document.addEventListener('DOMContentLoaded', function() {
            const taskItems = document.querySelectorAll('.task-item');
            taskItems.forEach((item, index) => {
                item.style.animationDelay = (index * 0.05) + 's';
            });
            
            // Add floating animation to some elements
            const floatingElements = document.querySelectorAll('.logo-icon, .action-icon');
            floatingElements.forEach((el, index) => {
                el.style.animationDelay = (index * 0.2) + 's';
                el.classList.add('floating');
            });
        });

        // Enhanced sidebar toggle with smooth animation
        document.addEventListener('DOMContentLoaded', function() {
            const sidebar = document.getElementById('sidebar');
            const content = document.querySelector('.content');
            
            // Check initial state
            if (sidebar.classList.contains('collapsed')) {
                content.style.gridTemplateColumns = '80px 1fr';
            }
        });
    </script>
</body>
</html>""")
    
    # Print the complete HTML
    print(''.join(html_parts))

if __name__ == "__main__":
    main()