@echo off
echo Compiling Dehradun Explorer for Windows...
echo ===========================================

:: Check if required files exist
if not exist "cfiles\authentication.c" (
    echo Error: authentication.c not found in cfiles\
    pause
    exit /b 1
)

if not exist "dehradun_data.csv" (
    echo Warning: dehradun_data.csv not found in current directory
)

echo Compiling auth_manager...
gcc -o auth_manager.exe auth_manager.c cfiles\authentication.c
if %errorlevel% equ 0 (
    echo ✓ auth_manager.exe compiled successfully
) else (
    echo ✗ auth_manager.exe compilation failed
    pause
    exit /b 1
)

echo Compiling tourist_manager...
gcc -o tourist_manager.exe tourist_manager.c cfiles\tourist.c cfiles\search.c
if %errorlevel% equ 0 (
    echo ✓ tourist_manager.exe compiled successfully
) else (
    echo ✗ tourist_manager.exe compilation failed
    pause
    exit /b 1
)

echo Compiling graph_manager...
gcc -o graph_manager.exe graph_manager.c cfiles\graph.c
if %errorlevel% equ 0 (
    echo ✓ graph_manager.exe compiled successfully
) else (
    echo ✗ graph_manager.exe compilation failed
    pause
    exit /b 1
)

echo.
echo ===========================================
echo Compilation completed successfully!
echo Next steps:
echo 1. Run: pip install -r requirements.txt
echo 2. Run: python app.py
echo 3. Open: http://localhost:5000
echo ===========================================
pause