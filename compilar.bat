@echo off
setlocal

:: Verificar que se pase un archivo como argumento
if "%~1"=="" (
    echo Uso: compilar.bat archivo.c
    exit /b 1
)

:: Llamar a cl con el argumento y las librerías
cl %1.c FSUIPC_User.lib paho-mqtt3c.lib /link User32.lib /NODEFAULTLIB:LIBC.lib

%1.exe

endlocal
