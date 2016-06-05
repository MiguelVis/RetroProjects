@echo off
rem Test if a file was built
rem Usage: make_tst file
rem IE:    make_tst banner.x
if exist %1 goto OK
echo Build ERROR! : %1
goto QUIT
:OK
echo Build ok     : %1
:QUIT