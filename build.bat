@echo off

ls *.cc *.cpp | xargs -n 1 echo cl.bat | cmd
ls *.asm | xargs -n 1 echo ml64.bat | cmd
