@echo off
rem : TeXcount.bat: Windows batch script to run TeXcount
rem :
rem : Script will run TeXcount, write output to a temporary
rem : html file, and display this in the default web browser
rem : (provided html files are associated with this).

rem : Define tc to be the command to run TeXcount
rem : - If installed e.g. with MikTeX
rem : set tc=texcount
rem : - If Perl script texcount.pl is in PATH
set tc=texcount.pl
rem : - If not in PATH, give full path and file name, e.g.
rem set tc="C:/Program Files/TeXcount/texcount.pl"

rem : Set temporary file (%temp%=temporary file directory)
set tempdoc=%temp%\texcount_details.html

rem : Run TeXcount with -html and other desired options
%tc% -sub -inc -v -html -dir dissertation.tex > %tempdoc%

rem : Open temporary HTML document
start %tempdoc%

rem : Pause if you want to see output (e.g. error messages)
rem pause

