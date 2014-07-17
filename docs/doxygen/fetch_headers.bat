@echo off

cd ..\..

for /f "tokens=*" %%a in (docs\doxygen\DoxygenFileList.txt) do (
	(echo %%a | find /I ".h" >NULL) && (
		echo %%a
		For /F %%B in ('dir /S /B %%a') do copy /Y %%B docs\doxygen\%%a
	)
)
