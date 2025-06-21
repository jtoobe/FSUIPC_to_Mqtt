#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FSUIPC_User.h"
#include "MQTTClient.h"

#define MAX_DATOS 10
#define CONFIG_FILE "config.ini"

typedef struct {
    DWORD offset;
    char type[8];
    double scale;
    char topic[128];
    void* buffer;
    int habilitado;
    int escucha_mqtt;
} Dato;

Dato datos[MAX_DATOS];
int cantidad_datos = 0;
MQTTClient client;
int qos = 1;
int timeout = 10000;
int intervalo_publicacion = 1000;

void cargarConfiguracion() {
    FILE* f = fopen(CONFIG_FILE, "r");
    if (!f) {
        printf("No se pudo abrir el archivo de configuración: %s\n", CONFIG_FILE);
        exit(1);
    }

    char linea[256], seccion[32] = "";
    Dato temp;
    cantidad_datos = 0;
    int dentro_dato = 0;

    memset(&temp, 0, sizeof(Dato));
    strcpy(temp.type, "DWORD");
    temp.scale = 1.0;
    temp.habilitado = 1;
    temp.escucha_mqtt = 1;

    while (fgets(linea, sizeof(linea), f)) {
        char* p = linea;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == ';' || *p == '#' || *p == '\n' || *p == '\r') continue;

        if (*p == '[') {
            if (dentro_dato && temp.habilitado && cantidad_datos < MAX_DATOS) {
                if (!temp.offset) {
                    printf("[IGNORADO] Sección sin offset válido.\n");
                } else {
                    datos[cantidad_datos] = temp;
                    datos[cantidad_datos].buffer = strcmp(temp.type, "WORD") == 0
                        ? malloc(sizeof(WORD)) : malloc(sizeof(DWORD));
                    cantidad_datos++;
                }
            }
            memset(&temp, 0, sizeof(Dato));
            strcpy(temp.type, "DWORD");
            temp.scale = 1.0;
            temp.habilitado = 1;
            temp.escucha_mqtt = 1;

            sscanf(p, "[%31[^]]", seccion);
            dentro_dato = strncmp(seccion, "dato", 4) == 0;
            continue;
        }

        char clave[64], valor[128];
        if (sscanf(p, "%63[^=]=%127[^\n\r]", clave, valor) == 2) {
            for (char* v = valor + strlen(valor) - 1; v > valor && (*v == ' ' || *v == '\t'); *v-- = '\0');

            if (!dentro_dato) {
                if (strcmp(seccion, "MQTT") == 0) {
                    if (strcmp(clave, "broker") == 0) {
                        char clientID[32];
                        sprintf(clientID, "FSUIPC_%d", GetTickCount() & 0xFFFF);
                        MQTTClient_create(&client, valor, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
                        printf("Broker: %s\n", valor);
                    } else if (strcmp(clave, "qos") == 0) qos = atoi(valor);
                    else if (strcmp(clave, "timeout") == 0) timeout = atoi(valor);
                } else if (strcmp(seccion, "Publicacion") == 0 && strcmp(clave, "intervalo") == 0) {
                    intervalo_publicacion = atoi(valor);
                }
            } else {
                if (strcmp(clave, "offset") == 0) temp.offset = strtoul(valor, NULL, 0);
                else if (strcmp(clave, "type") == 0) strncpy(temp.type, valor, sizeof(temp.type));
                else if (strcmp(clave, "scale") == 0) temp.scale = atof(valor);
                else if (strcmp(clave, "topic") == 0) strncpy(temp.topic, valor, sizeof(temp.topic));
                else if (strcmp(clave, "habilitado") == 0) temp.habilitado = atoi(valor);
                else if (strcmp(clave, "escucha_mqtt") == 0) temp.escucha_mqtt = atoi(valor);
            }
        }
    }

    if (dentro_dato && temp.habilitado && cantidad_datos < MAX_DATOS) {
        datos[cantidad_datos] = temp;
        datos[cantidad_datos].buffer = strcmp(temp.type, "WORD") == 0
            ? malloc(sizeof(WORD)) : malloc(sizeof(DWORD));
        cantidad_datos++;
    }

    fclose(f);
    printf("\nResumen de configuración:\n");
printf("%-6s %-10s %-7s %-9s %-40s\n", "Dato", "Offset", "Publica", "Escucha", "Topic");
printf("---------------------------------------------------------------\n");

for (int i = 0; i < cantidad_datos; i++) {
    printf("[%-4d] 0x%06X   %-7s %-9s %-40s\n",
        i + 1,
        datos[i].offset,
        datos[i].habilitado ? "SI" : "NO",
        datos[i].escucha_mqtt ? "SI" : "NO",
        datos[i].topic);
}
printf("\n");

}

void publicarDato(Dato* d) {
    double valor = 0.0;
    if (strcmp(d->type, "WORD") == 0)
        valor = (*(WORD*)d->buffer) * d->scale;
    else
        valor = (*(DWORD*)d->buffer) * d->scale;

    char payload[64];
    snprintf(payload, sizeof(payload), "%.2f", valor);
    printf("<< FSUIPC ? MQTT: %s = %s\n", d->topic, payload);


    MQTTClient_message msg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    msg.payload = payload;
    msg.payloadlen = (int)strlen(payload);
    msg.qos = qos;
    msg.retained = 0;

    int rc = MQTTClient_publishMessage(client, d->topic, &msg, &token);
    if (rc == MQTTCLIENT_SUCCESS) {
        MQTTClient_waitForCompletion(client, token, timeout);
    } else {
        printf("Error al publicar en MQTT (%s): %d\n", d->topic, rc);
    }
}

int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
    char payload[64];
    snprintf(payload, sizeof(payload), "%.*s", message->payloadlen, (char*)message->payload);
    int valor = atoi(payload);

    for (int i = 0; i < cantidad_datos; i++) {
        if (datos[i].escucha_mqtt && strcmp(topicName, datos[i].topic) == 0) {
            DWORD dwResult;
            int size = strcmp(datos[i].type, "WORD") == 0 ? sizeof(WORD) : sizeof(DWORD);
            DWORD valor32 = (DWORD)valor;
            FSUIPC_Write(datos[i].offset, size, &valor32, &dwResult);
            if (FSUIPC_Process(&dwResult)) {
                printf(">> MQTT ? FSUIPC: %s = %d escrito en 0x%X\n", topicName, valor, datos[i].offset);
            } else {
                printf("Error al escribir en FSUIPC desde MQTT (topic: %s)\n", topicName);
            }
            break;
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main() {
    DWORD dwResult;

    cargarConfiguracion();

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("No se pudo conectar al broker MQTT\n");
        return 1;
    }

    for (int i = 0; i < cantidad_datos; i++) {
        if (datos[i].escucha_mqtt) {
            MQTTClient_subscribe(client, datos[i].topic, qos);
        }
    }

    if (!FSUIPC_Open(SIM_ANY, &dwResult)) {
        printf("No se pudo conectar con FSUIPC: %lu\n", dwResult);
        return 1;
    }

    printf("Conexión exitosa. Comenzando transmisión.\n");

    while (1) {
        for (int i = 0; i < cantidad_datos; i++) {
            int size = strcmp(datos[i].type, "WORD") == 0 ? sizeof(WORD) : sizeof(DWORD);
            FSUIPC_Read(datos[i].offset, size, datos[i].buffer, &dwResult);
        }

        if (FSUIPC_Process(&dwResult)) {
            for (int i = 0; i < cantidad_datos; i++) {
                if (datos[i].habilitado) {
                    publicarDato(&datos[i]);
                }
            }
        }

        MQTTClient_yield();
        Sleep(intervalo_publicacion);
    }

    FSUIPC_Close();
    MQTTClient_disconnect(client, timeout);
    MQTTClient_destroy(&client);
    return 0;
}
