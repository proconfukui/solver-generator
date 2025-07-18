@echo off
chcp 65001 > nul

echo --- Cleaning up old executables ---
del "C:\Users\kyons\プロコン\Solver\solver-generator\generator.exe" "C:\Users\kyons\プロコン\Solver\solver-generator\solver.exe" 2>nul

echo --- Compiling generator.cpp ---
g++ "C:\Users\kyons\プロコン\Solver\solver-generator\generator.cpp" -o "C:\Users\kyons\プロコン\Solver\solver-generator\generator.exe" -finput-charset=UTF-8 -fexec-charset=UTF-8 -Wall -Wextra
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Failed to compile generator.cpp
    goto :eof
)

echo --- Compiling solver.cpp ---
g++ "C:\Users\kyons\プロコン\Solver\solver-generator\solver.cpp" -o "C:\Users\kyons\プロコン\Solver\solver-generator\solver.exe" -finput-charset=UTF-8 -fexec-charset=UTF-8 -Wall -Wextra
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Failed to compile solver.cpp
    goto :eof
)

echo.
echo --- Running generator.exe ---
"C:\Users\kyons\プロコン\Solver\solver-generator\generator.exe"
if %errorlevel% neq 0 (
    echo.
    echo ERROR: generator.exe exited with an error.
    goto :eof
)

echo.
echo --- Running solver.exe ---
"C:\Users\kyons\プロコン\Solver\solver-generator\solver.exe"
if %errorlevel% neq 0 (
    echo.
    echo ERROR: solver.exe exited with an error.
    goto :eof
)

echo.
echo --- Script finished ---
pause