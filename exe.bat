@echo off
rem コンソールの文字コードをUTF-8に設定
chcp 65001 > nul

rem --- 注意！ ---
rem 以下の YOUR_USERNAME の部分は、ご自身のWindowsユーザー名に書き換えてください。
rem 例: "C:\Users\Taro\procon\..."
set "BASE_PATH=C:\Users\kyons\procon\Solver\solver-generator"


echo --- Cleaning up old executables ---
del "%BASE_PATH%\generator.exe" "%BASE_PATH%\solver.exe" 2>nul

echo --- Compiling generator.cpp ---
g++ "%BASE_PATH%\generator.cpp" -o "%BASE_PATH%\generator.exe" -finput-charset=UTF-8 -fexec-charset=UTF-8 -Wall -Wextra
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Failed to compile generator.cpp
    goto :eof
)

echo --- Compiling solver.cpp ---
g++ "%BASE_PATH%\solver.cpp" -o "%BASE_PATH%\solver.exe" -finput-charset=UTF-8 -fexec-charset=UTF-8 -Wall -Wextra
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Failed to compile solver.cpp
    goto :eof
)

echo.
echo --- Running generator.exe ---
"%BASE_PATH%\generator.exe"
if %errorlevel% neq 0 (
    echo.
    echo ERROR: generator.exe exited with an error.
    goto :eof
)

echo.
echo --- Running solver.exe ---
"%BASE_PATH%\solver.exe"
if %errorlevel% neq 0 (
    echo.
    echo ERROR: solver.exe exited with an error.
    goto :eof
)

echo.
echo --- Script finished ---
pause