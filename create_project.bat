@echo off
rem # create_project.bat
rem #
rem # Create a MESCC project.
rem #
rem # Created: 06 Jan 2016
rem # Updated: 18 Dec 2018
rem #
rem # Check command line arguments
rem #
if "%1"=="" goto usage
if not "%2"=="" goto usage
rem #
rem # Check project name
rem #
if exist "%1" goto err_exist
rem #
rem # Make project folder
rem #
echo Making project folder...
mkdir "%1"
rem #
rem # Update contents
rem #
call update_project %1
rem #
rem # Making initial files
rem #
echo Making initial files...
copy mescc\template.c %1\%1.c
rem #
rem # Project makefile
rem #
echo cpm cc %1 > %1\make.bat
echo cpm ccopt %1 >> %1\make.bat
echo cpm zsm %1.@@Z >> %1\make.bat
echo cpm hextocom %1 >> %1\make.bat
echo erase %1.zsm >> %1\make.bat
echo erase %1.hex >> %1\make.bat
rem #
rem # Done
rem #
goto end
rem #
rem # Usage
rem #
:usage
echo Usage: create_project project_name
goto end
rem #
rem # Error
rem #
:err_exist
echo Error: Project already exists!
goto end
rem #
rem # End
rem #
:end