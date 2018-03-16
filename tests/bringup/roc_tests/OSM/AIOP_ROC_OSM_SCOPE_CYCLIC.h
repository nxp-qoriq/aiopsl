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
@File		AIOP_ROC_OSM_SCOPE_CYCLIC.h

@Description	This file contains OSM DOA test.

*//***************************************************************************/

/**********************************************************************************************/
/* Frame type: 																				  */
/* 1:IPv4/IPv4 UDP or TCP or GRE wi/wo Vtag		2:IPv6/IPv6 UDP or TCP or GRE wi/wo Vtag      */
/* IPv6 will have all combination of Extension as fragmented and unfragmented part			  */
/* MTU=[256,1500] Payload=[800,3000 ]													  */
/**********************************************************************************************/
#ifndef __AIOP_ROC_PARSER_CONSTRAINT_H
#define __AIOP_ROC_PARSER_CONSTRAINT_H

#include "common.h"
#include "tlu_param_checker.h"
#include "aiop_common_parameters.h"
#include "frame_generators.h"
#include "frame_headers.h"

static uint32_t id;
#define ep_asapa 0x000403c0

#define CONSTRAINT_NAME "AIOP_ROC_OSM_SCOPE_CYCLIC.h"
static char *__CRG_FILE_NAME = (char *)"AIOP_ROC_OSM_SCOPE_CYCLIC.h";

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
	DEF_CRG1( FD_Offset, 1, CrgRange(512,512));
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
	DEF_CRG1( ErrorFlow, 1, CrgSingleton(0));

	DEF_CRG1( ScopeMode, 1, CrgSingleton(EXCLUSIVE));
	DEF_CRG1( ScopeLevel, 1, CrgRange(LEVEL1,LEVEL1));
}

void   FrameInputClass:: set_constraints()
{
	DEF_CRG1( FT[0] , 1 ,CrgRange(Ether,Ether));
	DEF_CRG1( VLAN[0], 1, CrgRange (no_vlan,no_vlan) );//Y
	DEF_CRG1( MPLSNum[0], 1, CrgRange (0,0) );	
}

void FDMACmdReplaceSegment_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_REPLACE_SEGMENT));
    DEF_CRG1(to_offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(to_size, 1, CrgRange(0, 0xFF));
    DEF_CRG1(from_size, 1, CrgRange(0, 10));
    DEF_CRG1(SA, 1, CrgRange(0, 1));

    for (i = 0; i < 3; i++) {
        DEF_CRG1(reserved[i], 1, CrgSingleton(0));
    }


    for (i = 0; i < 255; i++) {
        DEF_CRG1(replace_content[i], 1, CrgRange(0, 0xFF));
    }

    DEF_CRG1(size_rs, 1, CrgRange(0, 100));
    //DEF_CRG1(ws_address_rs, 1, CrgRange(0, 50)); /* represent offset */
};

/* ----------- NEW ------------------*/
/*10. FDMA cmd enqueue frame*/
void FDMACmdEnqueueWorkingFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_WORKING_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(0, 0xFFFFFFFF));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0xFFFF));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0xFF));
    //DEF_CRG1(storage_profile, 1, CrgRange(0, 0xFF));
    //DEF_CRG1(release_frame_handle, 1, CrgRange(0, 0xFF));
    DEF_CRG1(PS, 1, CrgRange(0, 1));
    DEF_CRG1(TC, 1, CrgRange(0, 3));
    DEF_CRG1(EIS, 1, CrgRange(1, 1));

    //DEF_CRG1(reserved, 1, CrgSingleton(0));
};

/*** OSM ***/
void FDMACmdPresentSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_PRESENT_SEGMENT));
    DEF_CRG1(offset, 1, CrgRange(0x80,0x80));
    DEF_CRG1(present_size, 1, CrgRange(128, 128));
    DEF_CRG1(seg_length, 1, CrgRange(0, 0));
    DEF_CRG1(seg_handle, 1, CrgRange(0, 0));
    DEF_CRG1(SR, 1, CrgRange(0, 0));  
};

void FDMACmdASAUpdate_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ASA_UPDATE));
    DEF_CRG1(value, 1, CrgRange(120, 120)); 
};

void FDMACmdCloseSegment_CMDClass:: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CLOSE_SEGMENT));
};

void IFMODULECmd_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_IF_MODULE));
};

void OSMCmdTranToExIncScopeID_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_TRANS_XX_INC));
    DEF_CRG1(cmdSize, 1, CrgSingleton(8));
    DEF_CRG1(numOfCmd, 1, CrgRange(3, 3)); 

};

void OSMCmdTranToExNewScopeID_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_TRANS_XX_NEW));
    DEF_CRG1(scope_id, 1 ,CrgRange(0x1, 0xffffffff)); 
    DEF_CRG1(cmdSize, 1, CrgSingleton(16));
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 1)); 

};


void OSMCmdTranToConIncScopeID_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_TRANS_XC_INC));
    DEF_CRG1(cmdSize, 1, CrgSingleton(8));
    DEF_CRG1(numOfCmd, 1, CrgRange(3, 8)); 

};

void OSMCmdTranToConNewScopeID_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_TRANS_XC_NEW));
    DEF_CRG1(scope_id, 1 ,CrgRange(0x1, 0xffffffff)); 
    DEF_CRG1(cmdSize, 1, CrgSingleton(16));
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 1)); 


};

void OSMCmdRelinquishEx_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_RELINQUISH));
    DEF_CRG1(cmdSize, 1, CrgSingleton(8));
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 1)); 

};

void OSMCmdEnterToExIncScope_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_ENTER_XX_INC));
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 3)); 
    DEF_CRG1(cmdSize, 1, CrgSingleton(8));

};

void OSMCmdEnterToExNewScope_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_ENTER_XX_NEW));
    DEF_CRG1(scope_id, 1 ,CrgRange(0xaaaaaaae,0xaaaaaaae)); 
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 1)); 
    DEF_CRG1(cmdSize, 1, CrgSingleton(16));
};

void OSMCmdEnterScope_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_ENTER));
    DEF_CRG1(scope_id, 1 ,CrgRange(0x1, 0xffffffff));
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 1)); 
    DEF_CRG1(flags , 1 , CrgRange(OSM_SCOPE_ENTER_CONC_OP,OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP));
    DEF_CRG1(cmdSize, 1, CrgSingleton(16));

};

void OSMCmdScopeExit_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_SCOPE_EXIT));
    DEF_CRG1(numOfCmd, 1, CrgRange(1, 1)); 
    DEF_CRG1(cmdSize, 1, CrgSingleton(8));

};

void OSMCmdGetScope_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_OSM_GET_SCOPE));
};

void OSMCmdClass::set_constraints(){

    /*** OSM ***/
    DEF_CRG1(OSM_SCOPE_TRANS_TO_EX_INC_CMD, 1, CrgRange(1, 1));	
	DEF_CRG1(OSM_SCOPE_TRANS_TO_EX_NEW_CMD, 1, CrgRange(0, 0));	
	DEF_CRG1(OSM_SCOPE_TRANS_TO_CON_INC_CMD, 1, CrgRange(0, 0));	
	DEF_CRG1(OSM_SCOPE_TRANS_TO_CON_NEW_CMD, 1, CrgRange(0, 0));
	DEF_CRG1(OSM_SCOPE_RELINQUISH_EX_CMD, 1, CrgRange(0, 0));
	DEF_CRG1(OSM_SCOPE_ENTER_TO_EX_INC_CMD, 1, CrgRange(0, 0));
	DEF_CRG1(OSM_SCOPE_ENTER_TO_EX_NEW_CMD, 1, CrgRange(1, 1));	
	DEF_CRG1(OSM_SCOPE_ENTER_CMD, 1, CrgRange(0,0));	
	DEF_CRG1(OSM_SCOPE_EXIT_CMD, 1, CrgRange(0, 0));	
	DEF_CRG1(OSM_GET_SCOPE_CMD, 1, CrgRange(0, 0));	
	
	/* FDMA */
	DEF_CRG1(FDMA_ASA_UPDATE_CMD, 1, CrgRange(0, 0));	
	DEF_CRG1(FDMA_CLOSE_SEGMENT_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(FDMA_PRESENT_SEGMENT_CMD, 1,  CrgRange(0, 0));
	DEF_CRG1(IF_MODULE_CMD, 1, CrgRange(0, 0));	    
   
}

void OSMCmdListClass::set_constraints()
{
    DEF_CRG1(maxNumOfCmd, 1, CrgRange(4, 4));
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
//	CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, type.r, 3, CrgRange (0x0, 0x7ff),CrgRange (0x801,0x8863),CrgRange (0x8865,0xffff)); //exculinf TCP,UDP,SCTP
//	DEF_CRG1( da_match.r, 1, CrgRange (0,1) );//10
}

void   IPv4HeaderClass:: set_constraints() {
	int i;
	DEF_CRG1( Version, 1, CrgSingleton (4) );//Y
	DEF_CRG1( IHL, 1, CrgRange (5,5) );// no options
	DEF_CRG1( TOS, 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( Length, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( Id, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( Flags, 1, CrgRange (0,0) );//10
	DEF_CRG1( FragmentOffset, 1, CrgRange (0x0000,0x0000) );//10
	DEF_CRG1( TTL, 1, CrgRange (3,255) );//10
	//CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Protocol.r, 4, CrgRange (0x0, 0x5),CrgRange (0x7,0x10),CrgRange (0x12,0x83),CrgRange (0x85,0xff)); //exculinf TCP,UDP,SCTP
	Crg_Range_Con (__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &Protocol.get_crg_var(), 4, CrgRange (0x0, 0x0),CrgRange (0x0,0x0),CrgRange (0x0,0x0),CrgRange (0x0,0x0));
	DEF_CRG1( Checksum, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( SourceIP, 1, CrgRange (0xcccccccc,0xcccccccc) );//10
	DEF_CRG1( DestIP, 1, CrgRange (0xdddddddd,0xdddddddd) );//10
	DEF_CRG1( TunnelHeaderEn, 1, CrgRange (0,0) );//10
	DEF_CRG1( TunnelHeaderType, 1, CrgRange (e_IPV4,e_IPV6) );//10
	for(i=0; i<20; i++){
		DEF_CRG1( Options[i], 1, CrgRange (0,0xffffffff) );//10
	}
	DEF_CRG1( IPChecksumOK, 1, CrgRange (1,1) );//10
/* FULL_HEADER_SUPPORT */


}

void   FramePayloadClass:: set_constraints() {
	/*due to FD offset cannot be bigger than 8K and we use single buffer output, so we limit frame length to about 7k*/
	//Min should be 15*160=2400,15 is cmd number,160 is the large input parameters of one cmd(create rule)
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
								  Crg_NOT_SORT, Crg_NO_DET_VARS, &Length.get_crg_var(), 2,\
								  CrgRange (50,50),\
								  CrgRange (50,50));

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
	DEF_CRG1( SourcePort, 1, CrgRange (	0xaaaa,	0xaaaa) );//10
	DEF_CRG1( DestPort, 1, CrgRange (0xbbbb,0xbbbb) );//10
	DEF_CRG1( Length, 1, CrgRange (0xd,0xd) );//10
	DEF_CRG1( Checksum, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( ZeroChecksum, 1, CrgRange (0,0) );
}

void DCCPHeaderClass:: set_constraints(){
	DEF_CRG1( SourcePort, 1, CrgRange (0x1 , 0xffff) );
	DEF_CRG1( DestPort, 1, CrgRange (0x1 , 0xffff));
	DEF_CRG1( DataOffset, 1, CrgRange (0xff , 0xff) ); 
	DEF_CRG1( CCVal, 1, CrgRange (0 , 0));
	DEF_CRG1( CsCov, 1, CrgRange (0 , 0) );
	DEF_CRG1( Checksum, 1, CrgRange (0x1 , 0xffff));
	DEF_CRG1( Reserved, 1, CrgRange (0 , 0) ); 
	DEF_CRG1( Type, 1, CrgRange (0 , 9));
	DEF_CRG1( X, 1, CrgRange (1 , 1) );
	DEF_CRG1( SequenceNumber, 1, CrgRange (0x1 , 0xffffff));
	DEF_CRG1( SequenceNumberCont, 1, CrgRange (0xdddddddd , 0xdddddddd) ); 
}

void GTPHeaderClass:: set_constraints(){
	DEF_CRG1( Version, 1, CrgRange (2 , 2) );
	DEF_CRG1( P, 1, CrgRange (0 , 0));
	DEF_CRG1( T, 1, CrgRange (1 , 1) );
	DEF_CRG1( Spare_1, 1, CrgRange (0 , 0));
	DEF_CRG1( MessageType, 1, CrgRange (0x1 , 0x1) );
	DEF_CRG1( MessageLength, 1, CrgRange (0xffff , 0xffff));
	DEF_CRG1( TEID, 1, CrgRange (0x1 , 0xffffffff) );
	DEF_CRG1( SequenceNumber, 1, CrgRange (0x222222 , 0x222222));
	DEF_CRG1( Spare_2, 1, CrgRange (0x22 , 0x22));

}

void SCTPHeaderClass:: set_constraints(){
	DEF_CRG1( SourcePort, 1, CrgRange (0x1 , 0xffff) );
	DEF_CRG1( DestPort, 1, CrgRange (0x1 , 0xffff));
	DEF_CRG1( VerifTag, 1, CrgRange (0 , 0) );
	DEF_CRG1( Checksum, 1, CrgRange (0x11111111 , 0x11111111));
}

void LLC_SNAPHeaderClass:: set_constraints(){
	DEF_CRG1( llc, 1, CrgRange (0xaaaaaa03 , 0xaaaaaa03) );
	DEF_CRG1( oui, 1, CrgRange (0 , 0) );
	DEF_CRG1( type, 1, CrgRange (0xff , 0xff) );
}

void ICMPHeaderClass:: set_constraints(){
	DEF_CRG1( type, 1, CrgRange (4 , 4) );
	DEF_CRG1( code, 1, CrgRange (0 , 0) );
	DEF_CRG1( checksum, 1, CrgRange (0xaaaa , 0xaaaa) );
	DEF_CRG1(restHeader,1, CrgRange (0,0xffffffff));
}

void   ARPHeaderClass:: set_constraints() {
	DEF_CRG1( htype, 1, CrgRange (1 , 1) );// Ethernet
	DEF_CRG1( ptype, 1, CrgRange (0x800 , 0x800) );// IPv4
	DEF_CRG1( hlen, 1, CrgRange (0x6 , 0x6) );//HW length 
	DEF_CRG1( plen, 1, CrgRange (0x4 , 0x4) );//Protocol length
	DEF_CRG1( operation, 1, CrgRange (0x1,0x2) );// request / replay
	DEF_CRG1( sha[0], 1, CrgRange (0x1,0xff) );// hardware address of the sender
	DEF_CRG1( sha[1], 1, CrgRange (0x1,0xff) );// hardware address of the sender
	DEF_CRG1( sha[2], 1, CrgRange (0x1,0xff) );// hardware address of the sender
	DEF_CRG1( sha[3], 1, CrgRange (0x1,0xff) );// hardware address of the sender
	DEF_CRG1( sha[4], 1, CrgRange (0x1,0xff) );// hardware address of the sender
	DEF_CRG1( sha[5], 1, CrgRange (0x1,0xff) );// hardware address of the sender
	DEF_CRG1( spa, 1, CrgRange (0x1,0xffffffff) );// ip address of the sender
	DEF_CRG1( tha[0], 1, CrgRange (0x1,0xff) );//hardware address of the receiver
	DEF_CRG1( tha[1], 1, CrgRange (0x1,0xff) );//hardware address of the receiver
	DEF_CRG1( tha[2], 1, CrgRange (0x1,0xff) );//hardware address of the receiver
	DEF_CRG1( tha[3], 1, CrgRange (0x1,0xff) );//hardware address of the receiver
	DEF_CRG1( tha[4], 1, CrgRange (0x1,0xff) );//hardware address of the receiver
	DEF_CRG1( tha[5], 1, CrgRange (0x1,0xff) );//hardware address of the receiver
	DEF_CRG1( tpa, 1, CrgRange (0x1,0xffffffff) );//ip address of the receiver*/
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
	//DEF_CRG1( Sbit, 1, CrgRange (0,0) );
	DEF_CRG1( TTL, 1, CrgRange (0x4,0x6) );

}
void   IPSECHeaderClass:: set_constraints() {
	DEF_CRG1( NextHeader, 1, CrgRange (0,0) );
	DEF_CRG1( PayloadLength, 1, CrgRange (0,0) );
	DEF_CRG1( Reserved, 1, CrgRange (0,0) );
	DEF_CRG1( SecurityParametersIndex, 1, CrgRange (0,0) );
	DEF_CRG1( SequenceNumber, 1, CrgRange (0,0) );
}
void   Tunnel_IPv4HeaderClass:: set_constraints() {
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
	DEF_CRG1( TunnelHeaderEn, 1, CrgRange (0,0) );//10
	DEF_CRG1( TunnelHeaderType, 1, CrgRange (e_IPV4,e_IPV6) );//10
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

#endif //__AIOP_ROC_FDMA_H




