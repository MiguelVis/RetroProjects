@echo off
rem Make a SamaruX external command from a built-in one.
rem Usage: make_ext2 sx_name name
rem IE:    make_ext2 sx_cat cat
call make_ext %1
if exist %2.x del %2.x
ren %1.x %2.x
