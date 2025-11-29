Programa que permite publicar offset de FSUIPC en MQTT

los datos a subir se definen en el archivo config.ini

Por ejemplo: 

[dato1]
offset=0x0570
type=DWORD
scale=0.000002364   ; convierte a pies (aprox)
topic=sim/altitude
habilitado=1


[dato2]
offset=0x02C8
type=DWORD
scale=0.234375      ; convierte a pies/min
topic=sim/vertical_speed
habilitado=1


[dato3]
offset=0x02BC
type=DWORD
scale=0.0078125     ; convierte a nudos
topic=sim/ias
habilitado=1


Los valores pueden ser WORD o DWORD
El valor de scale varia en funcion al dato leido de FSUIPC (recomiendo usar FS-interrogate). El valor leiodo es multiplicado por la cantidad en esta configuracion.

habilitado puede ser 1 o 0 dependiendo si se lee o no ese offset


