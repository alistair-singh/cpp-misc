
@echo off
pushd wwatch
msbuild /nologo /p:configuration=release /v:quiet
popd
wwatch\X64\Release\wwatch %*

