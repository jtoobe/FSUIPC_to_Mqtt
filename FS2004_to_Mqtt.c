#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "FSUIPC_User.h"
#include "MQTTClient.h"

#define MQTT_ADDRESS  "tcp://broker.hivemq.com:1883"
#define MQTT_TOPIC    "simulador/estado"
#define MQTT_QOS      1
#define MQTT_TIMEOUT  10000L

MQTTClient client;

void enviarDatos(double altitud, double velocidad) {
    char mensaje[100];
    snprintf(mensaje, sizeof(mensaje), "Altitud: %.0f ft, IAS: %d kt", altitud, (int)velocidad);

    printf("Enviando a MQTT: %s\n", mensaje);

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = mensaje;
    pubmsg.payloadlen = (int)strlen(mensaje);
    pubmsg.qos = MQTT_QOS;
    pubmsg.retained = 0;

    int rc = MQTTClient_publishMessage(client, MQTT_TOPIC, &pubmsg, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Error publicando mensaje MQTT: %d\n", rc);
    } else {
        MQTTClient_waitForCompletion(client, token, MQTT_TIMEOUT);
    }
}

int main(void) {
    DWORD dwResult;
    WORD simHour = 0, simMin = 0, simSec = 0;
    DWORD altitudeRaw = 0;
    WORD ias = 0;
    double altitudeFeet = 0.0;
    double iasKt = 0.0;

    // ID aleatorio para evitar conflictos con HiveMQ
    char clientID[32];
    sprintf(clientID, "FSUIPC_Pub_%d", GetTickCount() & 0xFFFF);

    // Crear cliente MQTT
    MQTTClient_create(&client, MQTT_ADDRESS, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, NULL, NULL);

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    int rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Error al conectar con el servidor MQTT. Código: %d\n", rc);
        return 1;
    }
    printf("Conexión MQTT establecida correctamente\n");

    // Conectar con FSUIPC
    if (FSUIPC_Open(SIM_ANY, &dwResult)) {
        printf("Conectado a FSUIPC\n");

        while (1) {
            FSUIPC_Read(0x0238, 2, &simHour, &dwResult);
            FSUIPC_Read(0x023A, 2, &simMin, &dwResult);
            FSUIPC_Read(0x023C, 2, &simSec, &dwResult);
            FSUIPC_Read(0x0574, 4, &altitudeRaw, &dwResult);
            FSUIPC_Read(0x02BC, 2, &ias, &dwResult);

            if (FSUIPC_Process(&dwResult)) {
                altitudeFeet = (double)altitudeRaw * 3.28;
                iasKt = (double)ias / 128;

                printf("Hora: %02d:%02d:%02d | Altitud: %.0f ft | Velocidad IAS: %d kt\n",
                    simHour, simMin, simSec, altitudeFeet, (int)iasKt);

                enviarDatos(altitudeFeet, iasKt);
            } else {
                printf("FSUIPC_Process error: %lu\n", dwResult);
            }

            Sleep(1000);
        }

        FSUIPC_Close();
    } else {
        printf("Error al conectar con FSUIPC: Código %lu\n", dwResult);
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}
