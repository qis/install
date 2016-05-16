@echo off
rem Enter the script directory.
pushd %~dp0

rem Determine the project name.
for /f "tokens=2 delims=( " %%i in ('findstr /c:"project(" CMakeLists.txt') do (
  set project=%%i
)

rem Copy the executable to the user startup directory.
copy "bin\%project%.exe" "%AppData%\Microsoft\Windows\Start Menu\Programs\Startup\%project%.exe"

rem Start the application.
start "" "%AppData%\Microsoft\Windows\Start Menu\Programs\Startup\%project%.exe"

rem Leave the script directory.
popd
