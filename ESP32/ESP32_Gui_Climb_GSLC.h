//<File !Start!>
// FILE: [ESP32_Gui_Climb_GSLC.h]
// Created by GUIslice Builder version: [0.17.b40]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>

#ifndef _GUISLICE_GEN_H
#define _GUISLICE_GEN_H

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XKeyPad_Alpha.h"
#include "elem/XRadial.h"

// Ensure optional features are enabled in the configuration
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if !defined(DRV_DISP_TFT_ESPI)
  #error E_PROJECT_OPTIONS tab->Graphics Library should be Adafruit_GFX
#endif
#include <TFT_eSPI.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_POP_KEYPAD_ALPHA};
enum {E_ELEM_RADIALGAUGE1,E_ELEM_TEXT1,E_ELEM_TEXT2,E_ELEM_TEXT3
      ,E_ELEM_TEXT4,E_ELEM_TEXT5,E_ELEM_TEXT6,E_ELEM_TEXT7,E_ELEM_TEXT8
      ,E_ELEM_TEXTINPUT1,E_ELEM_TEXTINPUT2,E_ELEM_KEYPAD_ALPHA};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN10X16,E_BUILTIN5X8,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 11 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asKeypadAlphaElem[1];
gslc_tsElemRef                  m_asKeypadAlphaElemRef[1];
gslc_tsXKeyPad                  m_sKeyPadAlpha;
gslc_tsXRadial                  m_sXRadialGauge1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_pElemInTxt1;
extern gslc_tsElemRef* m_pElemInTxt2;
extern gslc_tsElemRef* m_pElemRadial1;
extern gslc_tsElemRef* m_pElemKeyPadAlpha;
//<Extern_References !End!>

// Define debug message function
static int16_t DebugOut(char ch);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState);
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId);
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos);
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbTickScanner(void* pvGui,void* pvScope);

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
void InitGUIslice_gen()
{
  gslc_tsElemRef* pElemRef = NULL;

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_BUILTIN10X16,GSLC_FONTREF_PTR,NULL,2)) { return; }
    if (!gslc_FontSet(&m_gui,E_BUILTIN5X8,GSLC_FONTREF_PTR,NULL,1)) { return; }
//<Load_Fonts !End!>

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_POP_KEYPAD_ALPHA,m_asKeypadAlphaElem,1,m_asKeypadAlphaElemRef,1);  // KeyPad

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  

  // Create progress bar E_ELEM_RADIALGAUGE1 
  pElemRef = gslc_ElemXRadialCreate(&m_gui,E_ELEM_RADIALGAUGE1,E_PG_MAIN,&m_sXRadialGauge1,
    (gslc_tsRect){122,8,196,186},0,100,75,GSLC_COL_WHITE);
  gslc_ElemXRadialSetIndicator(&m_gui,pElemRef,GSLC_COL_WHITE,75,8,true);
  gslc_ElemXRadialSetTicks(&m_gui,pElemRef,GSLC_COL_WHITE,25,10);
  m_pElemRadial1 = pElemRef;
  
  // Create E_ELEM_TEXT1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT1,E_PG_MAIN,(gslc_tsRect){10,211,60,16},
    (char*)"Climb",0,E_BUILTIN10X16);
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,E_PG_MAIN,(gslc_tsRect){14,43,24,16},
    (char*)"Up",0,E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_TEXT3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT3,E_PG_MAIN,(gslc_tsRect){6,148,48,16},
    (char*)"Down",0,E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_TEXT4 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT4,E_PG_MAIN,(gslc_tsRect){60,97,12,16},
    (char*)"0",0,E_BUILTIN10X16);
  
  // Create E_ELEM_TEXT5 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT5,E_PG_MAIN,(gslc_tsRect){72,60,21,16},
    (char*)"5",0,E_BUILTIN10X16);
  
  // Create E_ELEM_TEXT6 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT6,E_PG_MAIN,(gslc_tsRect){72,131,21,16},
    (char*)"5",0,E_BUILTIN10X16);
  
  // Create E_ELEM_TEXT7 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT7,E_PG_MAIN,(gslc_tsRect){92,16,24,16},
    (char*)"10",0,E_BUILTIN10X16);
  
  // Create E_ELEM_TEXT8 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT8,E_PG_MAIN,(gslc_tsRect){90,175,24,16},
    (char*)"10",0,E_BUILTIN10X16);
  
  // Create E_ELEM_TEXTINPUT1 text input field
  static char m_sInputText1[11] = "00000";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXTINPUT1,E_PG_MAIN,(gslc_tsRect){229,90,70,22},
    (char*)m_sInputText1,11,E_BUILTIN10X16);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_pElemInTxt1 = pElemRef;
  
  // Create E_ELEM_TEXTINPUT2 text input field
  static char m_sInputText2[11] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXTINPUT2,E_PG_MAIN,(gslc_tsRect){246,207,67,22},
    (char*)m_sInputText2,11,E_BUILTIN10X16);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_RED);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_pElemInTxt2 = pElemRef;

  // -----------------------------------
  // PAGE: E_POP_KEYPAD_ALPHA
  
  static gslc_tsXKeyPadCfg_Alpha sCfgTx;
  sCfgTx = gslc_ElemXKeyPadCfgInit_Alpha();
  m_pElemKeyPadAlpha = gslc_ElemXKeyPadCreate_Alpha(&m_gui, E_ELEM_KEYPAD_ALPHA, E_POP_KEYPAD_ALPHA,
    &m_sKeyPadAlpha, 65, 80, E_BUILTIN5X8, &sCfgTx);
  gslc_ElemXKeyPadValSetCb(&m_gui, m_pElemKeyPadAlpha, &CbKeypad);
//<InitGUI !End!>

//<Startup !Start!>
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H
