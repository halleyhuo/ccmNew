/**********************************************************************
 *
 * Coypright(c), 2015-2017, IDC
 *
 * FileName:
 *		curve.cpp
 *
 * Description:
 *		Class GenVdCmdData 
 *
 * Date:
 *		2017-11-13
 *
 * Author:
 *		Halley
 *
 */


#include "IDCType.h"
#include <stdlib.h>
#include "CurvePoints.h"
#include "eptables.h"

/******************************************************************
 *
 * Internal defines
 *
 */
#define INVALID_FPS_VALUE		0xFFFF
#define INVALID_CURVE_INDEX		0xFFFF


#define MULTI_RATIO				1000


/******************************************************************
 *
 * Internal varibles
 *
 */



/******************************************************************
 *
 * Internal functions
 *
 */
#define PARAM_OUT
#define PARAM_IN

/*
 * kv = kv_org * 100
 * ma = ma_org * 1000
 * ms = ms_org * 1000
 * ep = ep_org * 10000
 */

static ushort CalculateEpValus(PARAM_IN const EpContext *epContext1, PARAM_IN const EpContext *epContext2, PARAM_IN EpAccuracy epAccuracy, PARAM_OUT EpContext *epContexts)
{
	ulong			epValueKv1, epValueMa1, epValueMs1, epValueKv2, epValueMa2, epValueMs2;
	slong			deltaKv, deltaMa, deltaMs;
	slong			deltaEpValue, deltaEpValueKv, deltaEpValueMa, deltaEpValueMs;
	slong			ratioEpKv, ratioEpMa, ratioEpMs;
	ulong			kvs[2], mas[2], mss[2];
	ushort			multi;
	sshort			slopeKvMa;
	ushort			count = 0;



	epValueKv1 = EpTables::EpValueKv(epContext1->kv, epAccuracy);
	epValueKv2 = EpTables::EpValueKv(epContext2->kv, epAccuracy);

	epValueMa1 = EpTables::EpValueMa(epContext1->ma, epAccuracy);
	epValueMa2 = EpTables::EpValueMa(epContext2->ma, epAccuracy);

	epValueMs1 = EpTables::EpValueMs(epContext1->ms, epAccuracy);
	epValueMs2 = EpTables::EpValueMs(epContext2->ms, epAccuracy);

	deltaKv = epContext2->kv - epContext1->kv;
	deltaMa = epContext2->ma - epContext1->ma;
	deltaMs = epContext2->ms - epContext1->ms;

	deltaEpValueKv = epValueKv2 - epValueKv1;
	deltaEpValueMa = epValueMa2 - epValueMa1;
	deltaEpValueMs = epValueMs2 - epValueMs1;
	deltaEpValue = deltaEpValueKv + deltaEpValueMa + deltaEpValueMs;

	ratioEpKv = (MULTI_RATIO * deltaEpValueKv) / deltaEpValue;
	ratioEpMa = (MULTI_RATIO * deltaEpValueMa) / deltaEpValue;
	ratioEpMs = (MULTI_RATIO * deltaEpValueMs) / deltaEpValue;

	switch(epAccuracy)
	{
		case EpAccuracy16_1:
			multi = 625;
		break;			


		default:
		break;
	}


	slopeKvMa = (deltaKv * MULTI_RATIO) / deltaMa;		// slope of kv:ma




	slong			tempDeltaEpValueKv, tempDeltaEpValueMa, tempDeltaEpValueMs;
	ulong			tempEpValueKv, tempEpValueMa, tempEpValueMs;
	ulong			tempKv, tempMa, tempMs;
	ushort			i = 1;
	ulong			tempDiffEpValueKv0, tempDiffEpValueKv1, tempDiffEpValueMa0, tempDiffEpValueMa1, tempDiffEpValueMs0, tempDiffEpValueMs1;

	
	while(1)
	{
		tempDeltaEpValueKv = (ratioEpKv * multi * i) / MULTI_RATIO;
		tempDeltaEpValueMa = (ratioEpMa * multi * i) / MULTI_RATIO;
		tempDeltaEpValueMs = (ratioEpMs * multi * i) / MULTI_RATIO;

		tempEpValueKv = epValueKv1 + tempDeltaEpValueKv;
		tempEpValueMa = epValueMa1 + tempDeltaEpValueMa;
		tempEpValueMs = epValueMs1 + tempDeltaEpValueMs;

		/*
		 */
		EpTables::GetKvFromEp(tempEpValueKv, kvs, epAccuracy);
		EpTables::GetMaFromEp(tempEpValueMa, mas, epAccuracy);
		EpTables::GetMsFromEp(tempEpValueMs, mss, epAccuracy);

		tempDiffEpValueKv0 = abs(tempEpValueKv - EpTables::EpValueKv(kvs[0], epAccuracy));
		tempDiffEpValueKv1 = abs(tempEpValueKv - EpTables::EpValueKv(kvs[1], epAccuracy));
		
		tempDiffEpValueMa0 = abs(tempEpValueMa - EpTables::EpValueMa(mas[0], epAccuracy));
		tempDiffEpValueMa1 = abs(tempEpValueMa - EpTables::EpValueMa(mas[1], epAccuracy));

		tempDiffEpValueMs0 = abs(tempEpValueMs - EpTables::EpValueMs(mss[0], epAccuracy));
		tempDiffEpValueMs1 = abs(tempEpValueMs - EpTables::EpValueMs(mss[1], epAccuracy));

		tempKv = tempDiffEpValueKv0 < tempDiffEpValueKv1 ? kvs[0] : kvs[1];
		tempMa = tempDiffEpValueMa0 < tempDiffEpValueMa1 ? mas[0] : mas[1];
		tempMs = tempDiffEpValueMs0 < tempDiffEpValueMs1 ? mss[0] : mss[1];

		if((deltaKv > 0 && deltaMa > 0 && (tempKv > epContext2->kv /*|| tempMa > epContext2->ma*/)) ||
			(deltaKv == 0 && deltaMa > 0 && tempMa > epContext2->ma) ||
			(deltaKv > 0 && deltaMa < 0 && (tempKv > epContext2->kv /*|| tempMa < epContext2->ma*/)))
		{
			count = i-1;
			break;
		}
		else
		{
			epContexts[i-1].kv = tempKv;
			epContexts[i-1].ma = tempMa;
			epContexts[i-1].ms = tempMs;
		}

		i++;
	}

	return count;
}

/******************************************************************
 *
 * Class CurvePoints
 *
 */

CurvePoints::CurvePoints()
{
	memset(m_epContexts, 0, sizeof(EpContext)*MAX_NUM_EP_CONTEXT);
	m_numOfPoints = 0;
}


CurvePoints::CurvePoints(ushort numOfPoints, const EpContext * baseEpContexts, EpAccuracy epAccuracy)
{
	memset(m_epContexts, 0, sizeof(EpContext)*MAX_NUM_EP_CONTEXT);
	m_numOfPoints = 0;


	InitCurve(numOfPoints, baseEpContexts, epAccuracy);
}

CurvePoints::~CurvePoints()
{

}

sshort CurvePoints::InitCurve(ushort numOfPoints, const EpContext * baseEpContexts, EpAccuracy epAccuracy)
{
	ushort			index;
	ushort			position = 0;
	ushort			count = 0;

	if(numOfPoints < 2 || baseEpContexts == NULL)
	{
		return -1;		/* Invalid params */
	}

	/*
	 * Save the parameters
	 */
	m_numOfBasePoints = numOfPoints;

	for(ushort i = 0; i < numOfPoints; i++)
	{
		memcpy(&m_baseEpContext[i], &baseEpContexts[i], sizeof(EpContext));
	}

	m_epAccuracy = epAccuracy;

	/*
	 * Calculate the pointers
	 */
	for(index = 1; index < numOfPoints; index++)
	{

		count = CalculateEpValus(&baseEpContexts[index - 1], &baseEpContexts[index], epAccuracy, &m_epContexts[position]);
		position += count;
	}

	m_numOfPoints = position;

	return 0;
}

EpContext CurvePoints::EpContextValue(ushort epIndex)
{
	return m_epContexts[epIndex];
}

ushort CurvePoints::EpIndex(const EpContext &epContext)
{
	ushort			index = 0;

	for(index = 0; index < MAX_NUM_EP_CONTEXT; index++)
	{
		if(epContext.kv == m_epContexts[index].kv
			&& epContext.ma == m_epContexts[index].ma
			&& epContext.ms == m_epContexts[index].ms)
		{
			break;
		}
	}

	if(index < MAX_NUM_EP_CONTEXT)
	{
		return index;
	}

	return INVALID_EP_INDEX;
}



