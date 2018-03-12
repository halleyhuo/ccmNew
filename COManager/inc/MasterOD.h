
#ifndef MASTEROD_H
#define MASTEROD_H

#include "data.h"

/* Prototypes of function provided by object dictionnary */
UNS32 COMaster_valueRangeTest (UNS8 typeValue, void * value);
const indextable * COMaster_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks);

/* Master node data struct */
extern CO_Data COMaster_Data;
extern UNS32 COMaster_obj1006;



#endif // MASTEROD_H
