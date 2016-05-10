@echo off

set NUGET=.nuget\nuget.exe
set VSTOOLSDIR=packages\VisualCppTools

if not exist %NUGET% (powershell -Command "wget http://nuget.org/nuget.exe -OutFile .nuget\nuget.exe")

::if not exist %VSTOOLSDIR%* ( 

%NUGET% install VisualCppTools -SolutionDirectory . -ConfigFile .nuget\nuget.config -NonInteractive -PreRelease -OutputDirectory packages 

::)

for /D %%i in ( %VSTOOLSDIR%* ) do ( set NVCHOME=%~dp0%%i)

echo %NVCHOME%

set INCLUDE=%NVCHOME%\lib\native\include;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\ATLMFC\INCLUDE;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\ucrt;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6.1\include\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\shared;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.10586.0\winrt;
set LIB=%NVCHOME%\lib\native\lib\amd64;C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\ATLMFC\LIB\amd64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.10586.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6.1\lib\um\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.10586.0\um\x64;
set PATH=%NVCHOME%\lib\native\bin\amd64;%PATH%

set NUGET=
set VSTOOLSDIR=
set NVCHOME=
