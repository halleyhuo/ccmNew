/**********************************************************************
 *
 * Coypright(c), 2015-2017, IDC
 *
 * FileName:
 *		curve.h
 *
 * Description:
 *		Class curve 
 *
 * Date:
 *		2017-11-13
 *
 * Author:
 *		Halley
 *
 */


#ifndef __CURVE_H__
#define __CURVE_H__

#include "epType.h"

#define MAX_NUM_CURVES				4
#define MAX_NUM_POINTS				4
#define MAX_NUM_EP_CONTEXT			1000

#define INVALID_EP_KV				0xFFFF
#define INVALID_EP_MA				0xFFFF
#define INVALID_EP_MS				0xFFFF

#define INVALID_EP_INDEX			0xFFFF


#define EP_STEP						(1/16)		// 1/16


class CurvePoints
{

public:
	CurvePoints();
	CurvePoints(ushort numOfBasePoints, const EpContext *baseEpContext, EpAccuracy epAccuracy);
	virtual ~CurvePoints();


	sshort		InitCurve(ushort numOfPoints, const EpContext *baseEpContext, EpAccuracy epAcurracy);
	EpContext	EpContextValue(ushort epIndex);
	ushort		EpIndex(const EpContext &epContext);

public:
	ushort		m_numOfPoints;
	EpContext	m_epContexts[MAX_NUM_EP_CONTEXT];

private:
	ushort		m_numOfBasePoints;
	EpContext	m_baseEpContext[MAX_NUM_POINTS];
	EpAccuracy	m_epAccuracy;
	
};

#endif


