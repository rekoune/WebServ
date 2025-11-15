#!/usr/bin/env python3
import os
import json
import cgi
import cgitb
from datetime import datetime

# Enable error reporting
cgitb.enable()

def get_session_id():
    """Get session ID from cookie - set by C++ server"""
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    session_id = None
    
    if cookie_header:
        for cookie in cookie_header.split(';'):
            cookie = cookie.strip()
            if cookie.startswith('SESSION_ID='):
                session_id = cookie.split('=', 1)[1]
                break
    
    return session_id

def load_tasks(session_id):
    """Load tasks from session file"""
    if not session_id:
        return []
    
    task_file = f"/tmp/tasks_{session_id}.json"
    try:
        with open(task_file, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
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
    except Exception as e:
        print(f"Error saving tasks: {e}", file=sys.stderr)
        return False

def handle_post_request(session_id):
    """Handle POST requests to add/update/delete tasks"""
    form = cgi.FieldStorage()
    action = form.getvalue('action', '')
    
    tasks = load_tasks(session_id)
    
    if action == 'add' and form.getvalue('task_text'):
        # Add new task
        new_task = {
            'id': len(tasks) + 1,
            'text': form.getvalue('task_text'),
            'completed': False,
            'created_at': datetime.now().isoformat(),
            'category': form.getvalue('category', 'personal')
        }
        tasks.append(new_task)
        save_tasks(session_id, tasks)
        
    elif action == 'toggle':
        # Toggle task completion
        task_id = int(form.getvalue('task_id'))
        for task in tasks:
            if task['id'] == task_id:
                task['completed'] = not task['completed']
                break
        save_tasks(session_id, tasks)
        
    elif action == 'delete':
        # Delete task
        task_id = int(form.getvalue('task_id'))
        tasks = [t for t in tasks if t['id'] != task_id]
        save_tasks(session_id, tasks)
        
    elif action == 'clear_completed':
        # Clear completed tasks
        tasks = [t for t in tasks if not t['completed']]
        save_tasks(session_id, tasks)
    
    return tasks

def get_category_emoji(category):
    """Get emoji for category"""
    emojis = {
        'personal': '👤',
        'work': '💼', 
        'shopping': '🛒',
        'health': '🏥'
    }
    return emojis.get(category, '📝')

def generate_task_html(tasks):
    """Generate HTML for task list"""
    if not tasks:
        return '''
                <div class="empty-state">
                    <h3>🎉 No tasks yet!</h3>
                    <p>Add your first task using the form above to get started.</p>
                </div>
                '''
    
    task_html = []
    for task in tasks:
        completed_class = 'completed' if task['completed'] else ''
        checked = 'checked' if task['completed'] else ''
        category_emoji = get_category_emoji(task['category'])
        
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
                        {category_emoji}
                    </span>
                    <form method="POST" style="display: inline;">
                        <input type="hidden" name="action" value="delete">
                        <input type="hidden" name="task_id" value="{task['id']}">
                        <button type="submit" class="delete-btn" onclick="return confirm(\'Delete this task?\')">Delete</button>
                    </form>
                </div>
                ''')
    
    return ''.join(task_html)

def main():
    # Get session ID from cookies
    session_id = get_session_id()
    method = os.environ.get('REQUEST_METHOD', 'GET')
    
    # Handle POST requests
    if method == 'POST':
        tasks = handle_post_request(session_id)
    else:
        tasks = load_tasks(session_id)
    
    # Count stats
    total_tasks = len(tasks)
    completed_tasks = len([t for t in tasks if t['completed']])
    pending_tasks = total_tasks - completed_tasks
    
    # Generate task HTML
    task_html = generate_task_html(tasks)
    
    # Output HTML
    print("Content-Type: text/html")
    print()
    
    print(f"""<!DOCTYPE html>
<html lang="en">
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
        }}

        .container {{
            max-width: 800px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            overflow: hidden;
        }}

        .header {{
            background: linear-gradient(45deg, #2c3e50, #3498db);
            color: white;
            padding: 2rem;
            text-align: center;
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

        .content {{
            padding: 2rem;
        }}

        .add-task {{
            display: flex;
            gap: 1rem;
            margin-bottom: 2rem;
            flex-wrap: wrap;
        }}

        .task-input {{
            flex: 1;
            min-width: 200px;
            padding: 1rem;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            font-size: 1rem;
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
        }}

        button {{
            padding: 1rem 1.5rem;
            border: none;
            border-radius: 8px;
            background: #3498db;
            color: white;
            cursor: pointer;
            font-size: 1rem;
            transition: background 0.3s ease;
        }}

        button:hover {{
            background: #2980b9;
        }}

        .task-list {{
            margin-bottom: 2rem;
        }}

        .task-item {{
            display: flex;
            align-items: center;
            gap: 1rem;
            padding: 1rem;
            border: 1px solid #e9ecef;
            border-radius: 8px;
            margin-bottom: 0.5rem;
            transition: all 0.3s ease;
            background: white;
        }}

        .task-item:hover {{
            background: #f8f9fa;
            transform: translateX(5px);
        }}

        .task-item.completed {{
            opacity: 0.6;
            background: #f8f9fa;
        }}

        .task-checkbox {{
            width: 20px;
            height: 20px;
            cursor: pointer;
        }}

        .task-text {{
            flex: 1;
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
        }}

        .category-personal {{ background: #e3f2fd; color: #1976d2; }}
        .category-work {{ background: #f3e5f5; color: #7b1fa2; }}
        .category-shopping {{ background: #e8f5e8; color: #388e3c; }}
        .category-health {{ background: #fff3e0; color: #f57c00; }}

        .task-actions {{
            display: flex;
            gap: 0.5rem;
        }}

        .delete-btn {{
            background: #e74c3c;
            padding: 0.5rem 1rem;
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

        .empty-state {{
            text-align: center;
            padding: 3rem;
            color: #666;
        }}

        .empty-state h3 {{
            margin-bottom: 1rem;
            color: #999;
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
                flex-direction: column;
            }}
            
            .task-input, .category-select {{
                min-width: auto;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>✅ Task Manager</h1>
            <p>Server Session-Based Task Management</p>
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
        
        <div class="content">
            <!-- Add Task Form -->
            <form method="POST" class="add-task">
                <input type="hidden" name="action" value="add">
                <input type="text" name="task_text" class="task-input" 
                       placeholder="What needs to be done?" required>
                <select name="category" class="category-select">
                    <option value="personal">👤 Personal</option>
                    <option value="work">💼 Work</option>
                    <option value="shopping">🛒 Shopping</option>
                    <option value="health">🏥 Health</option>
                </select>
                <button type="submit">Add Task</button>
            </form>
            
            <!-- Controls -->
            <div class="controls">
                <form method="POST">
                    <input type="hidden" name="action" value="clear_completed">
                    <button type="submit">🗑️ Clear Completed</button>
                </form>
                <button onclick="location.reload()">🔄 Refresh</button>
                <button onclick="showSessionInfo()">🔍 Session Info</button>
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
                <button onclick="hideSessionInfo()">Hide Debug</button>
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
        
        // Auto-focus on task input
        document.querySelector('.task-input')?.focus();
        
        // Add some interactivity
        document.addEventListener('DOMContentLoaded', function() {{
            console.log('Task Manager loaded with session:', '{session_id}');
        }});
    </script>
</body>
</html>""")

if __name__ == '__main__':
    main()