@echo off

set NUGET_DIR=%~dp0.nuget
set NUGET=%NUGET_DIR%\nuget.exe
set PACKAGEDIR=%~dp0packages\
set VSTOOLSDIR=%PACKAGEDIR%VisualCppTools

if "%1"=="skip" (goto :SkipDownload)

mkdir %NUGET_DIR%
if not exist %NUGET% (powershell -Command "wget http://nuget.org/nuget.exe -OutFile %NUGET%")

::if not exist %VSTOOLSDIR%* ( 

%NUGET% install VisualCppTools -SolutionDirectory . -ConfigFile %~dp0.nuget\nuget.config -NonInteractive -PreRelease -OutputDirectory %PACKAGEDIR% 

::)

:SkipDownload

echo %VSTOOLSDIR%

for /D %%i in ( %VSTOOLSDIR%* ) do ( set NVCHOME=%%i)

echo %NVCHOME%

set INCLUDE=%NVCHOME%\lib\native\include;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\ATLMFC\INCLUDE;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\ucrt;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6.1\include\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\shared;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\winrt;
set LIB=%NVCHOME%\lib\native\lib\amd64;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\ATLMFC\LIB\amd64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.10586.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6.1\lib\um\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.10586.0\um\x64;
set PATH=%NVCHOME%\lib\native\bin\amd64;%PATH%

set NUGET_DIR=
set NUGET=
set VSTOOLSDIR=
set PACKAGEDIR=
set NVCHOME=
