Programa en "C" que toma datos de FSUIPC 3.999 corriendo en un FS2004 y los publica en HiveMq (mqtt)

La idea es tomar esos datos con ESP32's y manejar todo tipo de salidas y displays

1) bajamos visual studio build tools 2022
2) bajamos paho-mqtt de https://github.com/eclipse-paho/paho.mqtt.c , bajamos el FSUIPC_SDK de https://www.fsuipc.com/  (programa que deberemos instalar para la version del simulador que tengamos)
3) en la carpeta ....\FSUIPC_SDK\SDK\UIPC_SDK_C copiamos los .lib .dll y .h de paho-mqtt
4) compilamos este programa con: cl FS2004_to_Mqtt.c FSUIPC_User.lib paho-mqtt3c.lib /link User32.lib /NODEFAULTLIB:LIBC.lib
5) les suministro un archivo .bat que simplifica la compilacion.
6) ejecutamos y conectandose a: https://www.hivemq.com/demos/websocket-client/ se puede verificar el topic simulador/#
7) podria usarse un broker mqtt local para menor latencia.
8) Desde el ESP32 nos podriamos conectar al mqtt y ya manejar todo tipo de actuadores y displays
9) En la carpeta FSUIPC_TO_MQTT esta la version que solo LEE valores de FSUIPC y los publica en MQTT
10) En la carpeta MQTT_TO_FSUIPC esta la version que LEE valores de FSUIPC y los publica en MQTT y tambien LEE valores de MQTT y los inyecta en FSUIPC para controlar el simulador
11) En el branch principal la definicion de valores esta en el fuente del programa
12) la DLL 

*************************************************************************************************************************************

Probado en FSX Steam con FSUIPC 4.977 (version no registrada)

*************************************************************************************************************************************


Falta probar en P3D

*************************************************************************************************************************************

Que permite este desarrollo? Tener conectados por wifi varios ESP32 a los datos del simulador. Esta forma de comunicacion no consume recursos de la maquina donde se ejecuta FS2004/FSX. Podrian emularse instrumentos como modulos independientes usando pantallas tft de bajo costo o usar servos e impresion 3D para instrumental analogico. En mi caso usando coolers de PC para crear un efecto de forceFeedback en el yugo y asiento.

Uso mobiflight para la construccion de cabinas de vuelo, este desarrollo permitiria la liberacion de recursos usando solo los MEGA para inputs si usas el programa de la carpeta FSUIPC_TO_MQTT

El programa de la carpeta MQTT_TO_FSUIPC permite la comunicacion bidireccional entre MQTT y el SIM y permitiria dejar de usar Mobiflight. Igualmente habria que ver el tema de latencia al configurar muchos topics.

Que lo disfruten !!!!

*************************************************************************************************************************************

Como usar un arduino mega para conectarlo a MQTT ?

https://github.com/petez69/MQTT-Arduino-MEGA-I-O https://github.com/Domochip/MegaMQTT https://www.instructables.com/Use-MQTT-Client-in-Arduino/

Aqui hay ejemplos de como hacerlo, esto permitiria un gran numero de salidas analogicas y digitales

Como usar un arduino UNO ?

https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-mqtt-device-to-device/

Explicacion del protocolo MQTT por Luis LLamas (Pagina Excelente!!!)

https://www.luisllamas.es/enviar-y-recibir-mensajes-por-mqtt-con-arduino-y-la-libreria-pubsubclient/
