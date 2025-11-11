#!/bin/bash

# CGI Bash Script - Processes request body
# Save as /cgi-bin/processor.sh and make executable: chmod +x processor.sh

# Set CGI headers
echo "Content-Type: text/plain"
echo ""

# Log for debugging (optional)
echo "=== CGI Processor Started ===" > /tmp/cgi_debug.log
echo "Method: $REQUEST_METHOD" >> /tmp/cgi_debug.log
echo "Content Length: $CONTENT_LENGTH" >> /tmp/cgi_debug.log

# Check if it's a POST request with body
if [ "$REQUEST_METHOD" = "POST" ]; then
    # Read the request body
    if [ "$CONTENT_LENGTH" -gt 0 ]; then
        echo "Reading $CONTENT_LENGTH bytes from stdin..." >> /tmp/cgi_debug.log
        read -r -n "$CONTENT_LENGTH" body
        
        # Process the body
        echo "=== PROCESSING BODY ==="
        echo "Body length: ${#body} characters"
        echo "Body content:"
        echo "$body"
        echo "=== END BODY ==="
        
        # Log the body (be careful with large bodies)
        echo "Body: $body" >> /tmp/cgi_debug.log
    else
        echo "ERROR: No content body received"
        echo "ERROR: No content body" >> /tmp/cgi_debug.log
    fi
else
    echo "ERROR: This script only accepts POST requests"
    echo "Received method: $REQUEST_METHOD" >> /tmp/cgi_debug.log
fi

echo "=== CGI Processor Finished ===" >> /tmp/cgi_debug.log