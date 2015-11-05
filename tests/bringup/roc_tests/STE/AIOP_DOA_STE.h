/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		AIOP_DOA_STE.h

@Description	This file contains STE DOA test.

*//***************************************************************************/

/**********************************************************************************************/
/* Frame type: 																				  */
/* 1:IPv4/IPv4 UDP or TCP or GRE wi/wo Vtag		2:IPv6/IPv6 UDP or TCP or GRE wi/wo Vtag      */
/* IPv6 will have all combination of Extension as fragmented and unfragmented part			  */
/* MTU=[256,1500] Payload=[800,3000 ]													      */
/* This test will create all available STE commands										      */
/**********************************************************************************************/
#ifndef __AIOP_ROC_STE_H
#define __AIOP_ROC_STE_H

#include "common.h"
#include "ste_param_generator.h"
#include "aiop_common_parameters.h"
#include "frame_generators.h"
#include "frame_headers.h"

#define CONSTRAINT_NAME "AIOP_ROC_STE.h"
static char *__CRG_FILE_NAME = (char *)"AIOP_ROC_STE.h";
#define CPU_OP_CPU           0
#define CPU_OP_TX_RX_CPU     1

#define SINGLE_FLOW          0
#define MULTI_FLOW           1

void STEGenCounterTypeClass::set_constraints()
{
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &counterType.get_crg_var(), 2,\
					  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE),\
					  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
}

void STECounterSetClass::set_constraints()
{
    DEF_CRG1(memType, 1, CrgRange(STE_MEMTYPE_DP_DDR, STE_MEMTYPE_DP_DDR));
    DEF_CRG1(numOfCounters, 1, CrgRange(STE_MAX_COUNTERS, STE_MAX_COUNTERS));
//    DEF_CRG1(startOffset, 1, CrgRange(0x01000000, 0x01000000));//STE_MEM_MAX_SIZE - STE_MAX_COUNTERS * 8));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &startOffset.get_crg_var(), 2,
                  CrgRange (0x0, 0x0),//right address boundary
                  CrgRange (0x0, 0x0));//wrong address boundary bad alignment errror
	DEF_CRG1(counter_create, 1 , CrgRange(STE_COUNTER_RANDOM,STE_COUNTER_RANDOM));
    DEF_CRG1(STE_Decode_Error, 1, CrgRange(0, 0));
    //DEF_CRG1(STE_Descriptor_Error, 1, CrgRange(0, 0));
    DEF_CRG1(STE_Transfer_Error, 1, CrgRange(0, 0));//need debug
}

void STECmdSet124Byte_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_SET_4B));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size.get_crg_var(), 3,
                  CrgRange (STE_COUNTERTYPE_1BYTE, STE_COUNTERTYPE_1BYTE),
                  CrgRange (STE_COUNTERTYPE_2BYTE, STE_COUNTERTYPE_2BYTE),
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 1)); 
    DEF_CRG1(value, 1, CrgRange(0, 0xffffffff));
}

void STECmdSet8Byte_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_SET_8B));
    DEF_CRG1(size, 1, CrgSingleton(STE_COUNTERTYPE_8BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 1)); 
    DEF_CRG1(valuelo, 1, CrgRange(0, 0xffffffff));
    DEF_CRG1(valuehi, 1, CrgRange(0, 0xffffffff));
}

void STECmdAdd_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_ADD));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 1)); 
    DEF_CRG1(value, 1, CrgRange(0x2, 0x2));
    DEF_CRG1(saturate, 1, CrgRange(1, 1));
}

void STECmdSub_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_DEC));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 1)); 
    DEF_CRG1(value, 1, CrgRange(0x2, 0x2));
    DEF_CRG1(saturate, 1, CrgRange(0, 0));
}

void STECmdIncAdd_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_INC_ADD));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size1.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size2.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 2)); 
    DEF_CRG1(value, 1, CrgRange(0xffffffff, 0xffffffff));
    DEF_CRG1(saturate1, 1, CrgRange(0, 0));
    DEF_CRG1(saturate2, 1, CrgRange(1, 1));
}

void STECmdIncSub_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_INC_SUB));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size1.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size2.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 2)); 
    DEF_CRG1(value, 1, CrgRange(0xffffffff, 0xffffffff));
    DEF_CRG1(saturate1, 1, CrgRange(1, 1));
    DEF_CRG1(saturate2, 1, CrgRange(0, 0));
}

void STECmdDecAdd_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_DEC_ADD));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size1.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size2.get_crg_var(), 1,
                 CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 2)); 
    DEF_CRG1(value, 1, CrgRange(0x2, 0x2));
    DEF_CRG1(saturate1, 1, CrgRange(0, 0));
    DEF_CRG1(saturate2, 1, CrgRange(1, 1));
}

void STECmdDecSub_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_DEC_SUB));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size1.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size2.get_crg_var(), 1,
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 2)); 
    DEF_CRG1(value, 1, CrgRange(0xffffffff, 0xffffffff));
    DEF_CRG1(saturate1, 1, CrgRange(0, 1));
    DEF_CRG1(saturate2, 1, CrgRange(0, 1));
}

void STECmdTaskJobSync_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_BARRIER));
}

void STECmdReadError_CMDClass::	set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_READ_ERRORS));
}

void STECmdClearError_CMDClass::set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_CLEAR_ERRORS));
}

void STECmdBADCMD_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(STE_CMDTYPE_BAD_CMD));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size1.get_crg_var(), 4,
                  CrgRange (STE_COUNTERTYPE_1BYTE, STE_COUNTERTYPE_1BYTE),
                  CrgRange (STE_COUNTERTYPE_2BYTE, STE_COUNTERTYPE_2BYTE),
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE),
                  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, 
                  Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, 
                  Crg_NOT_SORT, Crg_NO_DET_VARS, &size2.get_crg_var(), 4,
                  CrgRange (STE_COUNTERTYPE_1BYTE, STE_COUNTERTYPE_1BYTE),
                  CrgRange (STE_COUNTERTYPE_2BYTE, STE_COUNTERTYPE_2BYTE),
                  CrgRange (STE_COUNTERTYPE_4BYTE, STE_COUNTERTYPE_4BYTE),
                  CrgRange (STE_COUNTERTYPE_8BYTE, STE_COUNTERTYPE_8BYTE));
    DEF_CRG1(counterId, 1, CrgRange(0, STE_MAX_COUNTERS - 2)); 
    DEF_CRG1(value, 1, CrgRange(0, 0xffffffff));
    DEF_CRG1(saturate1, 1, CrgRange(0, 1));
    DEF_CRG1(saturate2, 1, CrgRange(0, 1));
}

void FDMACmdEnqueueWorkingFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_WORKING_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(6, 6));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0));
    DEF_CRG1(PS, 1, CrgRange(0, 0));
    DEF_CRG1(TC, 1, CrgRange(0, 0));
    DEF_CRG1(EIS, 1, CrgRange(1, 1));
    DEF_CRG1(spid, 1, CrgRange(0, 0));
}


void STECmdClass::set_constraints(){
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &cmdType.get_crg_var(), 12,\
					  CrgRange (STE_CMDTYPE_SET_4B,     STE_CMDTYPE_SET_4B),\
					  CrgRange (STE_CMDTYPE_SET_8B,     STE_CMDTYPE_SET_8B),\
					  CrgRange (STE_CMDTYPE_ADD,        STE_CMDTYPE_ADD),\
					  CrgRange (STE_CMDTYPE_DEC,        STE_CMDTYPE_DEC),\
					  CrgRange (STE_CMDTYPE_INC_ADD,    STE_CMDTYPE_INC_ADD),\
					  CrgRange (STE_CMDTYPE_INC_SUB,    STE_CMDTYPE_INC_SUB),\
					  CrgRange (STE_CMDTYPE_DEC_ADD,    STE_CMDTYPE_DEC_ADD),\
					  CrgRange (STE_CMDTYPE_DEC_SUB,    STE_CMDTYPE_DEC_SUB),\
					  CrgRange (STE_CMDTYPE_BARRIER,    STE_CMDTYPE_BARRIER),\
					  CrgRange (STE_CMDTYPE_READ_ERRORS,	STE_CMDTYPE_READ_ERRORS),\
					  CrgRange (STE_CMDTYPE_CLEAR_ERRORS,   STE_CMDTYPE_CLEAR_ERRORS),\
					  CrgRange (STE_CMDTYPE_BAD_CMD,   STE_CMDTYPE_BAD_CMD));
}

void STECmdListClass::set_constraints()
{
    //maxNumOfCmd must be greater than STE_MAX_COUNTERS
    DEF_CRG1(maxNumOfCmds, 1, CrgRange(STE_MAX_COMMANDS, STE_MAX_COMMANDS));
    DEF_CRG1(RandomizeCmds, 1, CrgRange(0, 0));
}

void   TestSysParamClass:: set_constraints() {
	int i;
	DEF_CRG1( Flow_Type, 1, CrgRange(CPU_OP_CPU,CPU_OP_CPU));
	DEF_CRG1( no_of_frames, 1, CrgRange (1,1) );
	DEF_CRG1( addFCS, 1, CrgRange(0,0));//Jephy_FCS
	DEF_CRG1( BufferSize, 1, CrgRange(2048,2048));//if OIM is 128, at least 448 (448,4096)
	DEF_CRG1( FD_Offset, 1, CrgRange(64,64));
	DEF_CRG1( Test_Type, 1, CrgRange (SINGLE_FLOW,SINGLE_FLOW) );
	DEF_CRG1( Mult_Queues_N_1, 1, CrgRange(0,0));
	DEF_CRG1( Mult_Queues_1_N, 1, CrgRange(0,0));
}

void   FrameInputClass:: set_constraints() {
	int i;
	int test_type = randomInt(1,1);
	if(test_type==1)
	{
		for(i=0;i<MAX_NO_OF_FRAMES;i++)
		{
			//DEF_CRG1( FT, 1, CrgRange (Ether_IPv4,Ether_IPv4) );		
			Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
						  Crg_NOT_SORT, Crg_NO_DET_VARS, &FT[i].get_crg_var(), 4,\
						  CrgRange (Ether_IPv4_UDP, Ether_IPv4_UDP),\
						  CrgRange (Ether_IPv4_UDP, Ether_IPv4_UDP),\
						  CrgRange (Ether_IPv4_UDP, Ether_IPv4_UDP),\
						  CrgRange (Ether_IPv4_UDP,Ether_IPv4_UDP));
		}							 
			
		DEF_CRG1( VLAN, 1, CrgRange (no_vlan,no_vlan) );
	}else
	{
		for(i=0;i<MAX_NO_OF_FRAMES;i++)
		{
			Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
						  Crg_NOT_SORT, Crg_NO_DET_VARS, &FT[i].get_crg_var(), 2,\
						  CrgRange (Ether_VTag_IPv4, Ether_VTag_IPv4),\
						  CrgRange (Ether_VTag_IPv4_UDP,Ether_VTag_IPv4_UDP));
		}
		DEF_CRG1( VLAN, 1, CrgRange (vlan1,vlan6) );
	}

//	DEF_CRG1( MPLSNum, 1, CrgRange (0,0) );
//	DEF_CRG1( number_of_frame, 1, CrgRange (1,1) );


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
								  CrgRange (300,400),\
								  CrgRange (300,400));

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
//	DEF_CRG1( NextHeader, 1, CrgRange (0,0) );
//	DEF_CRG1( PayloadLength, 1, CrgRange (0,0) );
//	DEF_CRG1( Reserved, 1, CrgRange (0,0) );
//	DEF_CRG1( SecurityParametersIndex, 1, CrgRange (0,0) );
//	DEF_CRG1( SequenceNumber, 1, CrgRange (0,0) );
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

#endif //__AIOP_ROC_STE_H

