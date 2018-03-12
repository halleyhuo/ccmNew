
#ifndef __EP_TABLE_H__
#define __EP_TABLE_H__


const ushort fpsValue[] =
{
	//5, 10, 20, 30, 50, 75, 100, 150 for 0.5, 1, 2, 3, 5, 7.5, 10, and 15 pulse(s)/s; 0 for Continue FL
	0,
	5,
	10,
	20,
	30,
	50,
	75,
	100,
	150
};

typedef struct _EpValue
{
	ushort		kv;
	ushort		ma;
	ushort		ms;
} EpValue;


const EpValue epValue[] = 
{
	{400	,200	,100},
	{401	,200	,100},
	{402	,200	,100},
	{402	,202	,100},
	{402	,202	,100},
	{403	,202	,100},
	{404	,202	,100},
	{404	,202	,100},
	{404	,205	,100},
	{405	,205	,100},
	{405	,205	,100},
	{406	,205	,100},
	{407	,205	,100},
	{407	,208	,100},
	{407	,208	,100},
	{408	,208	,100},
	{409	,208	,100},
	{409	,208	,100},
	{409	,211	,100},
	{410	,211	,100},
	{410	,211	,100},
	{411	,211	,100},
	{412	,211	,100},
	{412	,214	,100},
	{412	,214	,100},
	{413	,214	,100},
	{414	,214	,100},
	{414	,214	,100},
	{414	,217	,100},
	{415	,217	,100},
	{415	,217	,100},
	{416	,217	,100},
	{417	,217	,100},
	{417	,221	,100},
	{417	,221	,100},
	{418	,221	,100},
	{419	,221	,100},
	{419	,221	,100},
	{419	,224	,100},
	{420	,224	,100},
	{420	,224	,100},
	{421	,224	,100},
	{421	,227	,100},
	{423	,227	,100},
	{424	,227	,100},
	{425	,227	,100},
	{426	,227	,100},
	{426	,230	,100},
	{428	,230	,100},
	{429	,230	,100},
	{430	,230	,100},
	{431	,230	,100},
	{431	,234	,100},
	{433	,234	,100},
	{434	,234	,100},
	{435	,234	,100},
	{436	,234	,100},
	{436	,237	,100},
	{438	,237	,100},
	{439	,237	,100},
	{440	,237	,100},
	{441	,237	,100},
	{441	,241	,100},
	{443	,241	,100},
	{444	,241	,100},
	{445	,241	,100},
	{446	,241	,100},
	{446	,244	,100},
	{448	,244	,100},
	{449	,244	,100},
	{450	,244	,100},
	{451	,244	,100},
	{451	,248	,100},
	{453	,248	,100},
	{454	,248	,100},
	{455	,248	,100},
	{456	,248	,100},
	{456	,251	,100},
	{458	,251	,100},
	{459	,251	,100},
	{460	,251	,100},
	{461	,251	,100},
	{461	,255	,100},
	{463	,255	,100},
	{464	,255	,100},
	{465	,255	,100},
	{466	,255	,100},
	{466	,259	,100},
	{468	,259	,100},
	{469	,259	,100},
	{470	,259	,100},
	{471	,259	,100},
	{471	,262	,100},
	{473	,262	,100},
	{474	,262	,100},
	{475	,262	,100},
	{476	,262	,100},
	{476	,266	,100},
	{478	,266	,100},
	{479	,266	,100},
	{480	,266	,100},
	{481	,266	,100},
	{481	,270	,100},
	{483	,270	,100},
	{484	,270	,100},
	{485	,270	,100},
	{486	,270	,100},
	{486	,274	,100},
	{488	,274	,100},
	{489	,274	,100},
	{490	,274	,100},
	{491	,274	,100},
	{491	,278	,100},
	{493	,278	,100},
	{494	,278	,100},
	{495	,278	,100},
	{496	,278	,100},
	{496	,282	,100},
	{498	,282	,100},
	{499	,282	,100},
	{500	,282	,100},
	{500	,286	,100},
	{501	,286	,100},
	{503	,286	,100},
	{504	,286	,100},
	{505	,286	,100},
	{505	,290	,100},
	{506	,290	,100},
	{508	,290	,100},
	{509	,290	,100},
	{510	,290	,100},
	{510	,294	,100},
	{511	,294	,100},
	{513	,294	,100},
	{514	,294	,100},
	{515	,294	,100},
	{515	,300	,100},
	{516	,300	,100},
	{518	,300	,100},
	{519	,300	,100},
	{520	,300	,100},
	{520	,303	,100},
	{522	,303	,100},
	{524	,303	,100},
	{526	,303	,100},
	{528	,303	,100},
	{528	,307	,100},
	{530	,307	,100},
	{532	,307	,100},
	{533	,307	,100},
	{535	,307	,100},
	{535	,312	,100},
	{537	,312	,100},
	{539	,312	,100},
	{541	,312	,100},
	{543	,312	,100},
	{543	,316	,100},
	{545	,316	,100},
	{547	,316	,100},
	{548	,316	,100},
	{550	,316	,100},
	{550	,321	,100},
	{551	,321	,100},
	{553	,321	,100},
	{554	,321	,100},
	{555	,321	,100},
	{555	,326	,100},
	{556	,326	,100},
	{558	,326	,100},
	{559	,326	,100},
	{560	,326	,100},
	{560	,330	,100},
	{561	,330	,100},
	{563	,330	,100},
	{564	,330	,100},
	{565	,330	,100},
	{565	,335	,100},
	{566	,335	,100},
	{568	,335	,100},
	{569	,335	,100},
	{570	,335	,100},
	{570	,340	,100},
	{572	,340	,100},
	{574	,340	,100},
	{576	,340	,100},
	{578	,340	,100},
	{578	,345	,100},
	{580	,345	,100},
	{582	,345	,100},
	{583	,345	,100},
	{585	,345	,100},
	{585	,350	,100},
	{587	,350	,100},
	{589	,350	,100},
	{591	,350	,100},
	{593	,350	,100},
	{593	,355	,100},
	{595	,355	,100},
	{597	,355	,100},
	{598	,355	,100},
	{598	,360	,100},
	{600	,360	,100},
	{602	,360	,100},
	{604	,360	,100},
	{606	,360	,100},
	{606	,365	,100},
	{608	,365	,100},
	{610	,365	,100},
	{612	,365	,100},
	{613	,365	,100},
	{613	,371	,100},
	{615	,371	,100},
	{617	,371	,100},
	{619	,371	,100},
	{621	,371	,100},
	{621	,376	,100},
	{623	,376	,100},
	{625	,376	,100},
	{627	,376	,100},
	{628	,376	,100},
	{628	,381	,100},
	{630	,381	,100},
	{632	,381	,100},
	{634	,381	,100},
	{636	,381	,100},
	{636	,387	,100},
	{638	,387	,100},
	{640	,387	,100},
	{642	,387	,100},
	{643	,387	,100},
	{643	,392	,100},
	{645	,392	,100},
	{647	,392	,100},
	{649	,392	,100},
	{651	,392	,100},
	{651	,398	,100},
	{653	,398	,100},
	{655	,398	,100},
	{657	,398	,100},
	{658	,398	,100},
	{658	,404	,100},
	{660	,404	,100},
	{663	,404	,100},
	{665	,404	,100},
	{668	,404	,100},
	{668	,410	,100},
	{670	,410	,100},
	{673	,410	,100},
	{675	,410	,100},
	{678	,410	,100},
	{678	,416	,100},
	{680	,416	,100},
	{683	,416	,100},
	{685	,416	,100},
	{688	,416	,100},
	{688	,422	,100},
	{690	,422	,100},
	{693	,422	,100},
	{695	,422	,100},
	{698	,422	,100},
	{698	,428	,100},
	{700	,428	,100},
	{702	,428	,100},
	{704	,428	,100},
	{706	,428	,100},
	{706	,434	,100},
	{708	,434	,100},
	{710	,434	,100},
	{712	,434	,100},
	{713	,434	,100},
	{713	,440	,100},
	{715	,440	,100},
	{717	,440	,100},
	{719	,440	,100},
	{721	,440	,100},
	{721	,447	,100},
	{723	,447	,100},
	{725	,447	,100},
	{727	,447	,100},
	{727	,453	,100},
	{728	,453	,100},
	{730	,453	,100},
	{733	,453	,100},
	{735	,453	,100},
	{735	,460	,100},
	{738	,460	,100},
	{740	,460	,100},
	{743	,460	,100},
	{745	,460	,100},
	{745	,466	,100},
	{748	,466	,100},
	{750	,466	,100},
	{753	,466	,100},
	{755	,466	,100},
	{755	,473	,100},
	{758	,473	,100},
	{760	,473	,100},
	{763	,473	,100},
	{765	,473	,100},
	{765	,480	,100},
	{768	,480	,100},
	{770	,480	,100},
	{773	,480	,100},
	{775	,480	,100},
	{775	,487	,100},
	{778	,487	,100},
	{780	,487	,100},
	{783	,487	,100},
	{785	,487	,100},
	{785	,494	,100},
	{788	,494	,100},
	{790	,494	,100},
	{793	,494	,100},
	{795	,494	,100},
	{795	,501	,100},
	{798	,501	,100},
	{800	,501	,100},
	{800	,509	,100},
	{800	,516	,100},
	{800	,523	,100},
	{800	,531	,100},
	{800	,539	,100},
	{800	,546	,100},
	{800	,554	,100},
	{800	,562	,100},
	{800	,571	,100},
	{800	,579	,100},
	{800	,587	,100},
	{800	,596	,100},
	{800	,604	,100},
	{800	,613	,100},
	{800	,622	,100},
	{800	,631	,100},
	{800	,640	,100},
	{800	,649	,100},
	{800	,659	,100},
	{800	,668	,100},
	{800	,678	,100},
	{800	,688	,100},
	{800	,698	,100},
	{800	,708	,100},
	{800	,718	,100},
	{800	,729	,100},
	{800	,739	,100},
	{800	,750	,100},
	{800	,761	,100},
	{800	,772	,100},
	{800	,783	,100},
	{800	,794	,100},
	{800	,806	,100},
	{800	,818	,100},
	{800	,829	,100},
	{800	,841	,100},
	{800	,854	,100},
	{800	,866	,100},
	{800	,879	,100},
	{800	,891	,100},
	{800	,904	,100},
	{800	,917	,100},
	{800	,931	,100},
	{800	,944	,100},
	{800	,958	,100},
	{800	,972	,100},
	{800	,986	,100},
	{800	,1000	,100},
	{800	,1015	,100},
	{800	,1029	,100},
	{800	,1044	,100},
	{800	,1059	,100},
	{800	,1075	,100},
	{800	,1090	,100},
	{800	,1106	,100},
	{800	,1122	,100},
	{800	,1138	,100},
	{800	,1155	,100},
	{800	,1172	,100},
	{800	,1189	,100},
	{800	,1206	,100},
	{800	,1223	,100},
	{800	,1241	,100},
	{800	,1259	,100},
	{803	,1259	,100},
	{808	,1241	,100},
	{810	,1241	,100},
	{813	,1241	,100},
	{818	,1223	,100},
	{820	,1223	,100},
	{825	,1206	,100},
	{828	,1206	,100},
	{830	,1206	,100},
	{835	,1189	,100},
	{838	,1189	,100},
	{840	,1189	,100},
	{845	,1172	,100},
	{848	,1172	,100},
	{850	,1172	,100},
	{857	,1155	,100},
	{860	,1155	,100},
	{866	,1138	,100},
	{869	,1138	,100},
	{872	,1138	,100},
	{878	,1122	,100},
	{882	,1122	,100},
	{885	,1122	,100},
	{891	,1106	,100},
	{894	,1106	,100},
	{897	,1106	,100},
	{904	,1090	,100},
	{908	,1090	,100},
	{915	,1075	,100},
	{919	,1075	,100},
	{923	,1075	,100},
	{930	,1059	,100},
	{934	,1059	,100},
	{938	,1059	,100},
	{945	,1044	,100},
	{949	,1044	,100},
	{956	,1029	,100},
	{960	,1029	,100},
	{964	,1029	,100},
	{971	,1015	,100},
	{975	,1015	,100},
	{979	,1015	,100},
	{986	,1000	,100},
	{990	,1000	,100},
	{994	,1000	,100},
	{1001	,986	,100},
	{1005	,986	,100},
	{1013	,972	,100},
	{1016	,972	,100},
	{1020	,972	,100},
	{1028	,958	,100},
	{1031	,958	,100},
	{1035	,958	,100},
	{1043	,944	,100},
	{1046	,944	,100},
	{1050	,944	,100},
	{1060	,931	,100},
	{1065	,931	,100},
	{1075	,917	,100},
	{1080	,917	,100},
	{1085	,917	,100},
	{1095	,904	,100},
	{1100	,904	,100},
};

#endif


