
#ifndef __EP_TYPE_H__
#define __EP_TYPE_H__

typedef enum
{
	EpAccuracy16_1,
	EpAccuracy4_1,
	EpAccuracy2_1,
	EpAccuracy1
} EpAccuracy;

/**
 * @brief Kv, ma, ms
 */
typedef struct _EpContext
{
	ulong		kv;
	ulong		ma;
	ulong		ms;
} EpContext;

#endif

