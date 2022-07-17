@ECHO OFF
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
SET me=%~n0
SET parent=%~dp0
arduino-cli config init
arduino-cli core update-index
arduino-cli core install arduino:samd
IF %ERRORLEVEL% NEQ 0 (
  REM Arduino SAMD Core has not been installed properly.
)
arduino-cli board list
SET count=0
FOR /f "tokens=* USEBACKQ" %%i IN (`arduino-cli board list`) do (
  SET /a count=!count!+1
  SET board_list!count!=%%i
)

IF "%count%"=="1!" (
    ECHO No boards found...
    pause
    EXIT
)
SET count_found=0
FOR /L %%v IN (2, 1, %count%) DO (
  (echo !board_list%%v! | findstr /i /c:"arduino:samd:nano_33_iot" >nul) && (
    ECHO Match!
    SET count_found=%%v
  ) || (
    ECHO No match, keep going...
  )
)

FOR /f "USEBACKQ" %%i IN (`ECHO !board_list%count_found%!`) do (
  SET nano_com_port=%%i
)
ECHO %nano_com_port%

ECHO Starting compilation...
arduino-cli compile --fqbn arduino:samd:nano_33_iot C:\Users\nicol\Documents\Arduino\libraries\IoT-Surf-Checker\examples\wifi_manager
ECHO %ERRORLEVEL% 
IF %ERRORLEVEL% NEQ 0 (
  ECHO Compilation has failed.
)
ECHO Starting upload...
arduino-cli upload -p %nano_com_port% --fqbn arduino:samd:nano_33_iot C:\Users\nicol\Documents\Arduino\libraries\IoT-Surf-Checker\examples\wifi_manager
ECHO %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 (
  ECHO Upload has failed.
)

pause