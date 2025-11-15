#!/usr/bin/env python3
import os
import json
import datetime
import hashlib

class SessionManager:
    def __init__(self):
        self.session_data = {}
        self.load_session()
    
    def load_session(self):
        """Load session data from cookies"""
        cookie_header = os.environ.get('HTTP_COOKIE', '')
        
        # Extract session ID
        session_id = None
        if 'SESSION_ID=' in cookie_header:
            for cookie in cookie_header.split(';'):
                cookie = cookie.strip()
                if cookie.startswith('SESSION_ID='):
                    session_id = cookie.split('=', 1)[1]
                    break
        
        if session_id:
            # Try to load session data from file
            session_file = f"/tmp/session_{session_id}.json"
            try:
                with open(session_file, 'r') as f:
                    self.session_data = json.load(f)
                self.session_id = session_id
            except FileNotFoundError:
                self.create_new_session()
        else:
            self.create_new_session()
    
    def create_new_session(self):
        """Create a new session"""
        import random
        import string
        
        self.session_id = ''.join(random.choices(string.ascii_letters + string.digits, k=32))
        self.session_data = {
            'session_id': self.session_id,
            'created_at': datetime.datetime.now().isoformat(),
            'last_activity': datetime.datetime.now().isoformat(),
            'visit_count': 0,
            'user_data': {}
        }
        self.save_session()
    
    def save_session(self):
        """Save session data to file"""
        self.session_data['last_activity'] = datetime.datetime.now().isoformat()
        session_file = f"/tmp/session_{self.session_id}.json"
        
        with open(session_file, 'w') as f:
            json.dump(self.session_data, f, indent=2)
    
    def get(self, key, default=None):
        return self.session_data.get('user_data', {}).get(key, default)
    
    def set(self, key, value):
        if 'user_data' not in self.session_data:
            self.session_data['user_data'] = {}
        self.session_data['user_data'][key] = value
        self.save_session()
    
    def delete(self, key):
        if 'user_data' in self.session_data and key in self.session_data['user_data']:
            del self.session_data['user_data'][key]
            self.save_session()
    
    def increment_visit_count(self):
        count = self.session_data.get('visit_count', 0) + 1
        self.session_data['visit_count'] = count
        self.save_session()
        return count
    
    def get_session_info(self):
        return {
            'session_id': self.session_id,
            'created_at': self.session_data.get('created_at'),
            'last_activity': self.session_data.get('last_activity'),
            'visit_count': self.session_data.get('visit_count', 0),
            'user_data': self.session_data.get('user_data', {})
        }

def get_session_manager():
    return SessionManager()