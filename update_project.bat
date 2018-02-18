@echo off
rem # update_project.bat
rem #
rem # Update a MESCC project with fresh binaries and libraries.
rem #
rem # Created: 06 Jan 2016
rem # Updated: 06 Jan 2016
rem # Updated: 18 Feb 2018
rem #
rem # Check command line arguments
rem #
if "%1"=="" goto usage
if not "%2"=="" goto usage
rem #
rem # Check project name
rem #
if not exist "%1" goto err_not_exist
rem #
rem # Copy binaries
rem #
echo Copying binaries...
copy /Y mescc\cpm.exe %1\
copy /Y mescc\cpm_player.exe %1\
copy /Y mescc\cc.com %1\
copy /Y mescc\ccopt.com %1\
copy /Y mescc\zsm.com %1\
copy /Y mescc\hextocom.com %1\
rem #
rem # Copy libraries
rem #
echo Copying libraries...
copy /Y mescc\alloc.h %1\
copy /Y mescc\atexit.h %1\
copy /Y mescc\bsearch.h %1\
copy /Y mescc\clock.h %1\
copy /Y mescc\conio.h %1\
copy /Y mescc\cpm.h %1\
copy /Y mescc\ctype.h %1\
copy /Y mescc\fileio.h %1\
copy /Y mescc\fprintf.h %1\
copy /Y mescc\mem.h %1\
copy /Y mescc\mescc.h %1\
copy /Y mescc\printf.h %1\
copy /Y mescc\qsort.h %1\
copy /Y mescc\rand.h %1\
copy /Y mescc\redir.h %1\
copy /Y mescc\setjmp.h %1\
copy /Y mescc\sprintf.h %1\
copy /Y mescc\stdbool.h %1\
copy /Y mescc\string.h %1\
copy /Y mescc\xprintf.h %1\
copy /Y mescc\z80.h %1\
rem #
rem # Done
rem #
goto end
rem #
rem # Usage
rem #
:usage
echo Usage: update_project project_name
goto end
rem #
rem # Error
rem #
:err_not_exist
echo Error: Project does not exists!
goto end
rem #
rem # End
rem #
:end