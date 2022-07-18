@ECHO OFF
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
SET me=%~n0
SET parent=%~dp0
SET target=arduino:samd:nano_33_iot
arduino-cli config init
arduino-cli core update-index
arduino-cli core install arduino:samd
IF %ERRORLEVEL% NEQ 0 (
  REM Arduino SAMD Core has not been installed properly.
)

arduino-cli lib update-index
arduino-cli lib install FastLED WiFiNINA_Generic WiFiManager_NINA_Lite ArduinoJson
IF %ERRORLEVEL% NEQ 0 (
  REM Surf Checker dependencies has not been installed properly.
)

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

SET path_to_example=%USERPROFILE%\Documents\Arduino\libraries\IoT-Surf-Checker\examples\wifi_manager

ECHO Starting compilation...
arduino-cli compile --fqbn %target% %path_to_example%
ECHO %ERRORLEVEL% 
IF %ERRORLEVEL% NEQ 0 (
  ECHO Compilation has failed.
  pause
  EXIT
)
ECHO Starting upload...
arduino-cli upload -p %nano_com_port% --fqbn %target% %path_to_example%
ECHO %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 (
  ECHO Upload has failed.
)

pause