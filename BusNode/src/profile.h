
#ifndef _PROFILE_H_
#define _PROFILE_H_

// Service data
#define PROFILE_DATA_NODE        0
#define PROFILE_DATA_COLLECTOR   1


// Roles
#define ROLE_NODE           1
#define ROLE_COLLECTOR      2
#define ROLE_INQUIRER       3

#if PROFILE_ROLE == ROLE_COLLECTOR
#define SERVICE_DATA_TO_SEARCH_FOR  PROFILE_DATA_NODE
#define LOCAL_NAME "Central"
#elif PROFILE_ROLE == ROLE_INQUIRER
#define SERVICE_DATA_TO_SEARCH_FOR  PROFILE_DATA_COLLECTOR
#define LOCAL_NAME "Master"
#elif PROFILE_ROLE == ROLE_NODE
#define LOCAL_NAME "Node"
#endif


uint8_t DeviceInit(void);
void APP_Tick(void);

#endif // _PROFILE_H_
