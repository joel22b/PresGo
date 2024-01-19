/***************************************************************************//**
 * @file
 * @brief Network - Dns Client Command Module
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _DNS_CLIENT_CMD_PRIV_H_
#define  _DNS_CLIENT_CMD_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void DNScCmd_Init(RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DNS_CLIENT_CMD_PRIV_H_
