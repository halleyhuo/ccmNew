
#if 0
#include "gbl/eptables.h"



const kv_bp_tab_t kv_bp_tab =
            { KV_BP_TAB_LEN ,
			 { 400,	 	403,	 405,	 408,
               410,	 	413,	 415,	 418,
               420,	 	425,	 430,	 435,
               440,	 	445,	 450,	 455,
               460,	 	465,	 470,	 475,
               480,	 	485,	 490,	 495,
               500,	 	505,	 510,	 515,
               520,	 	528,	 535,	 543,
               550,	 	555,	 560,	 565,
               570,	 	578,	 585,	 593,
               600,	 	608,	 615,	 623,
               630,	 	638,	 645,	 653,
               660,	 	670,	 680,	 690,
               700,	 	708,	 715,	 723,
               730,	 	740,	 750,	 760,
               770,	 	780,	 790,	 800,
               810,	 	820,	 830,	 840,
               850,	 	863,	 875,	 888,
               900,	 	915,	 930,	 945,
               960,	 	975,	 990,   1005,
              1020,	   1035,	1050,	1070,
              1090,	   1110,    1130,	1150,
              1170,	   1190,	1210,	1230,
              1250,	   1268,	1290,	1310,
              1330,	   1350,	1370,	1390,
              1410,	   1430,	1450,	1475,
              1500
             }
            };

const linear_bp_tab_t linear_bp_tab =
       	{ LIN_BP_TAB_LEN ,
           	{		  1,		  1,		  1,		  1,
              		  1,		  1,		  1,		  2,
              		  2,		  2,		  2,		  2,
              		  2,		  2,		  2,		  2,
              		  3,		  3,		  3,		  3,
              		  3,		  3,		  4,		  4,
              		  4,		  4,		  5,		  5,
              		  5,		  5,		  6,		  6,
              		  6,		  7,		  7,		  8,
              		  8,		  8,		  9,		  9,
/* Index: 40 */		 10,		 11,		 11,		 12,
              		 13,		 13,		 14,		 15,
              		 16,		 17,		 18,		 19,
              		 20,		 21,		 22,		 24,
              		 25,		 27,		 28,		 30,
              		 32,		 33,		 36,		 38,
              		 40,		 42,		 45,		 47,
              		 50,		 53,		 56,		 60,
              		 63,		 67,		 71,		 75,
              		 80,		 84,		 90,		 94,
/* Index: 80 */		100,		106,		110,		119,
              		125,		133,		140,		150,
              		160,		168,		180,		188,
              		200,		211,		220,		237,
              		250,		266,		280,		299,
              		320,		335,		360,		376,
              		400,		422,		450,		473,
              		500,		531,		560,		596,
              		630,		668,		710,		750,
              		800,		841,		900,		944,
/* Index: 120 */   1000,	   1059,	   1100,	   1189,
              	   1250,	   1334,	   1400,	   1496,
              	   1600,	   1679,	   1800,	   1884,
              	   2000,	   2113,	   2200,	   2371,
              	   2500,	   2661,	   2800,	   2985,
              	   3200,	   3350,	   3600,	   3758,
              	   4000,	   4217,	   4500,	   4732,
              	   5000,	   5309,	   5600,	   5957,
             	   6300,	   6683,	   7100,	   7499,
             	   8000,	   8414,	   9000,	   9441,
/* Index: 160 */  10000,	  10593,	  11000,	  11885,
              	  12500,	  13335,	  14000,	  14962,
              	  16000,	  16788,	  18000,	  18836,
              	  20000,	  21135,	  22000,	  23714,
              	  25000,	  26607,	  28000,	  29854,
              	  32000,	  33497,	  36000,	  37584,
              	  40000,	  42170,	  45000,	  47315,
              	  50000,	  53088,	  56000,	  59566,
              	  63000,	  66834,	  71000,	  74989,
              	  80000,	  84140,	  90000,	  94406,
/* Index: 200 */ 100000,	 105925,	 110000,	 118850,
              	 125000,	 133352,	 140000,	 149624,
              	 160000,	 167880,	 180000,	 188365,
              	 200000,	 211349,	 220000,	 237137,
              	 250000,	 266073,	 280000,	 298538,
              	 320000,	 334966,	 360000,	 375837,
              	 400000,	 421696,	 450000,	 473151,
              	 500000,	 530885,	 560000,	 595663,
              	 630000,	 668344,	 710000,	 749894,
              	 800000,	 841396,	 900000,	 944061,
              	1000000,	1059255,	1100000,	1188503,
              	1250000,	1333521,	1400000,	1496237,
              	1600000,	1678802,	1800000,	1883649,
              	2000000,	2113488,	2200000,	2371375,
              	2500000,	2660725,	2800000,	2985380,
              	3200000,	3349655,	3600000,	3758373,
              	4000000,	4216968,	4500000,	4731513,
              	5000000,	5308843,	5600000,	5956624,
              	6300000,	6683430,	7100000,	7498950,
              	8000000,	8413956,	9000000,	9440610,
               10000000
            }
       	};
#endif

#include "eptables.h"
#include "ep_table_16_1.h"
#include "epType.h"

ulong EpTables::EpValueKv(ushort kv, EpAccuracy epAccuracy)
{
	const ulong		*epKv;
	ushort			sizeEpKv;
	ushort			index;
	ushort			multi;

	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			epKv = ep_kv_16_1;
			sizeEpKv = sizeof(ep_kv_16_1);
			multi = 625; // index * 10000 / 16
			break;

		case EpAccuracy4_1:
			break;

		case EpAccuracy2_1:
			break;

		case EpAccuracy1:
			break;

		default:
			break;
	}

	for(index = 0; index < sizeEpKv; index++)
	{
		if((epKv[index] == kv) || ((index + 1 < sizeEpKv) && epKv[index] < kv && epKv[index+1] > kv))
		{
			break;
		}
	}

	return (index * multi);
}

ulong EpTables::EpValueMa(ushort ma, EpAccuracy epAccuracy)
{
	const ulong			*epMa;
	ushort			sizeEpMa;
	ushort			index;
	ushort			multi;

	
	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			epMa = ep_ma_16_1;
			sizeEpMa = sizeof(ep_ma_16_1);
			multi = 625; // index * 10000 / 16
			break;

		case EpAccuracy4_1:
			break;

		case EpAccuracy2_1:
			break;

		case EpAccuracy1:
			break;

		default:
			break;
	}

	for(index = 0; index < sizeEpMa; index++)
	{
		if((epMa[index] == ma) || ((index + 1 < sizeEpMa) && epMa[index] < ma && epMa[index+1] > ma))
			break;
	}

	return index * multi;
}

ulong EpTables::EpValueMs(ushort ms, EpAccuracy epAccuracy)
{
	const ulong			*epMs;
	ushort			sizeEpMs;
	ushort			index;
	ushort			multi;

	
	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			epMs = ep_ms_16_1;
			sizeEpMs = sizeof(ep_ms_16_1);
			multi = 625; // index * 10000 / 16
			break;

		case EpAccuracy4_1:
			break;

		case EpAccuracy2_1:
			break;

		case EpAccuracy1:
			break;

		default:
			break;
	}

	for(index = 0; index < sizeEpMs; index++)
	{
		if((epMs[index] == ms) || ((index + 1 < sizeEpMs) && epMs[index] < ms && epMs[index+1] > ms))
			break;
	}

	return index * multi;

}

void EpTables::GetKvFromEp(ulong epValue, ulong kvs[], EpAccuracy epAccuracy)
{
	const ulong				*epKv;
	ushort				sizeEpKv;
	ushort				i;

	ushort				multi;
	ushort				div;

	ushort				quotient;
	ushort				remainder;
	
	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			epKv = ep_kv_16_1;
			sizeEpKv = sizeof(ep_kv_16_1);
			multi = 16;
			div = 10000;
			break;

		case EpAccuracy4_1:
			break;

		case EpAccuracy2_1:
			break;

		case EpAccuracy1:
			break;

		default:
			break;
	}

	quotient = epValue*multi/div;
	remainder = (epValue*multi) % div;

	if(remainder == 0)
	{
		kvs[0] = epKv[quotient];
		kvs[1] = epKv[quotient];
	}
	else
	{
		kvs[0] = epKv[quotient];
		kvs[1] = epKv[quotient+1];
	}

	return;
}

void EpTables::GetMaFromEp(ulong epValue, ulong mas[], EpAccuracy epAccuracy)
{
	const ulong				*epMa;
	ushort				sizeEpMa;
	ushort				i;

	ushort				multi;
	ushort				div;

	ushort				quotient;
	ushort				remainder;
	
	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			epMa = ep_ma_16_1;
			sizeEpMa = sizeof(ep_ma_16_1);
			multi = 16;
			div = 10000;
			break;

		case EpAccuracy4_1:
			break;

		case EpAccuracy2_1:
			break;

		case EpAccuracy1:
			break;

		default:
			break;
	}

	quotient = epValue*multi/div;
	remainder = (epValue*multi) % div;

	if(remainder == 0)
	{
		mas[0] = epMa[quotient];
		mas[1] = epMa[quotient];
	}
	else
	{
		mas[0] = epMa[quotient];
		mas[1] = epMa[quotient+1];
	}

	return;

}

void EpTables::GetMsFromEp(ulong epValue, ulong mss[], EpAccuracy epAccuracy)
{
	const ulong				*epMs;
	ushort				sizeEpMs;
	ushort				i;

	ushort				multi;
	ushort				div;

	ushort				quotient;
	ushort				remainder;
	
	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			epMs = ep_ms_16_1;
			sizeEpMs = sizeof(ep_ms_16_1);
			multi = 16;
			div = 10000;
			break;

		case EpAccuracy4_1:
			break;

		case EpAccuracy2_1:
			break;

		case EpAccuracy1:
			break;

		default:
			break;
	}

	quotient = epValue*multi/div;
	remainder = (epValue*multi) % div;

	if(remainder == 0)
	{
		mss[0] = epMs[quotient];
		mss[1] = epMs[quotient];
	}
	else
	{
		mss[0] = epMs[quotient];
		mss[1] = epMs[quotient+1];
	}

	return;

}




