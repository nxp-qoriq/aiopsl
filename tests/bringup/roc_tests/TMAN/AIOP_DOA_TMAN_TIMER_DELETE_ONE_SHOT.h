/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		AIOP_DOA_TMAN_TIMER_DELETE_ONE_SHOT.h

@Description	This file contains TMAN DOA test.

*//***************************************************************************/

/**********************************************************************************************/
/* Frame type: 										  */
/* 1:IPv4/IPv4 UDP or TCP or GRE wi/wo Vtag		2:IPv6/IPv6 UDP or TCP or GRE wi/wo Vtag */
/* IPv6 will have all combination of Extension as fragmented and unfragmented part		 */
/* MTU=[256,1500] Payload=[800,3000 ]							  */
/* rundomize e_AIOP_TMI_CREATE, e_AIOP_TIMER_CREATE, e_AIOP_TIMER_DELETE one-shot timer   */
/**********************************************************************************************/

// e_AIOP_SPLIT_FRAME + e_AIOP_INITIAL_FRAME_PRESENTATION

#ifndef __AIOP_ROC_TMAN_TIMER_DELETE_CMD_H
#define __AIOP_ROC_TMAN_TIMER_DELETE_CMD_H

#include "main.h"
#include "common.h"
#include "TMAN_Param_generator.h"
#include "aiop_common_parameters.h"
#include "frame_headers.h"

#include "wrp_init.h"
#include "IPF_ROC_Model.h"

#define CONSTRAINT_NAME "AIOP_ROC_TMAN_TIMER_DELETE_CMD.h"
static char *__CRG_FILE_NAME = (char *)"AIOP_ROC_TMAN_TIMER_DELETE_CMD.h";


#ifdef B4860_ROC_DS
#define NUM_OF_FRAMES	MAX_NO_OF_FRAMES
#else
#define NUM_OF_FRAMES	100
#endif
#define CPU_OP_CPU           0
#define CPU_OP_TX_RX_CPU     1

#define SINGLE_FLOW          0
#define MULTI_FLOW           1

void   TestSysParamClass:: set_constraints() {
	int i;
	DEF_CRG1( IPF_Flow_Type, 1, CrgRange(CPU_OP_CPU,CPU_OP_CPU));
	DEF_CRG1( no_of_frames, 1, CrgRange (1,1) );
	DEF_CRG1( addFCS, 1, CrgRange(0,0));//Jephy_FCS
	DEF_CRG1( BufferSize, 1, CrgRange(2048,2048));//if OIM is 128, at least 448 (448,4096)
	CrgWeightCase (__CRG_FILE_NAME, __LINE__, CrgSetBase, Crg_NOT_BY_ARR, Crg_NOT_SORT, &DF_Test.get_crg_var(), 2, CrgCreateWeight(__CRG_FILE_NAME, __LINE__,0,0,7,1), CrgCreateWeight(__CRG_FILE_NAME, __LINE__,1,1,3,1));	
	DEF_CRG1( DF_Test, 1, CrgRange(0,0));//only enable for IPv4 test
	DEF_CRG1( DF_Discard, 1, CrgRange(0,0));
	Crg_Range_Con (__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &IPF_FOF.get_crg_var(), 2, CrgRange (96, 96),CrgRange (112,112));//,CrgRange (128,128)); if FOF=128+32+maxheader(38+60)=258 > 256
	DEF_CRG1( Var_BPID_For_SGE, 1, CrgRange(0,1));
	DEF_CRG1( FD_Offset, 1, CrgRange(64,64));
	DEF_CRG1( Diff_BPID_RX, 1, CrgRange(0,1));
	DEF_CRG1( Var_buffer_size, 1, CrgRange(0,1));
	DEF_CRG1( Test_Type, 1, CrgRange (SINGLE_FLOW,SINGLE_FLOW) );
	DEF_CRG1( Mult_Queues_N_1, 1, CrgRange(0,0));
	DEF_CRG1( Mult_Queues_1_N, 1, CrgRange(0,0));
	
	DEF_CRG1( AllocFailureTest, 1, CrgRange(0,0));
	DEF_CRG1( ScratchAllocFailureTest, 1, CrgRange(0,0));
	
	DEF_CRG1( MDS_10G_XAUI_Test, 1, CrgRange(0,0));
	DEF_CRG1( ActivatePlcr, 1, CrgRange(0,1));
   
	DEF_CRG1( b64kTest, 1, CrgRange(0, 0));
	DEF_CRG1( uAllocFailBufFactor, 1, CrgRange(0, 0));
	DEF_CRG1( optionsCounterEn, 1, CrgRange(0,0));
	DEF_CRG1( calculateL4checksum, 1, CrgRange(0,0));
}


/*1. TMAN cmd tmi create */
void TMANCmdTMICreate_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TMI_CREATE));
    DEF_CRG1(max_num_of_timers, 1, CrgRange(5, 5));

}

/*2. TMAN cmd tmi delete */
void TMANCmdTMIDelete_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TMI_DELETE));
	DEF_CRG1(mode_bits, 1, CrgRange(TMAN_INS_DELETE_MODE_WO_EXPIRATION,TMAN_INS_DELETE_MODE_FORCE_EXP));
	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
	DEF_CRG1(cb_with_confirmation, 1, CrgRange(1,1));
}

/*3. TMAN cmd tmi query*/
void TMANCmdTMIQuery_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TMI_QUERY));
	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
}

/*3. TMAN cmd tmi query sw */
void TMANCmdTMIQuerySW_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TMI_QUERY_SW));
	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
}


/*4. TMAN cmd timer create*/
void TMANCmdTimerCreate_CMDClass :: set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TIMER_CREATE));
	DEF_CRG1(mode_oneshot, 1, CrgRange(1,1));
	DEF_CRG1(mode_tman_priority, 1, CrgRange(0,0));
	DEF_CRG1(mode_aiop_priority, 1, CrgRange(e_AIOP_TIMER_MODE_LOW_PRIORITY_TASK,e_AIOP_TIMER_MODE_LOW_PRIORITY_TASK));
	DEF_CRG1(mode_granularity, 1, CrgRange(e_AIOP_TIMER_MODE_GRAN_100_USEC,e_AIOP_TIMER_MODE_GRAN_100_USEC));
	DEF_CRG1(duration, 1, CrgRange(0xfcf,0xfcf));
	DEF_CRG1(tmi_id, 1, CrgRange(1,1));
	DEF_CRG1(cb_with_confirmation, 1, CrgRange(1,1));
}

/*5. TMAN cmd timer delete*/
void TMANCmdTimerDelete_CMDClass :: set_constraints()
{
     DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TIMER_DELETE));
	 DEF_CRG1(mode_bits, 1, CrgRange(TMAN_TIMER_DELETE_MODE_WO_EXPIRATION,TMAN_TIMER_DELETE_MODE_WO_EXPIRATION));
 	 DEF_CRG1(tmi_id, 1, CrgRange(1,1));
	 DEF_CRG1(timer_id, 1, CrgRange(1,1));
}

/*6. TMAN cmd timer increase duration*/
void TMANCmdTimerIncDuration_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TIMER_INC_DURATION));
	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
	DEF_CRG1(timer_id, 1, CrgRange(1,PRE_CREATE_TIMER_NUM_PER_TMI));
	DEF_CRG1(duration, 1, CrgRange(1,0xFFFF));

}

/*7. TMAN cmd timer recharge*/
void TMANCmdTimerRecharge_CMDClass :: set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TIMER_RECHARGE));
 	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
 	DEF_CRG1(timer_id, 1, CrgRange(1,PRE_CREATE_TIMER_NUM_PER_TMI));
}

/*8. TMAN cmd timer query*/
void TMANCmdTimerQuery_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TIMER_QUERY));
	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
	DEF_CRG1(timer_id, 1, CrgRange(1,PRE_CREATE_TIMER_NUM_PER_TMI));

}

/*9. TMAN cmd timer completion confirmation*/
void TMANCmdCompletionConf_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_COMPLETION_CONF));
	DEF_CRG1(tmi_id, 1, CrgRange(1,PRE_CREATE_TMI_NUM));
	DEF_CRG1(timer_id, 1, CrgRange(PRE_CREATE_TIMER_NUM_PER_TMI+1,0xff));
}

/*10. TMAN cmd get TS*/
void TMANCmdGetTS_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_GET_TS));

}


/*11. FDMA cmd enqueue frame*/
void FDMACmdEnqueueWorkingFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_WORKING_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(0, 0xFFFFFFFF));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0xFFFF));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0xFF));
    DEF_CRG1(PS, 1, CrgRange(0, 1));
    DEF_CRG1(TC, 1, CrgRange(0, 3));
    DEF_CRG1(EIS, 1, CrgRange(1, 1));
  //  DEF_CRG1(spid, 1, CrgRange(0, 0xff));
}


/*12. FDMA cmd last cmd*/
void FDMATerminate_CMDClass::set_constraints(){
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_FDMA_OERATION_LAST));
}



void TMANCmdListClass::set_constraints(){
    DEF_CRG1(maxNumOfCmd, 1, CrgRange(PRE_CREATE_TMI_NUM+PRE_CREATE_TMI_NUM*PRE_CREATE_TIMER_NUM_PER_TMI+1+1, PRE_CREATE_TMI_NUM+PRE_CREATE_TMI_NUM*PRE_CREATE_TIMER_NUM_PER_TMI+1+1));
}


void TMANCmdClass::set_constraints(){ 
    DEF_CRG1(CMD, 1, CrgRange(e_AIOP_TIMER_DELETE, e_AIOP_TIMER_DELETE));
}


void   FrameInputClass:: set_constraints()
{
	int i;
	int test_type = randomInt(1,1);
	if(test_type==1)
	{
		for(i=0;i<MAX_NO_OF_FRAMES;i++)
		{
			Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &FT[i].get_crg_var(), 4,\
					  CrgRange (Ether_IPv4,Ether_IPv4),\
					  CrgRange (Ether_IPv4_UDP,Ether_IPv4_UDP),\
					  CrgRange (Ether_IPv6,Ether_IPv6),\
					  CrgRange (Ether_IPv6_UDP,Ether_IPv6_UDP));
		}
		DEF_CRG1( VLAN, 1, CrgRange (no_vlan,no_vlan) );//Y
	}
	else if(test_type==2)
	{
		for(i=0;i<MAX_NO_OF_FRAMES;i++)
		{
			Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
							  Crg_NOT_SORT, Crg_NO_DET_VARS, &FT[i].get_crg_var(), 4,\
							  CrgRange (Ether_VTag_IPv4, Ether_VTag_IPv4),\
							  CrgRange (Ether_VTag_IPv4_UDP, Ether_VTag_IPv4_UDP),\
							  CrgRange (Ether_VTag_IPv6,Ether_VTag_IPv6),\
							  CrgRange (Ether_VTag_IPv6_UDP,Ether_VTag_IPv6_UDP));
		}
		DEF_CRG1( VLAN, 1, CrgRange (vlan1,vlan3) );//Y
	}
    DEF_CRG1( burstSize, 1, CrgRange (1, 1) );
}

void 	EtherHeaderClass::set_constraints(){
	DEF_CRG1( da[0], 1, CrgRange (0x12,0x12) );
	DEF_CRG1( da[1], 1, CrgRange (0x34,0x34) );
	DEF_CRG1( da[2], 1, CrgRange (0x56,0x56) );
	DEF_CRG1( da[3], 1, CrgRange (0x70,0x77) );
	DEF_CRG1( da[4], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( da[5], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( sa[0], 1, CrgRange (0,0) );//10
	DEF_CRG1( sa[1], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( sa[2], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( sa[3], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( sa[4], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( sa[5], 1, CrgRange (0,0xff) );//10
	DEF_CRG1( length, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( type, 1, CrgRange (0x9999,0x9999) );//10
//	DEF_CRG1( da_match.r, 1, CrgRange (0,1) );//10
}

void   IPv4HeaderClass:: set_constraints() {
	int i;
	DEF_CRG1( Version, 1, CrgSingleton (4) );//Y
	DEF_CRG1( IHL, 1, CrgRange (5,15) );//Y
	DEF_CRG1( TOS, 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( Length, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( Id, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( Flags, 1, CrgRange (0,0) );//10
	DEF_CRG1( FragmentOffset, 1, CrgRange (0x0000,0x0000) );//10
	DEF_CRG1( TTL, 1, CrgRange (3,255) );//10
	//CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Protocol.r, 4, CrgRange (0x0, 0x5),CrgRange (0x7,0x10),CrgRange (0x12,0x83),CrgRange (0x85,0xff)); //exculinf TCP,UDP,SCTP
	Crg_Range_Con (__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &Protocol.get_crg_var(), 4, CrgRange (0x0, 0x0),CrgRange (0x0,0x0),CrgRange (0x0,0x0),CrgRange (0x0,0x0));
	DEF_CRG1( Checksum, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( SourceIP, 1, CrgRange (0,0xffffffff) );//10
	DEF_CRG1( DestIP, 1, CrgRange (0,0xffffffff) );//10
	for(i=0; i<20; i++){
		DEF_CRG1( Options[i], 1, CrgRange (0,0xffffffff) );//10
	}
	DEF_CRG1( IPChecksumOK, 1, CrgRange (1,1) );//10
/* FULL_HEADER_SUPPORT */


}

void   FramePayloadClass:: set_constraints() {
	/*due to FD offset cannot be bigger than 8K and we use single buffer output, so we limit frame length to about 7k*/

	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
								  Crg_NOT_SORT, Crg_NO_DET_VARS, &Length.get_crg_var(), 2,\
								  CrgRange (400,500),\
								  CrgRange (400,500));

}

void   PPPoEHeaderClass:: set_constraints() {
	//Jephy CrgWeightCase (CrgSetBase, Crg_NOT_BY_ARR, Crg_NOT_SORT, Version.r, 2, CrgCreateWeight (CrgSingleton (0x2), 0.2), CrgCreateWeight (CrgSingleton (0x1), 9.8) );
	DEF_CRG1( Version, 1, CrgRange (0x1,0x2) );//Y
	DEF_CRG1( Type, 1, CrgRange (0x1,0x1) );//Y
	DEF_CRG1( Code, 1, CrgRange (0x0,0x0) );//Y
	DEF_CRG1( SessionID, 1, CrgRange (0x1,0xfffe) );//10
	DEF_CRG1( Length, 1, CrgRange (0,0xffff) );//10
}

void   UDPHeaderClass:: set_constraints() {
	DEF_CRG1( SourcePort, 1, CrgRange (	0x0,	0xffff) );//10
	DEF_CRG1( DestPort, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( Length, 1, CrgRange (0xd,0xd) );//10
	DEF_CRG1( Checksum, 1, CrgRange (0x0,0xffff) );//10
}

void   TCPHeaderClass:: set_constraints() {
	int i;
	DEF_CRG1( SourcePort, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( DestPort, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( SeqNum, 1, CrgRange (0xcccccccc,0xcccccccc) );//10
	DEF_CRG1( AckNum, 1, CrgRange (0xdddddddd,0xdddddddd) );//10zz
	DEF_CRG1( DataOffset, 1, CrgRange (5,5) );//10
	DEF_CRG1( Reserved, 1, CrgRange (0,0) );//10
	DEF_CRG1( ECN, 1, CrgRange (0,0) );//10
	DEF_CRG1( CtrlBits, 1, CrgRange (1,1) );//10
	DEF_CRG1( Window, 1, CrgRange (0x3333,0x3333) );//10
	DEF_CRG1( Checksum, 1, CrgRange (0,0) );//10
	DEF_CRG1( UrgentPointer, 1, CrgRange (0x4444,0x4444) );//10
	DEF_CRG1( OptionsSize, 1, CrgRange (5,5) );//10
	DEF_CRG1( TCPChecksumOK, 1, CrgRange (1,1) );//10

	for(i=0; i<44; i++){
		DEF_CRG1( Options[i], 1, CrgRange (0,0xff) );//10
	}
}
//Jephy add
void   MinEncHeaderClass:: set_constraints() {
	DEF_CRG1( Protocol, 1, CrgRange (0x37,0x37) );
	DEF_CRG1( s_reserved, 1, CrgRange (0x80,0x80) );
	DEF_CRG1( Checksum, 1, CrgRange (0,0) );
	DEF_CRG1( SourceAddress, 1, CrgRange (0xcccccccc,0xcccccccc) );
	DEF_CRG1( DestAddress, 1, CrgRange (0xdddddddd,0xdddddddd) );

}

void   GREHeaderClass:: set_constraints() {
	DEF_CRG1( HeaderInfo, 1, CrgRange (0,0) );
	DEF_CRG1( FlagsVersion, 1, CrgRange (0,0) );
	DEF_CRG1( Protocol, 1, CrgRange (0x9967,0x9969) );//10{0x9967,0x998B,0x999F};
	DEF_CRG1( Checksum, 1, CrgRange (0,0) );
	DEF_CRG1( Offset, 1, CrgRange (0,0) );
	DEF_CRG1( Key, 1, CrgRange (0,0) );
	DEF_CRG1( SequenceNumber, 1, CrgRange (0,0) );
	DEF_CRG1( Routing, 1, CrgRange (0,0) );

}
void   MPLSHeaderClass:: set_constraints() {
	DEF_CRG1( Label, 1, CrgRange (0x9999,0x9999) );
	DEF_CRG1( Exp, 1, CrgRange (0,0) );
	DEF_CRG1( Sbit, 1, CrgRange (0,0) );
	DEF_CRG1( TTL, 1, CrgRange (0x4,0x6) );

}
void   IPSECHeaderClass:: set_constraints() {
	DEF_CRG1( SecurityParametersIndex, 1, CrgRange (0,0) );
	DEF_CRG1( SequenceNumber, 1, CrgRange (0,0) );
}
void   Tunnel_IPv4HeaderClass:: set_constraints() {
	int i;
	DEF_CRG1( Version, 1, CrgRange (0x4,0x4) );
	DEF_CRG1( IHL, 1, CrgRange (0x5,0x5) );
	DEF_CRG1( TOS, 1, CrgRange (0,0) );
	DEF_CRG1( Length, 1, CrgRange (0x5a,0x5a) );
	DEF_CRG1( Id, 1, CrgRange (0,0xffff) );
	DEF_CRG1( Flags, 1, CrgRange (0,0) );
	DEF_CRG1( FragmentOffset, 1, CrgRange (0x4000,0x4000) );
	DEF_CRG1( TTL, 1, CrgRange (0,0) );
	DEF_CRG1( Protocol, 1, CrgRange (0,0) );
	DEF_CRG1( SourceIP, 1, CrgRange (0x0,0xFFFFFFFF) );
	DEF_CRG1( IPChecksumOK, 1, CrgRange (1,1) );
	DEF_CRG1( Checksum, 1, CrgRange (0x7155,0x7155) );
	DEF_CRG1( DestIP, 1, CrgRange (0x0,0xFFFFFFFF) );
	for(i=0; i<20; i++){
		DEF_CRG1( Options[i], 1, CrgRange (0,0xff) );//10
	}

}
void   Tunnel_IPv6HeaderClass:: set_constraints() {
	DEF_CRG1( Version, 1, CrgRange (0x6,0x6) );
	DEF_CRG1( TC, 1, CrgRange (0x5,0x5) );
	DEF_CRG1( FlowLabel, 1, CrgRange (0,0xffff) );
	DEF_CRG1( PayloadLength, 1, CrgRange (0x5a,0x5a) );
	DEF_CRG1( NextHeader, 1, CrgRange (0x0004,0x0004) );
	DEF_CRG1( HopLimit, 1, CrgRange (0,0) );
	DEF_CRG1( SourceAddress[0], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( SourceAddress[1], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( SourceAddress[2], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( SourceAddress[3], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[0], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[1], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[2], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[3], 1, CrgRange (1,0xffffffff) );//10

}
//Jephy end

void   IPv6HeaderClass:: set_constraints() {
	DEF_CRG1( Version, 1, CrgSingleton (6) );//10
	DEF_CRG1( TC, 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( FlowLabel, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( PayloadLength, 1, CrgRange (0,0) );//10
	DEF_CRG1( NextHeader, 1, CrgRange (59,59) );//10
	DEF_CRG1( HopLimit, 1, CrgRange (3,255) );//10
	DEF_CRG1( SourceAddress[0], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( SourceAddress[1], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( SourceAddress[2], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( SourceAddress[3], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[0], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[1], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[2], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( DestinationAddress[3], 1, CrgRange (1,0xffffffff) );//10
	DEF_CRG1( NumOfExHdr, 1, CrgRange (0x00,0x00) );//10
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
											  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextExtHdr.get_crg_var(), 3,\
											  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
											  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
											  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
	DEF_CRG1( Rfc_ExtHdrOrder, 1, CrgRange (0x01,0x01) );//10
}

void   PPPHeaderClass:: set_constraints() {
	//CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Protocol.r, 2, CrgRange (0x0, 0x20),CrgRange (0x22,0xffff)); //excluding 0x21 IP
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &Protocol.get_crg_var(), 2, CrgRange (0x0, 0x20),CrgRange (0x22,0xffff));

}

void   VTagHeaderClass:: set_constraints() {
	DEF_CRG1( VPri, 1, CrgRange (0,7) );//10
	DEF_CRG1( CFI, 1, CrgRange (0,0) );//10
	DEF_CRG1( VID, 1, CrgRange (0,0xfff) );//10
	DEF_CRG1( length, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( type, 1, CrgRange (0,0xffff) );//10
}

void   OptionsClass:: set_constraints() {
	DEF_CRG1( OptionsData, 1, CrgRange (0,0xff));//10
}

void   AuthDataClass:: set_constraints() {
	DEF_CRG1( AuthData, 1, CrgRange (0,0xff));//10
}

void   PayloadDataClass:: set_constraints() {
	DEF_CRG1( PayloadData, 1, CrgRange (0,0xff));//10
}

void   AddressClass:: set_constraints() {
	DEF_CRG1( Address[0], 1, CrgRange (0,0xffffffff) );//10
	DEF_CRG1( Address[1], 1, CrgRange (0,0xffffffff) );//10
	DEF_CRG1( Address[2], 1, CrgRange (0,0xffffffff) );//10
	DEF_CRG1( Address[3], 1, CrgRange (0,0xffffffff) );//10
}

void   ICMPMesgClass:: set_constraints() {
	DEF_CRG1( ICMPMesg, 1, CrgRange (0,0xff));//10
}

void   HopbyHopExtHeaderClass:: set_constraints() {
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
										  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextExtHdr.get_crg_var(), 3,\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
	DEF_CRG1( HdrLen, 1, CrgRange (0,0) );
}

void   RoutingExtHeaderClass:: set_constraints() {
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
										  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextExtHdr.get_crg_var(), 3,\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
	DEF_CRG1( HdrLen, 1, CrgRange (0,0) );
	DEF_CRG1( RoutingType, 1, CrgSingleton (0) );
	DEF_CRG1( SegmentsLeft, 1, CrgRange (0,4) );
	DEF_CRG1( Reserved, 1, CrgRange (0x0,0xffffffff) );
}

void   FragmentHeaderClass:: set_constraints() {
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
										  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextExtHdr.get_crg_var(), 3,\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
	DEF_CRG1( Reserved, 1, CrgRange (0x00,0x00) );//10
	DEF_CRG1( FragmentOffset, 1, CrgRange (0x00,0x00));//10
	//CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, FragmentOffset.r, 2, CrgSingleton (0),CrgRange (1, 0xffff));
	DEF_CRG1( Res, 1, CrgRange (0x00,0x00));//10
	DEF_CRG1( MFlag, 1, CrgRange (0x0,0x1) );//10
	DEF_CRG1( Identification, 1, CrgRange (0x0,0xffffffff) );//10
}

void   DestOptionsExtHeaderClass:: set_constraints() {
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
										  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextExtHdr.get_crg_var(), 3,\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
	DEF_CRG1( HdrLen, 1, CrgRange (0,0) );
}

void   AHExtHeaderClass:: set_constraints() {
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
										  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextExtHdr.get_crg_var(), 3,\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
										  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
	DEF_CRG1( PayloadLen, 1, CrgRange (1,1) );//minimum - 1
	DEF_CRG1( Reserved, 1, CrgSingleton (0) );//10
	DEF_CRG1( SPI, 1, CrgRange (0x0,0xffffffff) );//10
	DEF_CRG1( SeqNum, 1, CrgRange (0x0,0xffffffff) );//10
}
void   ESPHeaderClass:: set_constraints() {
	DEF_CRG1( SPI, 1, CrgRange (0x0,0xffffffff) );//10
	DEF_CRG1( SeqNum, 1, CrgRange (0x0,0xffffffff) );//10
}

void   ESPTrailerClass:: set_constraints() {
	DEF_CRG1( PaddingLen, 1, CrgRange (0,0));//10
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
									  Crg_NOT_SORT, Crg_NO_DET_VARS, &NextHeader.get_crg_var(), 3,\
									  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
									  CrgRange (IP_PROTO_HOPOPTS, IP_PROTO_HOPOPTS),\
									  CrgRange (IP_PROTO_ROUTING, IP_PROTO_ROUTING));
}

void   ESPExtHeaderClass:: set_constraints() {
	DEF_CRG1( PayloadDataLength, 1, CrgRange (0,0) );//10
	DEF_CRG1( AuthDataLength, 1, CrgRange (0,0) );//10
}

void   ExtnHdrClass:: set_constraints() {

}

#endif //__AIOP_ROC_TMAN_TIMER_DELETE_CMD_H

