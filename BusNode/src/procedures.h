#ifndef _PROCEDURES_H_
#define _PROCEDURES_H_

extern char name[];

void PrintBondedDevices(void);
uint8_t GetNumBondedDevices(void);
tBleStatus ConfigureAdvertising(uint8_t filter_policy);
tBleStatus StartAdvertising(void);
tBleStatus StopAdvertising(void);

tBleStatus StartGeneralConnectionEstablishment(void);
tBleStatus StartAutoConnection(void);
void StopScan(void);

int BlacklistHit(uint8_t address_type, uint8_t address[]);
void BlacklistReset(void);


#endif /* _PROCEDURES_H_ */
