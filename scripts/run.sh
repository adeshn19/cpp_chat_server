#!/bin/bash

echo "🚀 Starting Chat System..."

# Move to project root (IMPORTANT FIX)
cd "$(dirname "$0")/.." || exit

# Start server in background
echo "🖥️ Starting server..."
cd server || exit
./server_exe &
SERVER_PID=$!
cd ..

sleep 1

# Start client
echo "💻 Starting client..."
cd client || exit
./client_exe

# Cleanup when client exits
echo "🧹 Stopping server..."
kill $SERVER_PID