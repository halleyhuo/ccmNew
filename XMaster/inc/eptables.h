
#include "IDCType.h"
#include "epType.h"

#ifndef __EP_TABLES_H__
#define __EP_TABLES_H__

class EpTables
{
	public:
		static ulong EpValueKv(ushort kv, EpAccuracy epAccuracy);
		static ulong EpValueMa(ushort ma, EpAccuracy epAccuracy);
		static ulong EpValueMs(ushort ms, EpAccuracy epAccuracy);
		static void GetKvFromEp(ulong epValue, ulong kvs[], EpAccuracy epAccuracy);
		static void GetMaFromEp(ulong epValue, ulong mas[], EpAccuracy epAccuracy);
		static void GetMsFromEp(ulong epValue, ulong mss[], EpAccuracy epAccuracy);
};

#endif

