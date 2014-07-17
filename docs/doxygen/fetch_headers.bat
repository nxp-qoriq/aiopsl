@echo off

echo                   ####
echo                ##########
echo             ##########  ....          Freescale Semiconductor Inc.
echo                ####  ..........       Copyright (c) 2013
echo                   ..........  ####
echo                      ....  ##########
echo                         ##########
echo                     ....   ####
echo                  ..........
echo               ..........  ####
echo           ####   ....  ##########
echo        ##########   ##########
echo     ##########  ....   ####
echo        ####  ..........
echo           ..........
echo       ####   ....
echo    ##########
echo ##########
echo    ####

echo Fetching header files for doxygen:

cd ..\..
for /f "tokens=*" %%a in (docs\doxygen\DoxygenFileList.txt) do (
	(echo %%a | find /I ".h" >NULL) && (
		echo %%a
		del /F /Q docs\doxygen\%%a 2> null
		
		For /F %%B in ('dir /S /B %%a') do (
			copy /Y %%B docs\doxygen\%%a > null
		)
	)
)

pause