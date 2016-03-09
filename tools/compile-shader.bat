	@echo off
	
	if not exist "%2" (
		if not exist "%~p2" md "%~p2"
		echo. > "%2"
		goto :compile
	)

	Rem for /f "usebackq" %%a in (`cscript //nologo get-time.vbs %1`) do (
	Rem 	echo %%a
	Rem 	set timestamp=%%a
	Rem )
	
	set timestamp=%~t1

	for /f %%f in ("%cd%") do set currentdir=%%~pnf
	cd "%~p2"

	set timestampfile=%~n2.timestamp

	if not exist "%timestampfile%" (
		goto :compile
	)

	for /f "delims=" %%f in (%timestampfile%) do set savedtimestamp=%%f

	if not "%savedtimestamp%" == "%timestamp%" (
		goto :compile
	)

	cd "%currentdir%"

goto :end

:compile
	cd "%currentdir%"
	call fxc /nologo /T%3_4_0 /Zpr /Fh %2 /Vn shader_code_%4 %1
	cd "%~p2"
	echo | set /p timestamp="%timestamp%" >%timestampfile%
	cd "%currentdir%"
	
:end