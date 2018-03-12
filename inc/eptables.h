#ifndef EP_TABLES_H
#define EP_TABLES_H
#include "IDCType.h"

#define	KV_BP_TAB_LEN		105
#define	LIN_BP_TAB_LEN		281
typedef	struct kv_bp_tab_tag
{
            ushort    size;
            ushort    kv[KV_BP_TAB_LEN];
}__attribute__ ((packed)) kv_bp_tab_t	;


typedef	struct linear_bp_tab_tag
			{
            ushort    laenge;
			ulong	linear_wert[LIN_BP_TAB_LEN];
			}/*__attribute__ ((packed))*/
			linear_bp_tab_t;
extern const kv_bp_tab_t kv_bp_tab;
extern const linear_bp_tab_t linear_bp_tab;

typedef struct
{
    ushort kv;
    ulong  ma;
}ContinueParaPair;

extern
#endif

