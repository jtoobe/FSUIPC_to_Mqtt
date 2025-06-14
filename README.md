Programa en "C" que toma datos de FSUIPC 3.999 corriendo en un FS2004 y los publica en HiveMq (mqtt)

La idea es tomar esos datos con ESP32's y manejar todo tipo de salidas y displays

1) bajamos visual studio build tools 2022
2) bajamos paho-mqtt
3) en la carpeta ....\FSUIPC_SDK\SDK\UIPC_SDK_C copiamos los .lib .dll y .h de paho-mqtt
4) compilamos este programa con: cl UIPChello_mqtt.c FSUIPC_User.lib paho-mqtt3c.lib /link User32.lib /NODEFAULTLIB:LIBC.lib
5) ejecutamos y conectandose a: https://www.hivemq.com/demos/websocket-client/ se puede verificar el topic simulador/#
6) podria usarse un broker mqtt local para menor latencia.
7) Desde el ESP32 nos podriamos conectar al mqtt y ya manejar todo tipo de actuadores y displays (ejemplos de uso de mqtt con ESP32 en github.com/jtoobe)
8) que lo disfruten!!!!

Probado en FSX Steam con FSUIPC 4.977 (version no registrada)

Falta probar en P3D
