/*********************************************************************
 *                SEGGER Microcontroller GmbH & Co. KG                *
 *        Solutions for real time microcontroller applications        *
 **********************************************************************
 *                                                                    *
 *        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
 *                                                                    *
 *        Internet: www.segger.com    Support:  support@segger.com    *
 *                                                                    *
 **********************************************************************

 ** emWin V5.34 - Graphical user interface for embedded applications **
   All  Intellectual Property rights  in the Software belongs to  SEGGER.
   emWin is protected by  international copyright laws.  Knowledge of the
   source code may not be used to write a similar product.  This file may
   only be used in accordance with the following terms:

   The  software has  been licensed  to Silicon Labs Norway, a subsidiary
   of Silicon Labs Inc. whose registered office is 400 West Cesar Chavez,
   Austin,  TX 78701, USA solely for  the purposes of creating  libraries
   for its  ARM Cortex-M3, M4F  processor-based devices,  sublicensed and
   distributed  under the  terms and conditions  of the  End User License
   Agreement supplied by Silicon Labs.
   Full source code is available at: www.segger.com

   We appreciate your understanding and fairness.
   ----------------------------------------------------------------------
   Licensing information

   Licensor:                 SEGGER Software GmbH
   Licensed to:              Silicon Laboratories Norway
   Licensed SEGGER software: emWin
   License number:           GUI-00140
   License model:            See Agreement, dated 20th April 2012
   Licensed product:         -
   Licensed platform:        Cortex M3, Cortex M4F
   Licensed number of seats: -
   ----------------------------------------------------------------------
   File        : GUIConf.c
   Purpose     : Display controller initialization
   ---------------------------END-OF-HEADER------------------------------
 */

#include "GUI.h"

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
//
// Define the available number of bytes available for the GUI
//
#define GUI_NUMBYTES  0x60000 // Arbitrarily sufficient for at least one frame

/*********************************************************************
 *
 *       Public code
 *
 **********************************************************************
 */
/*********************************************************************
 *
 *       GUI_X_Config
 *
 * Purpose:
 *   Called during the initialization process in order to set up the
 *   available memory for the GUI.
 */
void GUI_X_Config(void)
{
  //
  // 32 bit aligned memory area
  //
  static U32 aMemory[GUI_NUMBYTES / 4];
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  //
  // Set default font
  //
  GUI_SetDefaultFont(GUI_FONT_6X8);
}

/*************************** End of file ****************************/
