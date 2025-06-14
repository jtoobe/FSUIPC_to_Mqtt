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
} Dato;


Dato datos[MAX_DATOS];
int cantidad_datos = 0;
MQTTClient client;
int qos = 1;
int timeout = 10000;
int intervalo_publicacion = 1000;

void cargarConfiguracionManual() {
    FILE* f = fopen(CONFIG_FILE, "r");
    if (!f) {
        printf("Error al abrir %s\n", CONFIG_FILE);
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

    while (fgets(linea, sizeof(linea), f)) {
        char* p = linea;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == ';' || *p == '#' || *p == '\n' || *p == '\r') continue;

        if (*p == '[') {
            // Si se cierra un dato, evaluarlo
		
		if (dentro_dato && cantidad_datos < MAX_DATOS) {
		    if (!temp.habilitado) {
			printf("[IGNORADO] Seccion con offset=0x%X deshabilitada por configuracion.\n", temp.offset);
		    } else if (temp.offset || strcmp(temp.type, "WORD") == 0) {
			if (strlen(temp.topic) == 0)
			    snprintf(temp.topic, sizeof(temp.topic), "simulador/dato%d", cantidad_datos + 1);

			datos[cantidad_datos] = temp;
			datos[cantidad_datos].buffer = strcmp(temp.type, "WORD") == 0
			    ? malloc(sizeof(WORD)) : malloc(sizeof(DWORD));

			printf("[dato%d] offset=0x%X, type=%s, scale=%.4f, topic=%s, habilitado=%d\n",
			    cantidad_datos + 1, temp.offset, temp.type, temp.scale, temp.topic, temp.habilitado);
			cantidad_datos++;
		    } else {
			printf("[IGNORADO] Sección con tipo %s pero sin offset valido.\n", temp.type);
		    }
	}


            // Reset para nueva sección
            memset(&temp, 0, sizeof(Dato));
            strcpy(temp.type, "DWORD");
            temp.scale = 1.0;
            temp.habilitado = 1;
            dentro_dato = 0;

            sscanf(p, "[%31[^]]", seccion);
            if (strncmp(seccion, "dato", 4) == 0) dentro_dato = 1;
            continue;
        }

        char clave[64], valor[128];
        if (sscanf(p, "%63[^=]=%127[^\n\r]", clave, valor) == 2) {
            for (char* v = valor + strlen(valor) - 1; v > valor && (*v == ' ' || *v == '\t'); *v-- = '\0');

            if (!dentro_dato) {
                // MQTT & Publicacion
                if (strcmp(seccion, "MQTT") == 0) {
                    if (strcmp(clave, "broker") == 0) {
                        char clientID[32];
                        sprintf(clientID, "FSUIPC_%d", GetTickCount() & 0xFFFF);
                        MQTTClient_create(&client, valor, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
                        MQTTClient_setCallbacks(client, NULL, NULL, NULL, NULL);
                        printf("Broker: %s\n", valor);
                    } else if (strcmp(clave, "qos") == 0) {
                        qos = atoi(valor);
                        printf("QoS: %d\n", qos);
                    } else if (strcmp(clave, "timeout") == 0) {
                        timeout = atoi(valor);
                        printf("Timeout: %d\n", timeout);
                    }
                } else if (strcmp(seccion, "Publicacion") == 0) {
                    if (strcmp(clave, "intervalo") == 0) {
                        intervalo_publicacion = atoi(valor);
                        printf("Intervalo de publicación: %d ms\n", intervalo_publicacion);
                    }
                }
            } else {
                // DatoX
                if (strcmp(clave, "offset") == 0)
                    temp.offset = strtoul(valor, NULL, 0);
                else if (strcmp(clave, "type") == 0)
                    strncpy(temp.type, valor, sizeof(temp.type));
                else if (strcmp(clave, "scale") == 0)
                    temp.scale = atof(valor);
                else if (strcmp(clave, "topic") == 0)
                    strncpy(temp.topic, valor, sizeof(temp.topic));
                else if (strcmp(clave, "habilitado") == 0)
                    temp.habilitado = atoi(valor);
            }
        }
    }

    // Procesar el último bloque si era un dato
    if (dentro_dato && temp.habilitado && cantidad_datos < MAX_DATOS) {
        if (temp.offset || strcmp(temp.type, "WORD") == 0) {
            if (strlen(temp.topic) == 0)
                snprintf(temp.topic, sizeof(temp.topic), "simulador/dato%d", cantidad_datos + 1);

            datos[cantidad_datos] = temp;
            datos[cantidad_datos].buffer = strcmp(temp.type, "WORD") == 0
                ? malloc(sizeof(WORD)) : malloc(sizeof(DWORD));

            printf("[dato%d] offset=0x%X, type=%s, scale=%.4f, topic=%s\n",
                cantidad_datos + 1, temp.offset, temp.type, temp.scale, temp.topic);
            cantidad_datos++;
        } else {
            printf("Último dato sin offset válido. Ignorado.\n");
        }
    }

    fclose(f);
    printf("\nTotal de datos cargados: %d\n\n", cantidad_datos);
}





void publicarDato(Dato* d) {
    double valor = 0.0;
    if (strcmp(d->type, "WORD") == 0)
        valor = (*(WORD*)d->buffer) * d->scale;
    else
        valor = (*(DWORD*)d->buffer) * d->scale;

    char payload[64];
    snprintf(payload, sizeof(payload), "%.2f", valor);
    printf("Publicando %s => %s\n", d->topic, payload);

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
        printf("Error MQTT (%d)\n", rc);
    }
}

int main() {
    DWORD dwResult;
	
	char path[MAX_PATH];
GetCurrentDirectoryA(MAX_PATH, path);
printf("Directorio actual: %s\n", path);

	
    cargarConfiguracionManual();

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("Error al conectar con el broker MQTT\n");
        return 1;
    }

    if (!FSUIPC_Open(SIM_ANY, &dwResult)) {
        printf("Error al conectar con FSUIPC: %lu\n", dwResult);
        return 1;
    }

    printf("Conexiones exitosas. Transmitiendo...\n");

    while (1) {
        for (int i = 0; i < cantidad_datos; i++) {
            int size = (strcmp(datos[i].type, "WORD") == 0) ? sizeof(WORD) : sizeof(DWORD);
            FSUIPC_Read(datos[i].offset, size, datos[i].buffer, &dwResult);
        }

        if (FSUIPC_Process(&dwResult)) {
            for (int i = 0; i < cantidad_datos; i++) {
                publicarDato(&datos[i]);
            }
        } else {
            printf("FSUIPC_Process error: %lu\n", dwResult);
        }

        Sleep(intervalo_publicacion);
    }

    FSUIPC_Close();
    MQTTClient_disconnect(client, timeout);
    MQTTClient_destroy(&client);
    return 0;
}
