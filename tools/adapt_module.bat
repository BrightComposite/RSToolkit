@echo off
setlocal enableextensions
set here=%~dp0

REM Need to clear variables before user input! Otherwise value of the previous session may be used instead of empty input

:enter_name
set module_name=
set /p module_name=Enter module name: 

if "%module_name%" EQU "" (
    echo Module name must be non-empty!
    goto :enter_name
)

:enter_type
set module_type=
set /p module_type=Enter module type (static, shared, inline). Press Enter to use static: 

if "%module_type%" EQU "" (
    set module_type=static
    goto :enter_path
)

if "%module_type%" EQU "static" goto :enter_path
if "%module_type%" EQU "shared" goto :enter_path
if "%module_type%" EQU "inline" goto :enter_path
   
echo Invalid module type!
goto :enter_type

:enter_path
set module_path=
set /p module_path=Enter module path: 

if "%module_path%" EQU "" (
    echo Module path must be non-empty!
    goto :enter_path
)

for /f "delims=" %%i in ("%module_path%") do set inputpath="%%~fi"
pushd %inputpath% 2> nul
if errorlevel 1 (
    echo Module path is not a directory or doesn't exist!
    goto :enter_path
)

echo OK, module path exists

:enter_group
set group=
set /p group=Add source group: 

if "%group%" EQU "" (
    echo Group name must be non-empty!

    if defined groups_list (
        set abort_group=
        set /p abort_group=Press Enter to continue, "a" to abort adding: 

        if "%abort_group%" EQU "a" goto :enter_workspace_path
            
        goto :enter_group
    )

    goto :enter_group
)

:enter_group_directory
set directory=
set /p directory=Path: 

if "%directory%" EQU "" (
    echo Source group path must be non-empty!
    goto :enter_group_directory
)

for /f "delims=" %%i in ("%module_path%") do (
    for /f "delims=" %%j in ("%directory%") do (
        set inputpath="%%~fi/%%~j"
    )
)

pushd %inputpath% 2> nul
if errorlevel 1 (
    echo Source group path is not a directory or doesn't exist!
    goto :enter_group_directory
)

echo OK, source group directory exists

for /f "delims=" %%j in ("%directory%") do (
    if defined groups_list (
        for /f "delims=" %%i in ("%groups_list%") do (
            set groups_list="%%~i;%group%;%%~j"
        )
    ) else (
        set groups_list="%group%;%%~j"
    )
)

set add_group=
set /p add_group=Press Enter to continue, "a" to add a new group: 

if "%add_group%" EQU "a" goto :enter_group

:enter_workspace_path
set workspace_path=
set /p workspace_path=Enter workspace path (leave empty to use default): 

if "%workspace_path%" EQU "" set workspace_path="%here%.."

:enter_destination
set destination=
set /p destination=Enter destination (modules, third-party, projects). Press Enter to use third-party: 

if "%destination%" EQU "" (
    set destination=third-party
    goto :generate
)

if "%destination%" EQU "modules" goto :generate
if "%destination%" EQU "third-party" goto :generate
if "%destination%" EQU "projects" goto :generate
   
echo Invalid destination!
goto :enter_destination

:generate

echo OK...

cmake -D NAME=%module_name% -D TYPE=%module_type% -D PATH=%module_path% -D GROUPS=%groups_list% -D WORKSPACE=%workspace_path% -D DESTINATION=%destination% -P %here%generate_lists.cmake

pause
