
#ifndef __CURVES_H__
#define __CURVES_H__

#include "epType.h"

typedef struct _CurvesType
{
	ushort		fpsIndex;
	ushort		
}CurvesType;

class Curves
{
public:
	Curves(const char *fileName);

	/**
	 * @Description:
	 *	Get base points for the curve
	 *
	 * @Param:
	 *	fpsIndex - for FPS
	 *	curveIndex - for selection one type of curves
	 *	epBaseContexts - get the base EP contexts of the selection
	 *
	 * @return:
	 *	The number of base EP context. 0 for error
	 *
	 */
	ushort GetBasePoints(ushort fpsIndex, ushort curveIndex, EpContext *epBaseContexts);

private:
	EpContext	m_epContext[];
};
#endif

