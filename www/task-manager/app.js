// Global state
let currentTasks = [];
let currentTheme = 'light';
let currentSession = null;

// DOM Elements
const taskList = document.getElementById('task-list');
const taskInput = document.getElementById('task-input');
const sessionStatus = document.getElementById('session-status');
const sessionModal = document.getElementById('session-modal');
const sessionDebug = document.getElementById('session-debug');

// Initialize app
document.addEventListener('DOMContentLoaded', function() {
    loadTasks();
    setupEventListeners();
    startAutoRefresh();
});

// Event Listeners
function setupEventListeners() {
    // Enter key to add task
    taskInput.addEventListener('keypress', function(e) {
        if (e.key === 'Enter') {
            addTask();
        }
    });
    
    // Close modal when clicking outside
    window.addEventListener('click', function(e) {
        if (e.target === sessionModal) {
            closeSessionInfo();
        }
    });
}

// API Functions - Using query parameters for CGI
async function apiCall(action, method = 'GET', data = null) {
    // Use query parameters for CGI - server handles session via SESSION_ID cookie
    const baseUrl = '/cgi-bin/api_tasks.py';
    const url = `${baseUrl}?action=${action}`;
    
    const options = {
        method: method,
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        credentials: 'include' // Important: include cookies for session
    };
    
    if (data && method === 'POST') {
        const formData = new URLSearchParams();
        for (const key in data) {
            formData.append(key, data[key]);
        }
        options.body = formData;
    }
    
    try {
        const response = await fetch(url, options);
        return await response.json();
    } catch (error) {
        console.error('API call failed:', error);
        showNotification('Connection error. Please try again.', 'error');
        return { success: false, error: 'Network error' };
    }
}

// Task Management
async function loadTasks() {
    const result = await apiCall('get_tasks', 'POST');
    
    if (result.success) {
        currentTasks = result.tasks || [];
        currentTheme = result.theme || 'light';
        currentSession = result.session_id;
        
        updateUI();
        updateSessionStatus();
        applyTheme(currentTheme);
        
        console.log('Loaded tasks:', currentTasks.length, 'Session:', currentSession);
    } else {
        showNotification('Failed to load tasks: ' + (result.error || 'Unknown error'), 'error');
    }
}

async function addTask() {
    const text = taskInput.value.trim();
    const category = document.getElementById('task-category').value;
    
    if (!text) {
        showNotification('Please enter a task', 'warning');
        return;
    }
    
    const result = await apiCall('add_task', 'POST', {
        text: text,
        category: category
    });
    
    if (result.success) {
        taskInput.value = '';
        showNotification('Task added successfully!', 'success');
        await loadTasks(); // Reload to get updated list
    } else {
        showNotification('Failed to add task: ' + (result.error || 'Unknown error'), 'error');
    }
}

async function toggleTask(taskId, completed) {
    const result = await apiCall('update_task', 'POST', {
        id: taskId,
        completed: completed
    });
    
    if (result.success) {
        await loadTasks();
    } else {
        showNotification('Failed to update task', 'error');
    }
}

async function deleteTask(taskId) {
    if (!confirm('Are you sure you want to delete this task?')) {
        return;
    }
    
    const result = await apiCall('delete_task', 'POST', {
        id: taskId
    });
    
    if (result.success) {
        showNotification('Task deleted', 'success');
        await loadTasks();
    } else {
        showNotification('Failed to delete task', 'error');
    }
}

async function clearCompleted() {
    const completedCount = currentTasks.filter(t => t.completed).length;
    
    if (completedCount === 0) {
        showNotification('No completed tasks to clear', 'info');
        return;
    }
    
    if (!confirm(`Clear ${completedCount} completed task(s)?`)) {
        return;
    }
    
    const result = await apiCall('clear_completed', 'POST');
    
    if (result.success) {
        showNotification(result.message, 'success');
        await loadTasks();
    } else {
        showNotification('Failed to clear tasks', 'error');
    }
}

// Theme Management - Server-side only
async function setTheme(theme) {
    const result = await apiCall('update_preferences', 'POST', {
        theme: theme
    });
    
    if (result.success) {
        currentTheme = theme;
        applyTheme(theme);
        showNotification('Theme updated', 'success');
    } else {
        showNotification('Failed to update theme', 'error');
    }
}

function applyTheme(theme) {
    document.body.className = theme + '-theme';
    
    // Update radio buttons
    const radios = document.querySelectorAll('input[name="theme"]');
    radios.forEach(radio => {
        radio.checked = radio.value === theme;
    });
}

// No client-side cookie reading - everything comes from server

// UI Updates
function updateUI() {
    if (currentTasks.length === 0) {
        taskList.innerHTML = `
            <div class="empty-state">
                <h3>No tasks yet</h3>
                <p>Add your first task above to get started!</p>
            </div>
        `;
        return;
    }
    
    const tasksHtml = currentTasks.map(task => `
        <div class="task-item ${task.completed ? 'completed' : ''}">
            <input 
                type="checkbox" 
                class="task-checkbox" 
                ${task.completed ? 'checked' : ''}
                onchange="toggleTask(${task.id}, this.checked)"
            >
            <span class="task-text">${escapeHtml(task.text)}</span>
            <span class="task-category category-${task.category}">
                ${getCategoryEmoji(task.category)}
            </span>
            <div class="task-actions">
                <button class="task-action" onclick="deleteTask(${task.id})" title="Delete">
                    🗑️
                </button>
            </div>
        </div>
    `).join('');
    
    taskList.innerHTML = tasksHtml;
}

function updateSessionStatus() {
    if (currentSession) {
        const taskCount = currentTasks.length;
        const completedCount = currentTasks.filter(t => t.completed).length;
        
        sessionStatus.innerHTML = `
            👤 Server Session: ${currentSession.substring(0, 8)}... | 
            Tasks: ${completedCount}/${taskCount} |
            Theme: ${currentTheme}
        `;
    } else {
        sessionStatus.innerHTML = '👤 Creating server session...';
    }
}

// Session Debug - Shows server-side session info
async function showSessionInfo() {
    try {
        const response = await fetch('/cgi-bin/api_session.py');
        const data = await response.json();
        
        sessionDebug.innerHTML = `
            <h3>Server Session Debug Info:</h3>
            <pre>${JSON.stringify(data, null, 2)}</pre>
            
            <h3>How This Works:</h3>
            <ul>
                <li><strong>Server-Side Sessions:</strong> Your C++ server generates and manages session IDs</li>
                <li><strong>Automatic Session Creation:</strong> New session created on first visit</li>
                <li><strong>Session Persistence:</strong> Session data stored server-side in files</li>
                <li><strong>Cookie-Based Identification:</strong> Browser stores SESSION_ID cookie, server handles the rest</li>
                <li><strong>State Management:</strong> All tasks and preferences stored in server session</li>
            </ul>
            
            <div class="demo-instructions">
                <h4>Demo This:</h4>
                <ol>
                    <li>Add some tasks</li>
                    <li>Close and reopen browser - tasks still there!</li>
                    <li>Open in new tab - same session, same tasks!</li>
                    <li>Switch themes - preference stored server-side</li>
                </ol>
            </div>
        `;
        
        sessionModal.style.display = 'block';
    } catch (error) {
        sessionDebug.innerHTML = `<p>Error loading session info: ${error.message}</p>`;
        sessionModal.style.display = 'block';
    }
}

function closeSessionInfo() {
    sessionModal.style.display = 'none';
}

// Export Functionality
function exportTasks() {
    const exportData = {
        exported_at: new Date().toISOString(),
        session_id: currentSession,
        task_count: currentTasks.length,
        tasks: currentTasks,
        theme: currentTheme
    };
    
    const blob = new Blob([JSON.stringify(exportData, null, 2)], {
        type: 'application/json'
    });
    
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `tasks-${currentSession ? currentSession.substring(0, 8) : 'session'}.json`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    
    showNotification('Tasks exported successfully!', 'success');
}

// Auto-refresh for "real-time" updates - shows session persistence
function startAutoRefresh() {
    setInterval(async () => {
        await loadTasks();
    }, 3000); // Refresh every 3 seconds to show real-time session
}

// Utility Functions
function escapeHtml(unsafe) {
    return unsafe
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}

function getCategoryEmoji(category) {
    const emojis = {
        personal: '👤',
        work: '💼', 
        shopping: '🛒',
        health: '🏥'
    };
    return emojis[category] || '📝';
}

function showNotification(message, type = 'info') {
    // Simple notification system
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.textContent = message;
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        padding: 1rem;
        border-radius: 4px;
        color: white;
        z-index: 1000;
        animation: slideIn 0.3s ease;
    `;
    
    if (type === 'success') notification.style.background = '#28a745';
    else if (type === 'error') notification.style.background = '#dc3545';
    else if (type === 'warning') notification.style.background = '#ffc107';
    else notification.style.background = '#17a2b8';
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.remove();
    }, 3000);
}

// Add CSS for notifications and demo instructions
const style = document.createElement('style');
style.textContent = `
    @keyframes slideIn {
        from { transform: translateX(100%); opacity: 0; }
        to { transform: translateX(0); opacity: 1; }
    }
    
    .demo-instructions {
        background: #e3f2fd;
        padding: 1rem;
        border-radius: 8px;
        margin-top: 1rem;
    }
    
    .demo-instructions ol {
        margin-left: 1.5rem;
    }
    
    .demo-instructions li {
        margin-bottom: 0.5rem;
    }
`;
document.head.appendChild(style);