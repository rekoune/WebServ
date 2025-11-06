#!/usr/bin/env python3
import sys
import time

# Required CGI header
print("Content-Type: text/plain")
print()  # blank line separates headers from body
sys.stdout.flush()

# Simulate CGI generating output slowly
for i in range(5):
    print(f"Chunk {i + 1} - still running...", flush=True)
    time.sleep(2)

print("Done! CGI finished.", flush=True)
