@echo off
rem Make all the SamaruX external commands
rem Usage: make_all_ext
call make_ext banner
call make_ext cal
call make_ext robots
call make_ext strings
echo.
echo -------------
echo Build results
echo -------------
echo.
call make_tst banner.x
call make_tst cal.x
call make_tst robots.x
call make_tst strings.x
echo.