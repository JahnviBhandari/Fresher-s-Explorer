#!/bin/bash

echo "Compiling Dehradun Explorer Web Edition..."
echo "==========================================="

# Check if required files exist
if [ ! -f "cfiles/authentication.c" ] || [ ! -f "cfiles/authentication.h" ]; then
    echo "Error: authentication files not found in cfiles/"
    exit 1
fi

if [ ! -f "dehradun_data.csv" ]; then
    echo "Warning: dehradun_data.csv not found in current directory"
fi

echo "Compiling auth_manager..."
gcc -o auth_manager auth_manager.c cfiles/authentication.c
if [ $? -eq 0 ]; then
    echo "✓ auth_manager compiled successfully"
else
    echo "✗ auth_manager compilation failed"
    exit 1
fi

echo "Compiling tourist_manager..."
gcc -o tourist_manager tourist_manager.c cfiles/tourist.c cfiles/search.c
if [ $? -eq 0 ]; then
    echo "✓ tourist_manager compiled successfully"
else
    echo "✗ tourist_manager compilation failed"
    exit 1
fi

echo "Compiling graph_manager..."
gcc -o graph_manager graph_manager.c cfiles/graph.c
if [ $? -eq 0 ]; then
    echo "✓ graph_manager compiled successfully"
else
    echo "✗ graph_manager compilation failed"
    exit 1
fi

# Set execute permissions
chmod +x auth_manager tourist_manager graph_manager

echo ""
echo "==========================================="
echo "Compilation completed successfully!"
echo "Next steps:"
echo "1. Run: pip install -r requirements.txt"
echo "2. Run: python app.py"
echo "3. Open: http://localhost:5000"
echo "==========================================="