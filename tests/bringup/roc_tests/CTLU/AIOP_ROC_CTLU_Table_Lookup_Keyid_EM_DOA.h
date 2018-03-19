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
@File		AIOP_ROC_CTLU_Table_Lookup_Keyid_EM_DOA.h

@Description	This file contains CTLU DOA test.

*//***************************************************************************/

/**********************************************************************************************/
/* Frame type: 																				  */
/* 1:IPv4/IPv4 UDP or TCP or GRE wi/wo Vtag		2:IPv6/IPv6 UDP or TCP or GRE wi/wo Vtag      */
/* IPv6 will have all combination of Extension as fragmented and unfragmented part			  */
/* MTU=[256,1500] Payload=[800,3000 ]													  */
/**********************************************************************************************/
#ifndef __AIOP_ROC_CTLU_CONSTRAINT_H
#define __AIOP_ROC_CTLU_CONSTRAINT_H

#include "common.h"
#include "tlu_param_checker.h"
#include "aiop_common_parameters.h"
#include "frame_generators.h"
#include "frame_headers.h"

static uint32_t id;

#define CONSTRAINT_NAME "AIOP_ROC_CTLU_Table_Lookup_Keyid_EM_DOA.h"
static char *__CRG_FILE_NAME = (char *)"AIOP_ROC_CTLU_Table_Lookup_Keyid_EM_DOA.h";

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
	DEF_CRG1( TLU_Flow_Type, 1, CrgRange(CPU_OP_CPU,CPU_OP_CPU));
	DEF_CRG1( no_of_frames, 1, CrgRange (1,1) );
	DEF_CRG1( addFCS, 1, CrgRange(0,0));//Jephy_FCS
	DEF_CRG1( BufferSize, 1, CrgRange(2048,2048));//if OIM is 128, at least 448 (448,4096)
	CrgWeightCase (__CRG_FILE_NAME, __LINE__, CrgSetBase, Crg_NOT_BY_ARR, Crg_NOT_SORT, &DF_Test.get_crg_var(), 2, CrgCreateWeight(__CRG_FILE_NAME, __LINE__,0,0,7,1), CrgCreateWeight(__CRG_FILE_NAME, __LINE__,1,1,3,1));	
	DEF_CRG1( DF_Test, 1, CrgRange(0,0));//only enable for IPv4 test
	DEF_CRG1( DF_Discard, 1, CrgRange(0,0));
	Crg_Range_Con (__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &TLU_FOF.get_crg_var(), 2, CrgRange (96, 96),CrgRange (112,112));//,CrgRange (128,128)); if FOF=128+32+maxheader(38+60)=258 > 256
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

void   FrameInputClass:: set_constraints()
{
	int i;
	
	for(i=0;i<MAX_NO_OF_FRAMES;i++)
	{
		DEF_CRG1(FT[i], 1, CrgRange(Ether_IPv4_UDP,Ether_IPv4_UDP));	
		//DEF_CRG1(FT[i], 1, CrgRange(Ether_IPv6_UDP,Ether_IPv6_UDP));
		DEF_CRG1( VLAN[i], 1, CrgRange (no_vlan,no_vlan) );
		DEF_CRG1( MPLSNum[i], 1, CrgRange (0,0) );		
	}
}

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
    DEF_CRG1(TC, 1, CrgRange(0, 0));
    DEF_CRG1(EIS, 1, CrgRange(1, 1));

    //DEF_CRG1(reserved, 1, CrgSingleton(0));
};

/* ----------- NEW ------------------*/
/*20. FDMA cmd replace segment*/
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

void GeneralLookupFCV1_FCVClass :: set_constraints()
{
    DEF_CRG1(flctypv, 1, CrgRange(0, 1));
	DEF_CRG1(plidv, 1, CrgRange(0, 1));
	DEF_CRG1(qosmmv, 1, CrgRange(0, 1));
	DEF_CRG1(ifpidv, 1, CrgRange(0, 1));
	DEF_CRG1(rplidv, 1, CrgRange(0, 1));
	DEF_CRG1(epidv, 1, CrgRange(0, 1));
	DEF_CRG1(flctyp, 1, CrgRange(0, 2));
	DEF_CRG1(disc, 1, CrgRange(0, 1));
	DEF_CRG1(qpri, 1, CrgRange(0x0, 0xf));
	DEF_CRG1(dd, 1, CrgRange(0x0,0xf));
	DEF_CRG1(sc, 1, CrgRange(0, 1));
	DEF_CRG1(dropp, 1, CrgRange(0, 7));
	DEF_CRG1(qpriv, 1, CrgRange(0, 1));
	DEF_CRG1(ddv, 1, CrgRange(0, 1));
	DEF_CRG1(scv, 1, CrgRange(0, 1));
	DEF_CRG1(droppv, 1, CrgRange(0, 1));
	DEF_CRG1(hkidv, 1, CrgRange(0, 1));
    DEF_CRG1(qdidv, 1, CrgRange(0, 1));
	DEF_CRG1(tkidv, 1, CrgRange(0, 1));
	DEF_CRG1(hkid, 1, CrgRange(0x0, 0xff));
	DEF_CRG1(qdid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(tid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(kid, 1, CrgRange(0x0, 0xff));
	DEF_CRG1(plid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(qosmm, 1, CrgRange(0, 2));
	DEF_CRG1(ipre, 1, CrgRange(0, 1));
	DEF_CRG1(ifpid, 1, CrgRange(0x0, 0xfff));
	DEF_CRG1(rplid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(epid, 1, CrgRange(0x0, 0xffff));

}

void GeneralLookupFCV2_FCVClass :: set_constraints()
{
	DEF_CRG1(plidv, 1, CrgRange(0, 1));
	DEF_CRG1(qosmmv, 1, CrgRange(0, 1));
	DEF_CRG1(ifpidv, 1, CrgRange(0, 1));
	DEF_CRG1(flcv, 1, CrgRange(0, 1));
	DEF_CRG1(epidv, 1, CrgRange(0, 1));
	DEF_CRG1(disc, 1, CrgRange(0, 1));
	DEF_CRG1(qpri, 1, CrgRange(0x0, 0xf));
	DEF_CRG1(cbmt, 1, CrgRange(0, 1));
	DEF_CRG1(sc, 1, CrgRange(0, 1));
	DEF_CRG1(dropp, 1, CrgRange(0, 7));
	DEF_CRG1(qpriv, 1, CrgRange(0, 1));
	DEF_CRG1(ddv, 1, CrgRange(0, 1));
	DEF_CRG1(scv, 1, CrgRange(0, 1));
	DEF_CRG1(droppv, 1, CrgRange(0, 1));
	DEF_CRG1(hkidv, 1, CrgRange(0, 1));
    DEF_CRG1(qdidv, 1, CrgRange(0, 1));
	DEF_CRG1(hkid, 1, CrgRange(0x0, 0xff));
	DEF_CRG1(qdid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(plid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(qosmm, 1, CrgRange(0, 2));
	DEF_CRG1(ipre, 1, CrgRange(0, 1));
	DEF_CRG1(ifpid, 1, CrgRange(0x0, 0xfff));
	DEF_CRG1(swflc_0, 1, CrgRange(0x0, 0xffffffff));
	DEF_CRG1(swflc_1, 1, CrgRange(0x0, 0xffffffff));
}

void GeneralLookupFCV3_FCVClass :: set_constraints()
{
	DEF_CRG1(plidv, 1, CrgRange(0, 1));
	DEF_CRG1(qosmmv, 1, CrgRange(0, 1));
	DEF_CRG1(ifpidv, 1, CrgRange(0, 1));
	DEF_CRG1(flcv, 1, CrgRange(0, 1));
	DEF_CRG1(epidv, 1, CrgRange(0, 1));
	DEF_CRG1(disc, 1, CrgRange(0, 1));
	DEF_CRG1(qpri, 1, CrgRange(0x0, 0xf));
	DEF_CRG1(cbmt, 1, CrgRange(0, 1));
	DEF_CRG1(sc, 1, CrgRange(0, 1));
	DEF_CRG1(dropp, 1, CrgRange(0, 7));
	DEF_CRG1(qpriv, 1, CrgRange(0, 1));
	DEF_CRG1(ddv, 1, CrgRange(0, 1));
	DEF_CRG1(scv, 1, CrgRange(0, 1));
	DEF_CRG1(droppv, 1, CrgRange(0, 1));
	DEF_CRG1(hkidv, 1, CrgRange(0, 1));
    DEF_CRG1(qdbinv, 1, CrgRange(0, 1));
	DEF_CRG1(hkid, 1, CrgRange(0x0, 0xff));
	DEF_CRG1(qdbin, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(plid, 1, CrgRange(0x0, 0xffff));
	DEF_CRG1(qosmm, 1, CrgRange(0, 2));
	DEF_CRG1(ipre, 1, CrgRange(0, 1));
	DEF_CRG1(ifpid, 1, CrgRange(0x0, 0xfff));
	DEF_CRG1(swflc_0, 1, CrgRange(0x0, 0xffffffff));
	DEF_CRG1(swflc_1, 1, CrgRange(0x0, 0xffffffff));
}

void GeneralLookupFCV4_FCVClass :: set_constraints()
{
	DEF_CRG1(op2v, 1, CrgRange(0, 1));
	DEF_CRG1(opaque1_valid_bits, 1, CrgRange(0, 0xff));
    DEF_CRG1(disc, 1, CrgRange(0, 1));
	DEF_CRG1(opaque2, 1, CrgRange(0, 0xff));
	DEF_CRG1(tkidv, 1, CrgRange(0, 1));
	DEF_CRG1(chained_table_id, 1, CrgRange(0, 0xffff));
	DEF_CRG1(chained_key_id, 1, CrgRange(0, 0xff));
	DEF_CRG1(opaque1_0, 1, CrgRange(0, 0xffffffff));
	DEF_CRG1(opaque1_1, 1, CrgRange(0, 0xffffffff));
}

void GeneralLookupFCV5_FCVClass :: set_constraints()
{
	DEF_CRG1(op2v, 1, CrgRange(0, 1));
	DEF_CRG1(opaque1_valid_bits, 1, CrgRange(0, 0xff));
    DEF_CRG1(disc, 1, CrgRange(0, 0));  // no disc bit in ctlu 0.7
    DEF_CRG1(opaque2, 1, CrgRange(0, 0xff));
	DEF_CRG1(reference_pointer_0, 1, CrgRange(0, 0xffff));
	DEF_CRG1(reference_pointer_1, 1, CrgRange(0, 0xffffffff));
	DEF_CRG1(opaque1_0, 1, CrgRange(0, 0xffffffff));
	DEF_CRG1(opaque1_1, 1, CrgRange(0, 0xffffffff));
}

void GeneralLookupFCV6_FCVClass :: set_constraints()
{
	DEF_CRG1(op2v, 1, CrgRange(0, 1));  // op2v is fixed to 1, not support randomization
	DEF_CRG1(opaque1_valid_bits, 1, CrgRange(0x0, 0xff));
    DEF_CRG1(disc, 1, CrgRange(0, 0));  // no disc bit in ctlu 0.7
	DEF_CRG1(opaque2, 1, CrgRange(0x0, 0xff));
	DEF_CRG1(opaque0_0, 1, CrgRange(0x0, 0xffffffff));
	DEF_CRG1(opaque0_1, 1, CrgRange(0x0, 0xffffffff));
	DEF_CRG1(opaque1_0, 1, CrgRange(0x0, 0xffffffff));
	DEF_CRG1(opaque1_1, 1, CrgRange(0x0, 0xffffffff));
}

void CTLUCmdKCRBuilderInit_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_INIT));
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));
}

void CTLUCmdKCRBuilderAddConstantFec_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_CONSTANT_FEC));
    
    DEF_CRG1(constant, 1, CrgRange(0, 0)); //EM
	DEF_CRG1(num, 1, CrgRange(4, 4));
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));
}

void CTLUCmdKCRBuilderAddPrtclBasedGnrcFec_CMDClass :: set_constraints()
{
    uint8_t i;	

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_PRTCL_BASED_GNRC_FEC));
    //DEF_CRG1(pr_offset, 1, CrgRange(0, 0x3f));//for error flow(0, 0xff)
    
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
                        Crg_NOT_SORT, Crg_NO_DET_VARS, &pr_offset.get_crg_var(), 5,\
                        CrgRange (0x2,0x2),\
                        CrgRange (0x4,0x4),\
                        CrgRange (0x10,0x20),\
                        CrgRange (0x22,0x23),\
                        CrgRange (0x27,0x27));//for error flow(0, 0xff)

    DEF_CRG1(extract_offset, 1, CrgRange(0, 0xf));//for error flow(0, 0xff)
	DEF_CRG1(extract_size, 1, CrgRange(1, 0xf));//for error flow(0, 0xff)
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));

	DEF_CRG1(num_of_masks, 1, CrgRange(0, 4));//for error flow(0, 0xff)
	for (i = 0; i < 4; i++)
    {
        DEF_CRG1(mask[i], 1, CrgRange(0, 0xff));
		DEF_CRG1(mask_offset[i], 1, CrgRange(0, 0xf));//for error flow(0, 0xff)
    }
}

void CTLUCmdKCRBuilderAddLkupResultFec_CMDClass:: set_constraints()
{
    uint8_t i;

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_LKUP_RESULT_FEC));

    DEF_CRG1(extract_field, 1, CrgRange(0, 0xff));//for error flow(0, 0xff)
	DEF_CRG1(extract_size_in_opaque, 1, CrgRange(1, 0xff));//for error flow(0, 0xff)
	DEF_CRG1(offset_in_opaque, 1, CrgRange(0, 0xff));//for error flow(0, 0xff)
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));

	DEF_CRG1(num_of_masks, 1, CrgRange(0, 4));//for error flow(0, 0xff)
	for (i = 0; i < 4; i++)
    {
        DEF_CRG1(mask[i], 1, CrgRange(0, 0xff));
		DEF_CRG1(mask_offset[i], 1, CrgRange(0, 0xf));//for error flow(0, 0xff)
    }
}

void CTLUCmdKCRBuilderAddValidFieldFec_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_VALID_FIELD_FEC));

    DEF_CRG1(mask, 1, CrgRange(0, 0xff));//for error flow(0, 0xff)
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));
}

void CTLUCmdKCRBuilderAddGenericExtractFEC_CMDClass :: set_constraints()
{
    uint8_t i;

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC));
    DEF_CRG1(extract_offset, 1, CrgRange(0, 0xff));
	DEF_CRG1(extract_size, 1, CrgRange(1, 0xf));
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &gec_source.get_crg_var(), 2,\
					  CrgRange (KEYGEN_KCR_GEC_PARSE_RES,KEYGEN_KCR_GEC_PARSE_RES),\
					  CrgRange (KEYGEN_KCR_GEC_FRAME,KEYGEN_KCR_GEC_FRAME));
	DEF_CRG1(num_of_masks, 1, CrgRange(0, 4));
	for (i = 0; i < 4; i++)
    {
        DEF_CRG1(mask[i], 1, CrgRange(0, 0xff));
		DEF_CRG1(mask_offset[i], 1, CrgRange(0, 0xf));
    }
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));

}

void CTLUCmdKCRBuilderAddInputValueFec_CMDClass :: set_constraints()
{
    uint8_t i;

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_INPUT_VALUE_FEC));

    DEF_CRG1(extract_offset, 1, CrgRange(0, 4));
    DEF_CRG1(extract_size, 1, CrgRange(4, 4));
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));

	DEF_CRG1(num_of_masks, 1, CrgRange(4, 4));
	for (i = 0; i < 3; i++)
    {		
	DEF_CRG1(mask_offset[i], 1, CrgRange(i,i));
	DEF_CRG1(mask[i], 1, CrgRange(0x0, 0x0));             
    }

	DEF_CRG1(mask_offset[3], 1, CrgRange(3,3));
	DEF_CRG1(mask[3], 1, CrgRange(0x1, 0x1));
}

// jassay
void CTLUCmdKCRBuilderAddProtocolSpecificFiled_CMDClass :: set_constraints()
{
	uint8_t i;

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FIELD));
	DEF_CRG1(num_of_masks, 1, CrgRange(4, 4));  
	DEF_CRG1(FecId, 1, CrgRange(KEYGEN_KCR_IPDST_1_FECID, KEYGEN_KCR_IPDST_1_FECID));
	for (i = 0; i < 3; i++)
    {		
	DEF_CRG1(mask_offset[i], 1, CrgRange(i,i));
	DEF_CRG1(mask[i], 1, CrgRange(0x0, 0x0));             
    }

	DEF_CRG1(mask_offset[3], 1, CrgRange(3,3));
	DEF_CRG1(mask[3], 1, CrgRange(0x1, 0x1));    

    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));
}

// jassay_kcr_create
void CTLUCmdKeyCompositionCreate_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KEY_COMPOSITION_CREATE));
	DEF_CRG1(kid, 1, CrgRange(0x0, 0xf));
    DEF_CRG1(isFixSortValue, 1, CrgRange(0, 0));
}

void CTLUCmdKeyCompositionQuery_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KEY_COMPOSITION_QUERY));
	DEF_CRG1(kid, 1, CrgRange(0x0, 0xf));
    DEF_CRG1(isFixSortValue, 1, CrgRange(0, 0));
}

void CTLUCmdKeyCompositionDelete_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KEY_COMPOSITION_DELETE));
	DEF_CRG1(kid, 1, CrgRange(0x0, 0xf));
    DEF_CRG1(isFixSortValue, 1, CrgRange(0, 1));
}

// jassay_kcr_replace
void CTLUCmdKeyCompositionReplace_CMDClass :: set_constraints()
{

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_KEY_COMPOSITION_REPLACE));
/*
 	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &cmd.get_crg_var(), 2,\
					  CrgRange (e_AIOP_CTLU_KEY_COMPOSITION_CREATE,e_AIOP_CTLU_KEY_COMPOSITION_CREATE),\
					  CrgRange (e_AIOP_CTLU_KEY_COMPOSITION_REPLACE,e_AIOP_CTLU_KEY_COMPOSITION_REPLACE));
*/
	DEF_CRG1(kid, 1, CrgRange(0, 4));    
    DEF_CRG1(isFixSortValue, 1, CrgRange(1, 1));
}

void IFcmdStatement_CMDClass:: set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_IF_STATEMENT));
	DEF_CRG1(compared_size, 1, CrgRange(COMPARE_4BYTE, COMPARE_4BYTE));    
    DEF_CRG1(compared_value, 1, CrgRange(KEYGEN_KCR_SIZE_ERR, KEYGEN_KCR_SIZE_ERR));
    DEF_CRG1(compared_variable_addr, 1, CrgRange(8, 8));//hogan, the offset of status parameter from the repeat cmd
    DEF_CRG1(true_cmd_offset, 1, CrgRange(8, 8));//hogan, the offset from the repeat cmd
    DEF_CRG1(cond, 1, CrgRange(COND_NON_EQUAL, COND_NON_EQUAL));
}

void CTLUCmdTblException_CMDClass:: set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_EXCEPTION_MODULE));
	DEF_CRG1(flags, 1, CrgRange(FSL_VERIF_FATAL_FLAG_BUFF_CTX_TEST, FSL_VERIF_FATAL_FLAG_BUFF_CTX_TEST));
	DEF_CRG1(numOfCmd, 1, CrgRange(1,1));
}

void CTLUCmdTblCrt_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_LOOKUP_TABLE_CREATE));
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &stdy.get_crg_var(), 2,\
					  CrgRange (0,0),\
					  CrgRange (2,2));
#if 1
    //type 4 and 2 is not supported on the current simulator version(T4LS_SIM-20131213-AFM_ENGG_BUILD)
    DEF_CRG1(type, 1, CrgRange(0, 0));//fix to EM; 
#else
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
                    Crg_NOT_SORT, Crg_NO_DET_VARS, &type.get_crg_var(), 2,\
                    CrgRange (0,2),\
                    CrgRange (4,4));
#endif
#if 1
    /* not enough memory resource when table is in external memory
    aiop: Table is in external memory
    aiop: Memory required for committed Entries is 6912 bytes
    aiop: Memory available for Entries is 1152 bytes
    aiop: Table create permanently failed, not enough memory resource, Write MGCF as 1, status[4:5] as 1
    */
    DEF_CRG1(iex, 1, CrgRange(3, 5));
#else
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &iex.get_crg_var(), 3,\
					  CrgRange (2,2),\
					  CrgRange (3,3),\
					  CrgRange (4,4));
#endif
    //The committed_rules, max_rules and key_size needs confirm the scope of parameter
	//DEF_CRG1(mres, 1, CrgRange(2, 2));
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
				  Crg_NOT_SORT, Crg_NO_DET_VARS, &mres.get_crg_var(), 2,\
				  CrgRange (0,0),\
				  CrgRange (2,2));

	DEF_CRG1(agt, 1, CrgRange(0, 0));
	DEF_CRG1(committed_rules, 1, CrgRange(10, 15));
	DEF_CRG1(max_rules, 1, CrgRange(20, 30));

	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
										  Crg_NOT_SORT, Crg_NO_DET_VARS, &key_size.get_crg_var(), 2,\
										  CrgRange (4, 4),\
										  CrgRange (16, 16));		// EM on IPv4 or IPv6	
   
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &fcv_type.get_crg_var(), 1,\
					  CrgRange (e_AIOP_CTLU_GENERAL_LOOKUP_FCV5,e_AIOP_CTLU_GENERAL_LOOKUP_FCV6));
	

}

void CTLUCmdTblReplaceMR_CMDClass :: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(flags, 1, CrgRange(0, 1));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_REPLACE_MISS_RULE));
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
                    Crg_NOT_SORT, Crg_NO_DET_VARS, &fcv_type.get_crg_var(), 1,\
                    CrgRange (e_AIOP_CTLU_GENERAL_LOOKUP_FCV5,e_AIOP_CTLU_GENERAL_LOOKUP_FCV6));
}

void CTLUCmdTblGetParams_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_TABLE_GET_PARAMS));
}

void CTLUCmdTblGetMR_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_TABLE_GET_MISS_RULE));

}

void CTLUCmdTblDelete_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_LOOKUP_TABLE_DELETE));
}

void CTLUCmdTblRuleCrt_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_RULE_CREATE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
                    Crg_NOT_SORT, Crg_NO_DET_VARS, &fcv_type.get_crg_var(), 1,\
                    CrgRange (e_AIOP_CTLU_GENERAL_LOOKUP_FCV5,e_AIOP_CTLU_GENERAL_LOOKUP_FCV6));

}

void CTLUCmdTblRuleCrtOrRep_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(flags, 1, CrgRange(0, 1));
	DEF_CRG1(createrule, 1, CrgRange(0, 1));	
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_RULE_CREATE_Or_REPLACE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
                    Crg_NOT_SORT, Crg_NO_DET_VARS, &fcv_type.get_crg_var(), 1,\
                    CrgRange (e_AIOP_CTLU_GENERAL_LOOKUP_FCV5,e_AIOP_CTLU_GENERAL_LOOKUP_FCV6));
}

void CTLUCmdTblRuleRep_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(flags, 1, CrgRange(0, 1));	
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_RULE_REPLACE));
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
                    Crg_NOT_SORT, Crg_NO_DET_VARS, &fcv_type.get_crg_var(), 1,\
                    CrgRange (e_AIOP_CTLU_GENERAL_LOOKUP_FCV5,e_AIOP_CTLU_GENERAL_LOOKUP_FCV6));
}

void CTLUCmdTblRuleQry_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_RULE_QUERY));
}

void CTLUCmdTblRuleDel_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_RULE_DELETE));
	DEF_CRG1(flags, 1, CrgRange(0, 1));
    
}

void CTLUCmdGenKey_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));

	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_GEN_KEY));
    DEF_CRG1(isFixSortValue, 1, CrgRange(0, 1));

    DEF_CRG1(opaquein_0, 1, CrgRange(0x0, 0xffffffff));
    DEF_CRG1(opaquein_1, 1, CrgRange(0x0, 0xffffffff));
}

void CTLUCmdGenHash_CMDClass:: set_constraints()
{
    uint8_t test_type = randomInt(0,4);

	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_GEN_HASH));
    DEF_CRG1(randkeysize, 1, CrgRange(32, 32));//this bit is valid if israndkeysize is 1
    DEF_CRG1(israndkeysize, 1, CrgRange(0, 0));
    DEF_CRG1(isFixSortValue, 1, CrgRange(0, 0));

}

void CTLUCmdTblLookupKey_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_TABLE_LOOKUP_KEY));
}

void CTLUCmdTblLookupKeyID_DefaultFrame_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_TABLE_LOOKUP_DEFAULT_FRAME_KEYID));
}

void CTLUCmdTblLookupKeyID_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_TABLE_LOOKUP_KEYID));
	DEF_CRG1(ndf_params_input, 1, CrgRange(0, 1));
}

void CTLUCmdParserInit_CMDClass:: set_constraints()
{	
	DEF_CRG1(parser_starting_hxs, 1, CrgRange(PARSER_ETH_STARTING_HXS, PARSER_ETH_STARTING_HXS));
}

void CTLUCmdTblQuery_CMDClass:: set_constraints()
{
	DEF_CRG1(acc_id, 1, CrgRange(TABLE_ACCEL_ID_CTLU, TABLE_ACCEL_ID_CTLU));
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CTLU_TABLE_QUERY_DEBUG_VERIF));
}


void CTLUCmdListClass::set_constraints()
{
    DEF_CRG1(maxNumOfCmd, 1, CrgRange(3, 3));
}

void CTLUCmdClass::set_constraints(){
/*
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &CMD.get_crg_var(), 3,\
					  CrgRange (e_AIOP_CTLU_RULE_CREATE,e_AIOP_CTLU_RULE_CREATE),\
					  CrgRange (e_AIOP_CTLU_RULE_CREATE,e_AIOP_CTLU_RULE_CREATE),\
					  CrgRange (e_AIOP_CTLU_RULE_CREATE,e_AIOP_CTLU_RULE_CREATE));
*/
    uint8_t test_type = randomInt(1,5);

    DEF_CRG1(CTLU_Table_Exception_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Create_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_Table_Replace_MissRule_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Get_Parmas_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Get_MissRule_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Delete_CMD, 1, CrgRange(0, 0));

     
    DEF_CRG1(CTLU_Table_Rule_Create_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_Table_Rule_CreateOrRplc_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Rule_Replace_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Rule_Query_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Rule_Delete_CMD, 1, CrgRange(0, 0));
    
    DEF_CRG1(CTLU_Parser_Init_CMD, 1, CrgRange(0, 0));

    DEF_CRG1(CTLU_Table_Lkup_ByKeyid_DefaultFrame_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Table_Lkup_ByKeyid_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_Table_Lkup_ByKey_CMD, 1, CrgRange(0, 0));

    DEF_CRG1(CTLU_Table_Query_CMD, 1, CrgRange(0, 0));

    if (0 == test_type)
    {
        DEF_CRG1(IF_STATEMENT_CMD, 1, CrgRange(1, 1));
    }
    else
    {
        DEF_CRG1(IF_STATEMENT_CMD, 1, CrgRange(0, 0));
    }

    //gen cmd
    DEF_CRG1(CTLU_Gen_Key_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_Gen_Hash_CMD, 1, CrgRange(0, 0));
    
    DEF_CRG1(CTLU_KEY_ID_POOL_CREATE_CMD, 1, CrgRange(0, 0)); // jassay_kcr_create
    DEF_CRG1(CTLU_KCR_Bldr_Init_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_KCR_Bldr_Add_CnstntFec_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Bldr_Add_InputValueFec_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_KCR_Bldr_Add_Prtcl_SpcFld_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_KCR_Bldr_Add_Prtcl_BsdGnrcFec_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Bldr_Add_GnrcExtractFec_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Bldr_Lkup_RsltFldFec_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Bldr_Vld_FldFec_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Create_CMD, 1, CrgRange(1, 1));
    DEF_CRG1(CTLU_KCR_Replace_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Delete_CMD, 1, CrgRange(0, 0));
    DEF_CRG1(CTLU_KCR_Query_CMD, 1, CrgRange(0, 0));
/*

CrgNum CTLU_Table_Create_CMD;
CrgNum CTLU_Table_Replace_MissRule_CMD;
CrgNum CTLU_Table_Get_Parmas_CMD;
CrgNum CTLU_Table_Get_MissRule_CMD;
CrgNum CTLU_Table_Delete_CMD;

CrgNum CTLU_Table_Rule_Create_CMD;
CrgNum CTLU_Table_Rule_CreateOrRplc_CMD;
CrgNum CTLU_Table_Rule_Replace_CMD;
CrgNum CTLU_Table_Rule_Delete_CMD;

CrgNum CTLU_Table_Lkup_ByKeyid_DefaultFrame_CMD;
CrgNum CTLU_Table_Lkup_ByKey_CMD;

CrgNum CTLU_Table_Query_CMD;

//gen
CrgNum CTLU_Gen_Key_CMD;
CrgNum CTLU_Gen_Hash_CMD;

//Kcr
CrgNum CTLU_KCR_Bldr_Init_CMD;
CrgNum CTLU_KCR_Bldr_Add_CnstntFec_CMD;
CrgNum CTLU_KCR_Bldr_Add_Prtcl_SpcFld_CMD;
CrgNum CTLU_KCR_Bldr_Add_Prtcl_BsdGnrcFec_CMD;
CrgNum CTLU_KCR_Bldr_Add_GnrcExtractFec_CMD;
CrgNum CTLU_KCR_Bldr_Lkup_RsltFldFec_CMD;
CrgNum CTLU_KCR_Bldr_Vld_FldFec_CMD;
CrgNum CTLU_KCR_Create_CMD;
CrgNum CTLU_KCR_Replace_CMD;
CrgNum CTLU_KCR_Delete_CMD;
CrgNum CTLU_KCR_Query_CMD;
*/
}

void 	EtherHeaderClass::set_constraints(){
	DEF_CRG1( da[0], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( da[1], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( da[2], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( da[3], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( da[4], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( da[5], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( sa[0], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( sa[1], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( sa[2], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( sa[3], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( sa[4], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( sa[5], 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( length, 1, CrgRange (0,0xffff) );//10
//	CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, type.r, 3, CrgRange (0x0, 0x7ff),CrgRange (0x801,0x8863),CrgRange (0x8865,0xffff)); //exculinf TCP,UDP,SCTP
//	DEF_CRG1( da_match.r, 1, CrgRange (0,1) );//10
}

void 	 ArpHeaderClass::set_constraints(){
	DEF_CRG1(HardwareType, 1, CrgRange (1,1) );
	DEF_CRG1( ProtocolType, 1, CrgRange (0x0800,0x0800) );
	DEF_CRG1( HardwareAddressLength, 1, CrgRange (6,6) );
	DEF_CRG1( ProtocolAddressLength, 1, CrgRange (4,4) );
	DEF_CRG1( Operation, 1, CrgRange (1,4) );

	DEF_CRG1( SenderMac[0], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderMac[1], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderMac[2], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderMac[3], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderMac[4], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderMac[5], 1, CrgRange (0x0,0xff) );

	DEF_CRG1( SenderIP[0], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderIP[1], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderIP[2], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( SenderIP[3], 1, CrgRange (0x0,0xff) );

	DEF_CRG1( TargetMac[0], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetMac[1], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetMac[2], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetMac[3], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetMac[4], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetMac[5], 1, CrgRange (0x0,0xff) );
	
	DEF_CRG1( TargetIP[0], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetIP[1], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetIP[2], 1, CrgRange (0x0,0xff) );
	DEF_CRG1( TargetIP[3], 1, CrgRange (0x0,0xff) );

}

void   IPv4HeaderClass:: set_constraints() {
	int i;
	DEF_CRG1( Version, 1, CrgSingleton (4) );//Y
	DEF_CRG1( IHL, 1, CrgRange (5,5) );//Y
	DEF_CRG1( TOS, 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( Length, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( Id, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( Flags, 1, CrgRange (0,0) );//10
	DEF_CRG1( FragmentOffset, 1, CrgRange (0x0000,0x0000) );//10
	DEF_CRG1( TTL, 1, CrgRange (3,255) );//10
	//CrgOneOf (CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Protocol.r, 4, CrgRange (0x0, 0x5),CrgRange (0x7,0x10),CrgRange (0x12,0x83),CrgRange (0x85,0xff)); //exculinf TCP,UDP,SCTP
	Crg_Range_Con (__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &Protocol.get_crg_var(), 4, CrgRange (0x0, 0x0),CrgRange (0x0,0x0),CrgRange (0x0,0x0),CrgRange (0x0,0x0));
	DEF_CRG1( Checksum, 1, CrgRange (0,0xffff) );//10
	DEF_CRG1( SourceIP, 1, CrgRange (0x0,0xffffffff) );//10
	DEF_CRG1( DestIP, 1, CrgRange (0x0,0xffffffff) );//10
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
								  CrgRange (800,800),\
								  CrgRange (800,800));

}

void   PPPoEHeaderClass:: set_constraints() {
	//Jephy CrgWeightCase (CrgSetBase, Crg_NOT_BY_ARR, Crg_NOT_SORT, Version.r, 2, CrgCreateWeight (CrgSingleton (0x2), 0.2), CrgCreateWeight (CrgSingleton (0x1), 9.8) );
	DEF_CRG1( Version, 1, CrgRange (0x1,0x2) );//Y
	DEF_CRG1( Type, 1, CrgRange (0x1,0x1) );//Y
	DEF_CRG1( Code, 1, CrgRange (0x0,0x0) );//Y
	DEF_CRG1( SessionID, 1, CrgRange (0x1,0xfffe) );//10
	DEF_CRG1( Length, 1, CrgRange (0,0xffff) );//10
}

void   SCTPHeaderClass:: set_constraints() {
	DEF_CRG1( PortSrc, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( PortDst, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( VerficationTag, 1, CrgRange (0x0,0xffffffff) );//10
	//DEF_CRG1( Checksum, 1, CrgRange (0x0,0xffffffff) );//10
}

void   DCCPHeaderClass:: set_constraints() {
	DEF_CRG1( PortSrc, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( PortDst, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( DataOffset, 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( CCVal, 1, CrgRange (0x0,0xf) );//10
	DEF_CRG1( CsCov, 1, CrgRange (0x0,0xf) );//10
	//DEF_CRG1( Checksum, 1, CrgRange (0x0,0xffff) );//10
	//DEF_CRG1( Reserved, 1, CrgRange (0x0,0x0) );//10
	DEF_CRG1( Type, 1, CrgRange (0x0,0xf) );//10
	DEF_CRG1( XBit, 1, CrgRange (0x1,0x1) );//10
	DEF_CRG1( SequenceNumber, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( SequenceNumberCont, 1, CrgRange (0x0,0xffffffff) );//10
}

void   UDPHeaderClass:: set_constraints() {
	DEF_CRG1( SourcePort, 1, CrgRange (	0x3333,	0x3333) );//10
	DEF_CRG1( DestPort, 1, CrgRange (0x4444,0x4444) );//10
	DEF_CRG1( Length, 1, CrgRange (0xd,0xd) );//10
	DEF_CRG1( Checksum, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( ZeroChecksum, 1, CrgRange (0,0) );
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
	DEF_CRG1( SourceAddress[0], 1, CrgRange (0xa1a1a1a1,0xa1a1a1a1) );//10
	DEF_CRG1( SourceAddress[1], 1, CrgRange (0xa2a2a2a2,0xa2a2a2a2) );//10
	DEF_CRG1( SourceAddress[2], 1, CrgRange (0xa3a3a3a3,0xa3a3a3a3) );//10
	DEF_CRG1( SourceAddress[3], 1, CrgRange (0xa4a4a4a4,0xa4a4a4a4) );//10
	DEF_CRG1( DestinationAddress[0], 1, CrgRange (0xaaaaaaaa,0xaaaaaaaa) );//10
	DEF_CRG1( DestinationAddress[1], 1, CrgRange (0xbbbbbbbb,0xbbbbbbbb) );//10
	DEF_CRG1( DestinationAddress[2], 1, CrgRange (0xcccccccc,0xcccccccc) );//10
	DEF_CRG1( DestinationAddress[3], 1, CrgRange (0xdddddddd,0xdddddddd) );//10
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
	DEF_CRG1( PayloadData, 1, CrgRange (0xaa,0xaa));//10
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

void   GTPv2HeaderClass:: set_constraints() {
	//DEF_CRG1( version, 1, CrgRange (0x2,0x2) );//10
	DEF_CRG1( Pbit, 1, CrgRange (0x0,0x1) );//10
	DEF_CRG1( Tbit, 1, CrgRange (0x1,0x1) );//10
	DEF_CRG1( Spare1, 1, CrgRange (0x0,0x7) );//10
	DEF_CRG1( MessageType, 1, CrgRange (0x0,0xff) );//10
	DEF_CRG1( MessageLength, 1, CrgRange (0x0,0xffff) );//10
	DEF_CRG1( TEID, 1, CrgRange (0x0,0xffffffff) );//10
	DEF_CRG1( SequenceNumber, 1, CrgRange (0x0,0xffffff) );//10
	DEF_CRG1( Spare2, 1, CrgRange (0x0,0xff) );//10
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




