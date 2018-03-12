#ifndef __DATATRANSFER_H
#define __DATATRANSFER_H

#include "includes.h"

void ANO_SendAtt(void);
void ANO_SendMotec(void);
void ANO_SendADC(void);
void ANO_Send6050(void);
void ANO_SendGPS(void);

void CAN_SendLicence(void);
void CAN_SendADC_A(void);
void CAN_SendADC_B(void);
void CAN_SendGPS_A(void);
void CAN_SendGPS_B(void);

void SendDATA(void);

#endif
