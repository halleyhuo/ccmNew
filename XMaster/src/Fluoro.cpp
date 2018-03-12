#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "Fluoro.h"
#include "eptables.h"

#include "epTable.h"

//#include "XtpCom.h"

FlType FlMode; 
uchar  FlCurverNo;
ushort Dose_pulse; //dose per pulse
ushort kv;
ushort ma;
ushort mas;
ushort Fps; //5, 10, 20, 30, 50, 75, 100, 150 for 0.5, 1, 2, 3, 5, 7.5, 10, and 15 pulse(s)/s; 0 for Continue FL
uchar  focus; // 1 for small focus; 2 for large focus
uchar  RegulationInfo;//0 indicate Reguation Stop. 0xFF indicates always regualtion; other number indicates the regulation Frame count.
ushort ScenseTime;	// Scense of FL, check if IS can realize it or not

#define FPS_LIST_SIZE 8
#define PARALIST_NUMBER 20
//static ushort SelIdx[FPS_LIST_SIZE] =
//{0, 0, 0, 0, 0, 0, 0, 0};

typedef struct 
{
	ushort kv;//0.1 KV
	ushort mas;//0.01 mas
	ushort ma;//0.01ma
	uchar  TMax;//0.1ms
	ushort Fps;//0.1
	uchar  focus;//=1
}FluoroParaGroup;

ushort	g_flFps = 0;



#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE_LINE			1024

#define KEYWORD_CURVE_NUM		"CURVE_NUM"
#define KEYWORD_CURVE_ID		"CURVE_ID"
#define KEYWORD_BPN				"BASE_POINT_NUM"
#define KEYWORD_VAS				"VAS"



static bool LoadCurveSet(const char * fileName, CurveSet * curveSet)
{
	FILE	*fileCurve;
	char	strLine[MAX_SIZE_LINE];
	ushort	curveIndex;
	ushort	basePointIndex;



	if((fileCurve = fopen(fileName, "r")) == NULL)
	{
		//error
		return false;
	}

	curveIndex = 0;

	while(1)
	{
		char		*ret;
		char		*position;
		char		*posStart, *posEnd;
		char		subStr[10];

		ret = fgets(strLine, MAX_SIZE_LINE, fileCurve);
		if(ret == NULL)
			break;

		 if((position = strstr(strLine, KEYWORD_CURVE_NUM)) != NULL)
		 {
		 	// skip keyword and "="
		 	posStart = position + strlen(KEYWORD_CURVE_NUM) + 1;
		 	posEnd = strstr(strLine, ">");

			// get the number
		 	memset(subStr, 0, sizeof(subStr));
		 	strncpy(subStr, posStart, posEnd - posStart);

		 	curveSet->numCurves = atoi(subStr);

		 	continue;
		 }

		 if((position = strstr(strLine, KEYWORD_CURVE_ID)) != NULL)
		 {
			basePointIndex = 0;

		 	// skip keyword and "="
		 	posStart = position + strlen(KEYWORD_CURVE_ID) + 1;
		 	posEnd = strstr(strLine, ",");

		 	// get the number
		 	memset(subStr, 0, sizeof(subStr));
		 	strncpy(subStr, posStart, posEnd - posStart);

		 	curveSet->curveInfo[curveIndex].curveId = atoi(subStr);

		 	// read the number of this base point
		 	position = strstr(strLine, KEYWORD_BPN);
		 	if(position == NULL)
		 	{
		 		// error
		 	}

		 	// skip the keyword and "="
		 	posStart = position + strlen(KEYWORD_BPN) + 1;
			posEnd = strstr(strLine, ">");

			// get the number
			memset(subStr, 0 , sizeof(subStr));
			strncpy(subStr, posStart, posEnd - posStart);
			
		 	curveSet->curveInfo[curveIndex].numBasePoints = atoi(subStr);

		 	curveIndex++;

		 	continue;
		 }

		 if((position = strstr(strLine, KEYWORD_VAS)) != NULL)
		 {

		 	// skip keyword and "="
		 	position += strlen(KEYWORD_VAS);
		 	position++;

			/*
			 * Get KV
			 */
		 	posStart = position;

		 	position = strstr(position, "'");
		 	posEnd = position;

			memset(subStr, 0, sizeof(subStr));
			strncpy(subStr, posStart, posEnd - posStart);

		 	curveSet->curveInfo[curveIndex].baseVasContext[basePointIndex].kv = atoi(subStr);

		 	position++;

		 	/*
		 	 * Get mA
		 	 */
		 	posStart = position;

		 	position = strstr(position, "'");
		 	posEnd = position;

		 	memset(subStr, 0, sizeof(subStr));
		 	strncpy(subStr, posStart, posEnd - posStart);

		 	curveSet->curveInfo[curveIndex].baseVasContext[basePointIndex].ma = atoi(subStr);

			position++;

		 	/*
		 	 * Get ms
		 	 */
		 	posStart = position;

		 	position = strstr(position, ",");
		 	posEnd = position;

		 	memset(subStr, 0, sizeof(subStr));
		 	strncpy(subStr, posStart, posEnd - posStart);

		 	curveSet->curveInfo[curveIndex].baseVasContext[basePointIndex].ms = atoi(subStr);


			basePointIndex++;
		 	continue;
		 }
	}
}


FluoroParaGroup FluoroList[FPS_LIST_SIZE][PARALIST_NUMBER] = 
{
	//Continue Currently doesn't use this Table
	{
		{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},
		{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},
		{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},
		{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1},{400,0,2,0,0,1}
	},

	//Pulse 15 FPS, use 10ms
	{
		{400,2,79, 340,150,1},{402,2,81, 340,150,1},{404,2,83, 340,150,1},{405,2,85, 340,150,1},{407,2,87, 340,150,1},
		{408,2,89, 340,150,1},{409,2,91, 340,150,1},{411,2,93, 340,150,1},{412,2,96, 340,150,1},{414,2,98, 340,150,1},
		{415,2,100,340,150,1},{417,2,102,340,150,1},{419,2,104,340,150,1},{420,2,107,340,150,1},{422,2,110,340,150,1},
		{425,2,112,340,150,1},{428,2,114,340,150,1},{431,2,117,340,150,1},{434,2,120,340,150,1},{437,2,123,340,150,1},
	},

	//Pulse 10 FPS, use 10ms
	{
		{400,2,79, 340,100,1},{402,2,81, 340,100,1},{404,2,83, 340,100,1},{405,2,85, 340,100,1},{407,2,87, 340,100,1},
		{408,2,89, 340,100,1},{409,2,91, 340,100,1},{411,2,93, 340,100,1},{412,2,96, 340,100,1},{414,2,98, 340,100,1},
		{415,2,100,340,100,1},{417,2,102,340,100,1},{419,2,104,340,100,1},{420,2,107,340,100,1},{422,2,110,340,100,1},
		{425,2,112,340,100,1},{428,2,114,340,100,1},{431,2,117,340,100,1},{434,2,120,340,100,1},{437,2,123,340,100,1},
	},
	{
		{400,2,79, 340,75,1},{402,2,81, 340,75,1},{404,2,83, 340,75,1},{405,2,85, 340,75,1},{407,2,87, 340,75,1},
		{408,2,89, 340,75,1},{409,2,91, 340,75,1},{411,2,93, 340,75,1},{412,2,96, 340,75,1},{414,2,98, 340,75,1},
		{415,2,100,340,75,1},{417,2,102,340,75,1},{419,2,104,340,75,1},{420,2,107,340,75,1},{422,2,110,340,75,1},
		{425,2,112,340,75,1},{428,2,114,340,75,1},{431,2,117,340,75,1},{434,2,120,340,75,1},{437,2,123,340,75,1},
	},
	{
		{400,20,79, 340,50,1},{402,20,81, 340,50,1},{404,20,83, 340,50,1},{405,20,85, 340,50,1},{407,20,87, 340,50,1},
		{408,20,89, 340,50,1},{409,20,91, 340,50,1},{411,20,93, 340,50,1},{412,20,96, 340,50,1},{414,20,98, 340,50,1},
		{415,20,100,340,50,1},{417,20,102,340,50,1},{419,20,104,340,50,1},{420,20,107,340,50,1},{422,20,110,340,50,1},
		{425,20,112,340,50,1},{428,20,114,340,50,1},{431,20,117,340,50,1},{434,20,120,340,50,1},{437,20,123,340,50,1},
	},

	{
		{400,2,79, 340,30,1},{402,2,81, 340,30,1},{404,2,83, 340,30,1},{405,2,85, 340,30,1},{407,2,87, 340,30,1},
		{408,2,89, 340,30,1},{409,2,91, 340,30,1},{411,2,93, 340,30,1},{412,2,96, 340,30,1},{414,2,98, 340,30,1},
		{415,2,100,340,30,1},{417,2,102,340,30,1},{419,2,104,340,30,1},{420,2,107,340,30,1},{422,2,110,340,30,1},
		{425,2,112,340,30,1},{428,2,114,340,30,1},{431,2,117,340,30,1},{434,2,120,340,30,1},{437,2,123,340,30,1},
	},
	{
		{400,2,79, 340,20,1},{402,2,81, 340,20,1},{404,2,83, 340,20,1},{405,2,85, 340,20,1},{407,2,87, 340,20,1},
		{408,2,89, 340,20,1},{409,2,91, 340,20,1},{411,2,93, 340,20,1},{412,2,96, 340,20,1},{414,2,98, 340,20,1},
		{415,2,100,340,20,1},{417,2,102,340,20,1},{419,2,104,340,20,1},{420,2,107,340,20,1},{422,2,110,340,20,1},
		{425,2,112,340,20,1},{428,2,114,340,20,1},{431,2,117,340,20,1},{434,2,120,340,20,1},{437,2,123,340,20,1},
	},

	{
		{400,2,79, 340,10,1},{402,2,81, 340,10,1},{404,2,83, 340,10,1},{405,2,85, 340,10,1},{407,2,87, 340,10,1},
		{408,2,89, 340,10,1},{409,2,91, 340,10,1},{411,2,93, 340,10,1},{412,2,96, 340,10,1},{414,2,98, 340,10,1},
		{415,2,100,340,10,1},{417,2,102,340,10,1},{419,2,104,340,10,1},{420,2,107,340,10,1},{422,2,110,340,10,1},
		{425,2,112,340,10,1},{428,2,114,340,10,1},{431,2,117,340,10,1},{434,2,120,340,10,1},{437,1,123,340,10,1},
	}

};

static uchar checkFpsIdx(const ushort fps)
{
	uchar rc = 0xff;
	for(int i=0; i<FPS_LIST_SIZE; i++)
	{
		if(FluoroList[i][0].Fps==fps)
		{
			rc = i;
			break;
		}
	}
	return rc;
}


CFluoro::CFluoro():CIDCThread("Fluoro",IDC_TASK_FLUORO,Small,Moderate,true),m_ActualFluoroMode(FluoroSupportContinue),m_FluoroModeActived(true),m_FpsIdx(0)
{
	m_FlPara[FluoroSupportContinue].FlMode = FlContinue;
	m_FlPara[FluoroSupportContinue].FlCurverNo = 0;//REMARK, currently only support one Fluoro Curve
	m_FlPara[FluoroSupportContinue].DoseLevel = DoseLevelHigh;
	m_FlPara[FluoroSupportContinue].kv        = 400;//start from 40kv
	m_FlPara[FluoroSupportContinue].ma        = 20;
	m_FlPara[FluoroSupportContinue].mas       = 0;//
	m_FlPara[FluoroSupportContinue].Fps       = 0;
	m_FlPara[FluoroSupportContinue].focus       = 1;
	m_FlPara[FluoroSupportContinue].RegulationInfo       = 0;
	m_FlPara[FluoroSupportContinue].ScenseTime       = 0;

	m_KvEpVal[FluoroSupportContinue].curveNo = 0;
	m_KvEpVal[FluoroSupportContinue].LowValue = 0;
	m_KvEpVal[FluoroSupportContinue].HighValue = 85;
	m_KvEpVal[FluoroSupportContinue].ActValue = 0;

	m_FlPara[FluoroSupportPulse].FlMode = FlPulse;
	m_FlPara[FluoroSupportPulse].FlCurverNo = 0;//REMARK, currently only support one Fluoro Curve
	m_FlPara[FluoroSupportPulse].DoseLevel = DoseLevelHigh;
	m_FlPara[FluoroSupportPulse].kv        = 400;//start from 40kv
	m_FlPara[FluoroSupportPulse].ma        = 79;
	m_FlPara[FluoroSupportPulse].mas       = 1;//
	m_FlPara[FluoroSupportPulse].Fps       = 15;
	m_FlPara[FluoroSupportPulse].focus       = 1;
	m_FlPara[FluoroSupportPulse].RegulationInfo       = 0;
	m_FlPara[FluoroSupportPulse].ScenseTime       = 0;

	m_KvEpVal[FluoroSupportPulse].curveNo = 0;
	m_KvEpVal[FluoroSupportPulse].LowValue = 0;
	m_KvEpVal[FluoroSupportPulse].HighValue = 85;
	m_KvEpVal[FluoroSupportPulse].ActValue = 0;

	for(int i=0; i<FluoroSupportCnt; i++)
	{
		m_selParamIdx[i] = 0;//start 40kv
	}


	m_epIndex = 320;
	m_fpsIndex = 0;

	m_genVdData.FlMode	= FlContinue;
	m_genVdData.kv		= epValue[m_epIndex].kv;
	m_genVdData.ma		= epValue[m_epIndex].ma;
	m_genVdData.mas		= 0;
	m_genVdData.Fps		= 0;
	m_genVdData.focus	= 1;
	m_genVdData.Tmax	= 40;


	m_lastestAcqDataIndex = 0;


	m_curvePoints = NULL;

	LoadCurveSet(".\\curves.txt", &m_curveSet);
	
/*
	m_genVdData[0].FlMode	= FlContinue;
	m_genVdData[0].kv		= epValue[m_epIndex].kv;
	m_genVdData[0].ma		= epValue[m_epIndex].ma;
	m_genVdData[0].mas		= 0;
	m_genVdData[0].Fps		= 0;
	m_genVdData[0].focus	= 1;
	m_genVdData[0].Tmax		= 0;

	m_genVdData[1].FlMode	= FlPulse;
	m_genVdData[1].kv		= epValue[m_epIndex].kv;
	m_genVdData[1].ma		= epValue[m_epIndex].ma;
	m_genVdData[1].mas		= epValue[m_epIndex].ms;
	m_genVdData[1].Fps		= 5;
	m_genVdData[1].focus	= 1;
	m_genVdData[1].Tmax		= 0;

	m_genVdData[2].FlMode	= FlSingle;
	m_genVdData[2].kv		= epValue[m_epIndex].kv;
	m_genVdData[2].ma		= epValue[m_epIndex].ma;
	m_genVdData[2].mas		= epValue[m_epIndex].ms;
	m_genVdData[2].Fps		= 0;
	m_genVdData[2].focus	= 1;
	m_genVdData[2].Tmax		= 0;
*/
}

CFluoro::~CFluoro()
{
}

void CFluoro::run()
{
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,">>>INFO: CFluoro Running");
	
	while(true)
	{        
		char buf[MAX_ITC_LENGTH];
		const int rc = m_msgQueue->recv(buf,MAX_ITC_LENGTH);
		if(rc>0)
		{
			ITCObjHeader* p = (ITCObjHeader*)buf;
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Recv Msg Src %d Target %d Len %d ObjID 0x%x",
				p->tidSrc,p->tidTarget,p->len,p->ITCObjectID);
			switch(p->ITCObjectID)
			{
			case INT_GENERAL_CONTROL:
				if(p->len==sizeof(INT_GeneralCtrl_t_Frame))
				{
					INT_GeneralCtrl_t* INT_GeneralCmd = (INT_GeneralCtrl_t*)(p+1);
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Rcv IntGen funcCode %d controlElementType %d",
							INT_GeneralCmd->functionCode,INT_GeneralCmd->controlElementType);
					if(INT_GeneralCmd->functionCode==FuncKVSel && INT_GeneralCmd->controlElementType==PLUS_MINUS_BUTTON)
					{
						SGENData		sGenData;
						ushort			tmpEpIndex;


						tmpEpIndex = m_epIndex;

						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"INT_GENERAL_CONTROL epIndex:%d",tmpEpIndex);

						if(UICMD_PARAM_PLS == INT_GeneralCmd->param)
						{
							tmpEpIndex += 4;
							if(tmpEpIndex >= sizeof(epValue)/sizeof(EpValue) - 1)
							{
								tmpEpIndex = sizeof(epValue)/sizeof(EpValue) - 1;
							}
/*
							if(tmpEpIndex >= sizeof(epValue)/sizeof(EpValue) - 1) // max value
							{
								break;
							}
							tmpEpIndex++;
*/
						}
						else
						{
							if(tmpEpIndex < 4)
							{
								tmpEpIndex = 0;
							}
							else
							{
								tmpEpIndex -= 4;
							}
/*
							if(tmpEpIndex == 0) // min value
							{
								break;
							}
							tmpEpIndex--;
*/
						}

						sGenData.FlMode = m_genVdData.FlMode;
						sGenData.kv		= epValue[tmpEpIndex].kv;
						sGenData.ma		= epValue[tmpEpIndex].ma;
						sGenData.mas	= (sGenData.FlMode == FlContinue) ? 0 : (epValue[tmpEpIndex].ma * epValue[tmpEpIndex].ms) / 1000;
						sGenData.Tmax	= (sGenData.FlMode == FlContinue) ? 0 : epValue[tmpEpIndex].ms;
						sGenData.focus	= m_genVdData.focus;
						sGenData.Fps	= (sGenData.FlMode == FlContinue) ? 0 : fpsValue[m_fpsIndex];

						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Halley - Prepare data for send flmode:%d,  kv:%d, ma:%d, mas:%d, fps:%d",
								sGenData.FlMode, sGenData.kv, sGenData.ma, sGenData.mas, sGenData.Fps);

						if(CXtpCom::Sigleton()!=NULL)
						{
							CXtpCom::Sigleton()->onSGendata(&sGenData);
						}

					/*
						SGENData SGenData;
						SGenData.FlMode = m_FlPara[m_ActualFluoroMode].FlMode;
						if(m_ActualFluoroMode==FluoroSupportContinue)
						{
							if(UICMD_PARAM_PLS==INT_GeneralCmd->param && m_KvEpVal[FluoroSupportContinue].ActValue<m_KvEpVal[FluoroSupportContinue].HighValue)
							{
								m_KvEpVal[FluoroSupportContinue].ActValue++;
							}
							else if(UICMD_PARAM_MINUS==INT_GeneralCmd->param && m_KvEpVal[FluoroSupportContinue].ActValue>m_KvEpVal[FluoroSupportContinue].LowValue)
							{
								m_KvEpVal[FluoroSupportContinue].ActValue--;
							}							
							
							m_FlPara[FluoroSupportContinue].kv = kv_bp_tab.kv[m_KvEpVal[FluoroSupportContinue].ActValue];
							m_FlPara[FluoroSupportContinue].ma = GetLinerVal(m_FlPara[FluoroSupportContinue].kv,0);
							SGenData.kv = m_FlPara[m_ActualFluoroMode].kv;
							SGenData.ma = m_FlPara[m_ActualFluoroMode].ma;
							SGenData.mas = 0;
							SGenData.Tmax = 0;
							SGenData.Fps = 0;
							SGenData.focus = 1;//REMARK, use small focus
						}
						else
						{
							if(UICMD_PARAM_PLS==INT_GeneralCmd->param)
							{
								if(m_selParamIdx[m_ActualFluoroMode]<PARALIST_NUMBER-1)
								{
									m_selParamIdx[m_ActualFluoroMode]++;
								}
									
							}
							else
							{
								if(m_selParamIdx[m_ActualFluoroMode]>0)
								{
									m_selParamIdx[m_ActualFluoroMode]--;
								}
							}							
							SGenData.kv = FluoroList[m_FpsIdx][m_selParamIdx[m_ActualFluoroMode]].kv;
							SGenData.ma = FluoroList[m_FpsIdx][m_selParamIdx[m_ActualFluoroMode]].ma;
							SGenData.mas = FluoroList[m_FpsIdx][m_selParamIdx[m_ActualFluoroMode]].mas;
							SGenData.Tmax = FluoroList[m_FpsIdx][m_selParamIdx[m_ActualFluoroMode]].TMax;
							SGenData.Fps = FluoroList[m_FpsIdx][m_selParamIdx[m_ActualFluoroMode]].Fps;
							SGenData.focus = 1;//REMARK, use small focus
						}
						if(CXtpCom::Sigleton()!=NULL)
						{			

							CXtpCom::Sigleton()->onSGendata(&SGenData);
						}
					*/

						
					}
					else if(INT_GeneralCmd->functionCode==FuncAcqFluoro && INT_GeneralCmd->controlElementType==ON_OFF_LEVEL_BUTTON)
					{
						if(m_FluoroModeActived)
						{
							SFlCmd sflCmd = {FLCmdActive,0,m_FlPara[m_ActualFluoroMode].Fps};
							if(INT_GeneralCmd->param==ON_OFF_LEVEL_PRESS)//$FluoroACQ Process  Step 2[1], Fluoro Process
							{
								//Produce SFLCMD(Active) to Generate
								sflCmd.cmd = FLCmdActive;								
							}
							else//$FluoroACQ Process  Step 6, Fluoro Process Finish
							{
								//Produce SFLCMD(Stop) to Generate
								sflCmd.cmd = FLCmdStop;
							}
							CXtpCom::Sigleton()->onFlCmd(&sflCmd);
						}
					}
					else if(INT_GeneralCmd->functionCode==FuncFlFPS && INT_GeneralCmd->controlElementType==PLUS_MINUS_BUTTON)
					{
						ushort		tmpFpsIndex;

						tmpFpsIndex = (m_fpsIndex + 1) % (sizeof(fpsValue)/sizeof(ushort));

						
						if(m_curvePoints)
						{
							delete m_curvePoints;
						}
						m_curvePoints = new CurvePoints(m_curveSet.curveInfo[0].numBasePoints, (const EpContext*)m_curveSet.curveInfo[0].baseVasContext, EpAccuracy16_1);

						if(CXtpCom::Sigleton() != NULL)
						{
							SGENData	sGenData;

							sGenData.FlMode		= (fpsValue[tmpFpsIndex] == 0) ? FlContinue : FlPulse;
							sGenData.kv			= epValue[m_epIndex].kv;
							sGenData.ma			= epValue[m_epIndex].ma;
							sGenData.mas		= (sGenData.FlMode == FlContinue) ? 0 : (epValue[m_epIndex].ma * epValue[m_epIndex].ms) / 1000;
							sGenData.Fps		= (sGenData.FlMode == FlContinue) ? 0 : fpsValue[tmpFpsIndex];
							sGenData.Tmax		= (sGenData.FlMode == FlContinue) ? 0 : epValue[m_epIndex].ms;
							sGenData.focus		= 1;

							CXtpCom::Sigleton()->onSGendata(&sGenData);
						}
/*
						if(m_FluoroModeActived)
						{
							const uchar Table1stIdx = (m_FpsIdx+1)%FPS_LIST_SIZE;
							IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Table1stIdx:%d,m_FpsIdx:%d\n",Table1stIdx,m_FpsIdx);
							if(CXtpCom::Sigleton()!=NULL)
							{
								SGENData SGenData;
								SGenData.FlMode = (Table1stIdx==0)?FlContinue:FlPulse;
								if(SGenData.FlMode==FlContinue)
								{
									SGenData.kv = m_FlPara[FluoroSupportContinue].kv;
									SGenData.ma = m_FlPara[m_ActualFluoroMode].ma;
									SGenData.mas = 0;
									SGenData.Fps = 0;
									SGenData.Tmax = 0;
								}
								else
								{
									SGenData.kv = FluoroList[Table1stIdx][0].kv;
									SGenData.ma = FluoroList[Table1stIdx][0].ma;
									SGenData.mas = FluoroList[Table1stIdx][0].mas;
									SGenData.Fps = FluoroList[Table1stIdx][0].Fps;
									SGenData.Tmax =  FluoroList[Table1stIdx][0].TMax;
								}								
								
								SGenData.focus = 1;//REMARK, use small focus
								CXtpCom::Sigleton()->onSGendata(&SGenData);
							}
						}	
*/
					}
					else if(INT_GeneralCmd->functionCode==FuncSingleMode && INT_GeneralCmd->controlElementType==PLUS_MINUS_BUTTON)
					{
						SGENData			sGenData;

						sGenData.FlMode		= FlSingle;
						sGenData.kv			= epValue[m_epIndex].kv;
						sGenData.ma			= epValue[m_epIndex].ma;
						sGenData.mas		= (epValue[m_epIndex].ma * epValue[m_epIndex].ms) / 1000;
						sGenData.Fps		= fpsValue[m_fpsIndex];
						sGenData.Tmax		= (epValue[m_epIndex].ms/10); //  m_genVdData.Tmax;
						sGenData.focus		= m_genVdData.focus;

						if(CXtpCom::Sigleton() != NULL)
						{
							CXtpCom::Sigleton()->onSGendata(&sGenData);
						}

					/*
						if(m_FluoroModeActived)
						{
							m_FluoroModeActived = false;

							const uchar Table1stIdx = (m_FpsIdx+1)%FPS_LIST_SIZE;
							
							if(CXtpCom::Sigleton()!=NULL)
							{
								SGENData		sGenData;

								sGenData.FlMode		= FlSingle;
								sGenData.kv			= FluoroList[Table1stIdx][0].kv;
								sGenData.ma			= FluoroList[Table1stIdx][0].ma;
								sGenData.mas		= FluoroList[Table1stIdx][0].mas;
								sGenData.Fps		= FluoroList[Table1stIdx][0].Fps;
								sGenData.Tmax		= FluoroList[Table1stIdx][0].TMax;
								sGenData.focus		= 1;

								CXtpCom::Sigleton()->onSGendata(&sGenData);
							}
						}
					*/
					}
					else if(INT_GeneralCmd->functionCode==FuncFluoroMode && INT_GeneralCmd->controlElementType==PLUS_MINUS_BUTTON)
					{
						SGENData			sGenData;

						sGenData.FlMode		= (fpsValue[m_fpsIndex] == 0) ? FlContinue : FlPulse;
						sGenData.kv			= epValue[m_epIndex].kv;
						sGenData.ma			= epValue[m_epIndex].ma;
						sGenData.mas		= (sGenData.FlMode == FlContinue) ? 0 : (epValue[m_epIndex].ma * epValue[m_epIndex].ms) / 1000;
						sGenData.Fps		= (sGenData.FlMode == FlContinue) ? 0 : fpsValue[m_fpsIndex];
						sGenData.Tmax		= (sGenData.FlMode == FlContinue) ? 0 : (epValue[m_epIndex].ms);
						sGenData.focus		= m_genVdData.focus;

						if(CXtpCom::Sigleton() != NULL)
						{
							CXtpCom::Sigleton()->onSGendata(&sGenData);
						}

						
					/*
						if(!m_FluoroModeActived)
						{
							m_FluoroModeActived = true;

							const uchar Table1stIdx = m_FpsIdx;

							if(CXtpCom::Sigleton()!=NULL)
							{
								SGENData SGenData;
								SGenData.FlMode = (Table1stIdx==0)?FlContinue:FlPulse;
								if(SGenData.FlMode==FlContinue)
								{
									SGenData.kv = m_FlPara[FluoroSupportContinue].kv;
									SGenData.ma = m_FlPara[m_ActualFluoroMode].ma;
									SGenData.mas = 0;
									SGenData.Fps = 0;
									SGenData.Tmax = 0;
								}
								else
								{
									SGenData.kv = FluoroList[Table1stIdx][0].kv;
									SGenData.ma = FluoroList[Table1stIdx][0].ma;
									SGenData.mas = FluoroList[Table1stIdx][0].mas;
									SGenData.Fps = FluoroList[Table1stIdx][0].Fps;
									SGenData.Tmax =  FluoroList[Table1stIdx][0].TMax;
								}								
								
								SGenData.focus = 1;//REMARK, use small focus
								CXtpCom::Sigleton()->onSGendata(&SGenData);
							}
						}
						*/
					}
				}
				break;

			case INT_GEN_STATUS://produce SFLCMD to GeneratorVD & ImageStation $FluoroACQ Process  Step 4, Fluoro Process
			{
				if(p->len == sizeof(IntGenStatusFrame))
				{
					INT_GenStatus_t* data = (INT_GenStatus_t*)(p+1);
					if(data->IGenInfo.GeneratorReady==GeneratorReadyforAcq && data->IGenInfo.TubeStatus==TubeStatusWork &&data->IGenInfo.DoorStatus==DoorStatusOK)
					{
						//Produce SFLCMD(Start) to Generate  FluoroList
//						SFlCmd sflCmd = {FLCmdStart,0,m_FlPara[m_ActualFluoroMode].Fps};
						SFlCmd sflCmd = {FLCmdStart,0,FluoroList[m_FpsIdx][0].Fps};
						CXtpCom::Sigleton()->onFlCmd(&sflCmd,true);
					}					
				}
			}
				break;

			case INT_GEN_ACQ:
			{
				if(p->len == sizeof(IntGenAcqFrame))
				{
					INT_GenAcq_t	*data = (INT_GenAcq_t *)(p + 1);
					ushort			kv, ma, ms;

					// insert to acq data buffer
					memcpy(&m_genAcqData[m_lastestAcqDataIndex], data, sizeof(INT_GenAcq_t));
					kv = m_genAcqData[m_lastestAcqDataIndex].kv;
					ma = m_genAcqData[m_lastestAcqDataIndex].ma;
					ms = m_genAcqData[m_lastestAcqDataIndex].ms;
//					m_genAcqData[m_lastestAcqDataIndex].epValue = GetKvEp(kv) + GetMaEp(ma) + GetMsEp(ms);
					/*
					m_lastestAcqDataIndex++;
					if(m_lastestAcqDataIndex >= ACQ_DATA_BUFFER_NUM)
					{
						m_lastestAcqDataIndex = 0;
					}
					*/
				}
			}
			break;

			case INT_IWS_DOSE_INFO:
			{

				if(p->len == sizeof(IntIwsDoseInfoFrame))
				{
					INT_IwsDoseInfo_t	*data = (INT_IwsDoseInfo_t *)(p + 1);
					ushort				curDose = data->doseValue;
					int					deltaDose;
					int					adjustEp100;
					int					step;
					ushort				tempEpIndex;

					deltaDose = m_targetDose - curDose;

					if(deltaDose * 100 / m_targetDose < -6 && deltaDose * 100 / m_targetDose > 6)
					{
						if(deltaDose * 100 / curDose > 100 || deltaDose * 100 / curDose < -100)
						{
							adjustEp100 = 300;
						}
						else if(deltaDose * 100 / curDose > -25 && deltaDose * 100 / curDose < 25)
						{
							adjustEp100 = 25;
						}
						else
						{
							adjustEp100 = deltaDose * 100 / curDose;
						}

						step = adjustEp100 * 100 / 625;		// 625 = 1/16 * 100 * 100
/*
						tempEpIndex = GetEpIndex(fpsValue[m_fpsIndex], 
												m_genAcqData[m_lastestAcqDataIndex].kv,
												m_genAcqData[m_lastestAcqDataIndex].ma,
												m_genAcqData[m_lastestAcqDataIndex].ms);
*/
						if(step > 0)
						{
							tempEpIndex += step;
							if(tempEpIndex >= sizeof(epValue)/sizeof(EpValue) - 1)
							{
								tempEpIndex = sizeof(epValue)/sizeof(EpValue) - 1;
							}
						}
						else if(step < 0)
						{
							if(tempEpIndex < 0 - step)
							{
								tempEpIndex = 0;
							}
							else
							{
								tempEpIndex += step;
							}
						}
						else // step == 0, error
						{
							
						}


						SGENData			sGenData;

						sGenData.FlMode		= (fpsValue[m_fpsIndex] == 0) ? FlContinue : FlPulse;
						sGenData.kv			= epValue[tempEpIndex].kv;
						sGenData.ma			= epValue[tempEpIndex].ma;
						sGenData.mas		= (sGenData.FlMode == FlContinue) ? 0 : (epValue[tempEpIndex].ma * epValue[tempEpIndex].ms) / 1000;
						sGenData.Fps		= (sGenData.FlMode == FlContinue) ? 0 : fpsValue[m_fpsIndex];
						sGenData.Tmax		= (sGenData.FlMode == FlContinue) ? 0 : (epValue[tempEpIndex].ms);
						sGenData.focus		= m_genVdData.focus;

						if(CXtpCom::Sigleton() != NULL)
						{
							CXtpCom::Sigleton()->onSGendata(&sGenData);
						}
					}
				}

			}
			break;

			case INT_FL_DATA:
				if(p->len==sizeof(INT_FlData_Frame))
				{
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG," Recv Msg Src %d Target %d Len %d ObjID 0x%x mode %d",
								p->tidSrc,p->tidTarget,p->len,p->ITCObjectID,m_ActualFluoroMode);

					INT_FlData_t* INT_FlData = (INT_FlData_t*)(p+1);
					iFlData ifldata;
					
					//memcpy(&m_genVdData, INT_FlData, sizeof(INT_FlData_t));
					
					//SyncDataIndex();
					if(INT_FlData->FlMode == FlSingle)
					{

						m_genVdData.FlMode = FlSingle;

						ushort		index;

						for(index = 0; index < sizeof(epValue)/sizeof(EpValue); index++)
						{
							if(epValue[index].kv == INT_FlData->kv &&
								epValue[index].ma == INT_FlData->ma)
							{
								break;
							}
						}

						m_epIndex = index;
					}
					else if(INT_FlData->FlMode == FlContinue)
					{
						m_fpsIndex = 0;
						m_genVdData.FlMode = FlContinue;

						ushort		index;

						for(index = 0; index < sizeof(epValue)/sizeof(EpValue); index++)
						{
							if(epValue[index].kv == INT_FlData->kv &&
								epValue[index].ma == INT_FlData->ma )
							{
								break;
							}
						}

						m_epIndex = index;
					}
					else //Pulse
					{
						m_genVdData.FlMode = FlPulse;

						ushort		index;

						for(index = 0; index < sizeof(fpsValue)/sizeof(ushort); index++)
						{
							if(fpsValue[index] == INT_FlData->Fps)
							{
								break;
							}
						}
						m_fpsIndex = index;

						for(index = 0; index < sizeof(epValue)/sizeof(EpValue); index++)
						{
							if(epValue[index].kv == INT_FlData->kv &&
								epValue[index].ma == INT_FlData->ma)
							{
								break;
							}
						}

						m_epIndex = index;
					}

					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"INT_FL_DATA FlMode:%d, kv:%d, ma:%d, mas:%d, fps:%d, epIndex:%d, fpsIndex:%d",
							INT_FlData->FlMode, INT_FlData->kv, INT_FlData->ma, INT_FlData->mas, INT_FlData->Fps, m_epIndex, m_fpsIndex );

					ifldata.FlMode			= INT_FlData->FlMode;
					ifldata.FlCurverNo		= 0;
					ifldata.DoseLevel		= 0;
					ifldata.kv				= INT_FlData->kv;
					ifldata.ma				= INT_FlData->ma;
					ifldata.mas 			= INT_FlData->mas;
					ifldata.Fps 			= INT_FlData->Fps;
					ifldata.focus			= INT_FlData->focus;
					ifldata.RegulationInfo	= 0;
					ifldata.ScenseTime		= 0;

/*
					if(INT_FlData->FlMode == FlSingle)
					{
						ifldata.FlMode			= INT_FlData->FlMode;
						ifldata.FlCurverNo		= 0;
						ifldata.DoseLevel		= 0;
						ifldata.kv				= INT_FlData->kv;
						ifldata.ma				= INT_FlData->ma;
						ifldata.mas				= INT_FlData->mas;
						ifldata.Fps				= INT_FlData->Fps;
						ifldata.focus			= INT_FlData->focus;
						ifldata.RegulationInfo	= 0;
						ifldata.ScenseTime		= 0;
					}
					else
					{
						m_ActualFluoroMode = (INT_FlData->FlMode==FlContinue)?FluoroSupportContinue:FluoroSupportPulse;
						const uchar fpsIdx = checkFpsIdx(INT_FlData->Fps);
						g_flFps = INT_FlData->Fps;
						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"RfpsIdx:%d,fps:%d",fpsIdx,INT_FlData->Fps);
						if(fpsIdx!=0xff)
						{  
							m_FpsIdx = fpsIdx;
						}
						ifldata.FlMode			= INT_FlData->FlMode;
						ifldata.FlCurverNo		= m_FlPara[m_ActualFluoroMode].FlCurverNo;
						ifldata.DoseLevel		= m_FlPara[m_ActualFluoroMode].DoseLevel;
						ifldata.kv				= INT_FlData->kv;
						ifldata.ma				= INT_FlData->ma;
						ifldata.mas				= INT_FlData->mas;
						ifldata.Fps				= INT_FlData->Fps;
						ifldata.focus			= m_FlPara[m_ActualFluoroMode].focus;
						ifldata.RegulationInfo	= m_FlPara[m_ActualFluoroMode].RegulationInfo;
						ifldata.ScenseTime		= 0;
					}
*/
					CXtpCom::Sigleton()->onAcqUiShow(&ifldata); 				
				}
				else
				{
					IDC::Log(IDC::LOG_WARNING,LOG_TAG,"ILLEGAL Len %d ObjID 0x%x",p->len,INT_FL_DATA);
				}
				break;

			
			default:
				break;
			}               
		}
		else
		{
			IDC::Log(IDC::LOG_ERR,LOG_TAG,"Recv Msg Fail %d err %d",rc, m_msgQueue->GetIDCErrno());
		}
	}
}

//kv unit is 0.1kv
ushort CFluoro::GetLinerVal(const ushort kv, const ushort FlFps)
{
	ushort rc = 0xFFFF;
	switch(FlFps)
	{
		case 0://continue, result is ma, unit is 0.01 ma. 0.2ma~4ma
		{
			float maVal = 0.0535*kv-19.408;//unit is 0.1ma because unit of KV is 0.1KV
			rc = ushort(maVal*10);//unit change to 0.01ma
		}
		break;

		default:
		break;
	}
	return rc;
}

void CFluoro::SyncDataIndex(void)
{
	ushort			index;

	for(index = 0; index < sizeof(fpsValue); index++)
	{
	}
}

