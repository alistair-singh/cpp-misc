
cl.exe /nologo /MP /FAsc /WX /W4 /EHsc /Ox /Zi /GL %* /link /debug /opt:ref 
::/opt:icf Collapse similar functions

::clang-cl.exe /WX /W4 /EHsc /O2 /Zi -fms-compatibility-version=19 %*

