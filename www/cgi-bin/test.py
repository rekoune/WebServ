#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/plain\r\n")

print("=== CGI Environment Variables ===")
for key in sorted(os.environ.keys()):
    if key.startswith("HTTP_") or key in (
        "REQUEST_METHOD", "QUERY_STRING", "CONTENT_LENGTH", "CONTENT_TYPE",
        "SCRIPT_NAME", "SERVER_PROTOCOL", "SERVER_SOFTWARE", "GATEWAY_INTERFACE"
    ):
        print(f"{key} = {os.environ[key]}")

print("\n=== CGI Input Body ===")
content_length = os.environ.get("CONTENT_LENGTH")
if content_length:
    try:
        length = int(content_length)
        body = sys.stdin.read(length)
        print(body)
    except Exception as e:
        print(f"[Error reading body: {e}]")
else:
    print("(No body)")

while True :
    print("")

