#!/usr/bin/env python3
import os
import json
import cgi
import cgitb
from datetime import datetime

cgitb.enable()

def get_session_id():
    """Get session ID from cookie"""
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
    except Exception:
        return False

def main():
    session_id = get_session_id()
    method = os.environ.get('REQUEST_METHOD', 'GET')
    
    # Set JSON content type
    print("Content-Type: application/json")
    print()
    
    if not session_id:
        print(json.dumps({
            'success': False,
            'error': 'No session ID found',
            'tasks': []
        }))
        return
    
    if method == 'GET':
        # Return tasks as JSON
        tasks = load_tasks(session_id)
        print(json.dumps({
            'success': True,
            'session_id': session_id,
            'tasks': tasks,
            'total': len(tasks),
            'completed': len([t for t in tasks if t['completed']])
        }))
        
    elif method == 'POST':
        # Handle API actions
        form = cgi.FieldStorage()
        action = form.getvalue('action', '')
        tasks = load_tasks(session_id)
        
        if action == 'add' and form.getvalue('text'):
            new_task = {
                'id': len(tasks) + 1,
                'text': form.getvalue('text'),
                'completed': False,
                'created_at': datetime.now().isoformat(),
                'category': form.getvalue('category', 'personal')
            }
            tasks.append(new_task)
            save_tasks(session_id, tasks)
            
            print(json.dumps({
                'success': True,
                'task': new_task,
                'total_tasks': len(tasks)
            }))
            
        elif action == 'toggle':
            task_id = int(form.getvalue('task_id'))
            for task in tasks:
                if task['id'] == task_id:
                    task['completed'] = not task['completed']
                    break
            save_tasks(session_id, tasks)
            
            print(json.dumps({
                'success': True,
                'task_id': task_id
            }))
            
        elif action == 'delete':
            task_id = int(form.getvalue('task_id'))
            tasks = [t for t in tasks if t['id'] != task_id]
            save_tasks(session_id, tasks)
            
            print(json.dumps({
                'success': True,
                'task_id': task_id,
                'total_tasks': len(tasks)
            }))
            
        else:
            print(json.dumps({
                'success': False,
                'error': 'Invalid action'
            }))

if __name__ == '__main__':
    main()