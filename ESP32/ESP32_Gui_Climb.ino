//<App !Start!>
// FILE: [ESP32_Gui_Climb.ino]
// Created by GUIslice Builder version: [0.17.b40]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<App !End!>

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "ESP32_Gui_Climb_GSLC.h"
#include "User_Setup.h"
// #include <ESP8266WiFi.h> // Para ESP8266
#include <WiFi.h>  // Para ESP32
WiFiClient WIFI_CLIENT;
#include <PubSubClient.h>
PubSubClient MQTT_CLIENT;

// Nombre y contraseña red WiFi.
const char* ssid = "*******************";
const char* password = "*****************";



// ------------------------------------------------
// Program Globals
// ------------------------------------------------
// Free-running counter for display
unsigned entero = 0;
unsigned VSpeed = 0;
unsigned Altitud = 0;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_pElemInTxt1 = NULL;
gslc_tsElemRef* m_pElemInTxt2 = NULL;
gslc_tsElemRef* m_pElemRadial1 = NULL;
gslc_tsElemRef* m_pElemKeyPadAlpha = NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) {
  if (ch == (char)'\n') Serial.println("");
  else Serial.write(ch);
  return 0;
}

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY) {
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pElemRef);

  if (eTouch == GSLC_TOUCH_UP_IN) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
        //<Button Enums !Start!>
      case E_ELEM_TEXTINPUT1:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadAlpha, E_POP_KEYPAD_ALPHA, m_pElemInTxt1);
        break;
      case E_ELEM_TEXTINPUT2:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadAlpha, E_POP_KEYPAD_ALPHA, m_pElemInTxt2);
        break;
        //<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
// Checkbox / radio callbacks
// - Creating a callback function is optional, but doing so enables you to
//   detect changes in the state of the elements.
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState) {
  gslc_tsGui* pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pElemRef);
  if (pElemRef == NULL) {
    return false;
  }



  // Determine which element issued the callback
  switch (pElem->nId) {
      //<Checkbox Enums !Start!>

      //<Checkbox Enums !End!>
    default:
      break;
  }  // switch
  return true;
}
// KeyPad Input Ready callback
bool CbKeypad(void* pvGui, void* pvElemRef, int16_t nState, void* pvData) {
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pElemRef);

  // From the pvData we can get the ID element that is ready.
  int16_t nTargetElemId = gslc_ElemXKeyPadDataTargetIdGet(pGui, pvData);
  if (nState == XKEYPAD_CB_STATE_DONE) {
    // User clicked on Enter to leave popup
    // - If we have a popup active, pass the return value directly to
    //   the corresponding value field
    switch (nTargetElemId) {
        //<Keypad Enums !Start!>
      case E_ELEM_TEXTINPUT1:
        gslc_ElemXKeyPadInputGet(pGui, m_pElemInTxt1, pvData);
        break;

      case E_ELEM_TEXTINPUT2:
        gslc_ElemXKeyPadInputGet(pGui, m_pElemInTxt2, pvData);
        break;
        //<Keypad Enums !End!>
      default:
        break;
    }
  } else if (nState == XKEYPAD_CB_STATE_CANCEL) {
    // User escaped from popup, so don't update values
    gslc_PopupHide(&m_gui);
  }
  return true;
}
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

void setup() {
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(115200);
  // Wait for USB Serial
  delay(100);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used
               // Conectar con WiFi.
  Serial.println();
  Serial.print("Conectando con ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Configuración de la respuesta.
  MQTT_CLIENT.setCallback(callback);

  gslc_InitDebug(&DebugOut);

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUIslice_gen();
}

// Aquí configuramos lo que debe hacer cuando recibe un valor.
void callback(char* recibido, byte* payload, unsigned int length) {
  //Serial.print("Mensaje recibido: ");
  //Serial.print(recibido);
  //Serial.print("   ");

  // Convertir payload en una cadena terminada en null
  char mensaje[length + 1];
  for (int i = 0; i < length; i++) {
    mensaje[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }
  mensaje[length] = '\0';  // Importante para que atoi funcione



  // Ajusto valor para que se muestre correctamente en la GUI
  entero = atoi(mensaje);

  if (strcmp(recibido, "simulador/VSpeed") == 0) {
    // falta ajustar los calculos para adecuarlo al grafico en el display
    if (VSpeed + 65000) {
      VSpeed = map(entero, 0, 1000, 0, 100) + 75;
    } else {
      VSpeed = 75 - map(entero, 0, 66000, 0, 100);
    }
    //Serial.print("   Velocidad mapeada: ");
    //Serial.println(VSpeed);
  } else if (strcmp(recibido, "simulador/altitud") == 0) {
    //Serial.print("   Altura mapeada: ");
    Altitud = entero;
    //Serial.println(Altitud);
  }

  Serial.println(VSpeed);
  Serial.println(Altitud);
}


// -----------------------------------
// Main event loop
// -----------------------------------
void loop() {

  // ------------------------------------------------
  // Lectura MQTT
  // ------------------------------------------------

  if (!MQTT_CLIENT.connected()) {
    reconnect();
  }
  MQTT_CLIENT.loop();  // Chequea lo Subscrito.

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------

  //TODO - Add update code for any text, gauges, or sliders

  gslc_ElemXRadialSetVal(&m_gui, m_pElemRadial1, VSpeed);
  char buffer[10];
  itoa(Altitud, buffer, 10);  // o sprintf(buffer, "%d", Altitud);
  gslc_ElemSetTxtStr(&m_gui, m_pElemInTxt1, buffer);


  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);

  delay(200);
}


// Reconecta con MQTT broker
void reconnect() {
  // MQTT_CLIENT.setServer("192.168.1.206", 1883);  // si se usa un servidor en la red local, verificar IP
  // MQTT_CLIENT.setServer("broker.hivemq.com", 1883);
  MQTT_CLIENT.setServer("172.22.37.203", 1883);

  MQTT_CLIENT.setClient(WIFI_CLIENT);

  // Intentando conectar con el broker.
  while (!MQTT_CLIENT.connected()) {
    Serial.println("Intentando conectar con MQTT.");
    MQTT_CLIENT.connect("XJXT061651656845165416");  // usar un nombre aleatorio
    //                      topic /  valor
    MQTT_CLIENT.subscribe("simulador/VSpeed");
    MQTT_CLIENT.subscribe("simulador/altitud");

    String myString = "no Op";
    char buffer[10];
    myString.toCharArray(buffer, 10);
    gslc_ElemSetTxtStr(&m_gui, m_pElemInTxt2, buffer);
    // Espera antes de volver a intentarlo.

    // ------------------------------------------------
    // Periodically call GUIslice update function
    // ------------------------------------------------
    gslc_Update(&m_gui);
    delay(1000);
  }


  String myString = "  ";
  char buffer[10];
  myString.toCharArray(buffer, 10);
  gslc_ElemSetTxtStr(&m_gui, m_pElemInTxt2, buffer);
  Serial.println("Conectado a MQTT.");
}
