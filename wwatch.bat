
@echo off
pushd wwatch\src

"C:\Program Files (x86)\MSBuild\14.0\bin\msbuild.exe" /nologo /p:configuration=release /v:quiet /p:platform=x64

popd
wwatch\src\x64\Release\wwatch.exe %*

