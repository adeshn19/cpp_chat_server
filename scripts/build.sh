#!/bin/bash

echo "🔨 Building project..."

# Move to project root (IMPORTANT)
cd "$(dirname "$0")/.." || exit

# Build server
echo "📦 Building server..."
cd server || exit
make clean && make
if [ $? -ne 0 ]; then
    echo "❌ Server build failed"
    exit 1
fi
cd ..

# Build client
echo "📦 Building client..."
cd client || exit
make clean && make
if [ $? -ne 0 ]; then
    echo "❌ Client build failed"
    exit 1
fi
cd ..

echo "Build successful!"