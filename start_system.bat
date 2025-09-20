@echo off
echo 🚀 Starting MARS-SENTINEL System...
echo.
echo 📋 Quick Setup Checklist:
echo    1. Arduino connected to COM4 (or update COM port in app.py)
echo    2. Firmware uploaded to Arduino
echo    3. All sensors wired according to README.md
echo.
echo 🌐 Dashboard will open at: http://localhost:5000
echo.
echo Press any key to start the system...
pause >nul

REM Start the Python server
C:\Users\solom\miniprojets\MakeSense\.venv\Scripts\python.exe app.py
