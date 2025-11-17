#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Simple CGI test script"""

import sys
import json

# Force unbuffered output
sys.stdout.reconfigure(line_buffering=True)

print("Content-Type: application/json")
print()
print(json.dumps({
    "status": "ok",
    "message": "CGI is working!",
    "python_version": sys.version
}))
sys.stdout.flush()