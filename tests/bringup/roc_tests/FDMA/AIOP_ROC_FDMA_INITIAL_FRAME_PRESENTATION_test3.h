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
@File		AIOP_ROC_FDMA_INITIAL_FRAME_PRESENTATION_test3.h

@Description	This file contains FDMA DOA test.

*//***************************************************************************/

/**********************************************************************************************/
/* Frame type: 																				  */
/* 1:IPv4/IPv4 UDP or TCP or GRE wi/wo Vtag		2:IPv6/IPv6 UDP or TCP or GRE wi/wo Vtag      */
/* IPv6 will have all combination of Extension as fragmented and unfragmented part			  */
/* MTU=[256,1500] Payload=[800,3000 ]													  */
/**********************************************************************************************/

#ifndef __AIOP_ROC_FDMA_INITIAL_FRAME_PRESENTATION_H
#define __AIOP_ROC_FDMA_INITIAL_FRAME_PRESENTATION_H

#include "main.h"
#include "common.h"
#include "FDMA_Param_generator.h"
#include "aiop_common_parameters.h"
#include "frame_generators.h"
#include "frame_headers.h"

#define CONSTRAINT_NAME "AIOP_ROC_FDMA_INITIAL_FRAME_PRESENTATION.h"

static char *__CRG_FILE_NAME = (char *)"AIOP_ROC_FDMA_INITIAL_FRAME_PRESENTATION.h";

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
	DEF_CRG1( BufferSize, 1, CrgRange(1024,1024));//if OIM is 128, at least 448 (448,4096)
	CrgWeightCase (__CRG_FILE_NAME, __LINE__, CrgSetBase, Crg_NOT_BY_ARR, Crg_NOT_SORT, &DF_Test.get_crg_var(), 2, CrgCreateWeight(__CRG_FILE_NAME, __LINE__,0,0,7,1), CrgCreateWeight(__CRG_FILE_NAME, __LINE__,1,1,3,1));	
	DEF_CRG1( DF_Test, 1, CrgRange(0,0));//only enable for IPv4 test
	DEF_CRG1( DF_Discard, 1, CrgRange(0,0));
	Crg_Range_Con (__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, Crg_NOT_SORT, Crg_NO_DET_VARS, &IPF_FOF.get_crg_var(), 2, CrgRange (96, 96),CrgRange (112,112));//,CrgRange (128,128)); if FOF=128+32+maxheader(38+60)=258 > 256
	DEF_CRG1( Var_BPID_For_SGE, 1, CrgRange(0,1));
	DEF_CRG1( FD_Offset, 1, CrgRange(256,256));
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
	
#if FDMA_SINGLE_ERROR_TYPE
	DEF_CRG1( FDMA_ErrorType, 1, CrgRange(FDMA_SUCCESS, FDMA_SUCCESS));   
#else
	Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
								  Crg_NOT_SORT, Crg_NO_DET_VARS, &FDMA_ErrorType.get_crg_var(), 5,\
								  CrgRange (FDMA_SUCCESS,FDMA_SUCCESS),\
								  CrgRange (FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR,FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR),\
								  CrgRange (FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR,FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR),\
								  CrgRange (FDMA_FD_ERR,FDMA_FD_ERR),\
								  CrgRange (FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR,FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR));
#endif
	
}



/*2. FDMA cmd fetch hw annotation */
void FDMACmdReadASA_CMDClass :: set_constraints()
{
	DEF_CRG1(offset,1, CrgRange(0, 0)); /* 64B unit */
	DEF_CRG1(present_size, 1, CrgRange(4, 4)); /* 64B uint */
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_READ_ASA));
    DEF_CRG1(massive_test, 1, CrgRange(0, 0)); /* 64B uint */

}

/*3. FDMA cmd fetch sw annotation*/
void FDMACmdReadPTA_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_READ_PTA));
    DEF_CRG1(massive_test, 1, CrgRange(0, 0)); /* 64B uint */
}

/*4. FDMA write hw annotation.*/
void FDMACmdReplaceASA_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_REPLACE_ASA));
    DEF_CRG1(to_offset, 1, CrgRange(0, 0));
    DEF_CRG1(to_size, 1, CrgRange(2, 2));

    DEF_CRG1(from_size, 1, CrgRange(1, 1));
	DEF_CRG1(massive_test, 1, CrgRange(1, 1));
    DEF_CRG1(size_rs, 1, CrgRange(4, 4)); 	/* 64B * size_rs */
    DEF_CRG1(SA, 1, CrgRange(0, 0));

    /* generate source data for this replacement */
    for (i = 0; i < 128; i++) {
        DEF_CRG1(replace_content[i], 1, CrgRange(0, 0xFF));
    }
}

/*5. FDMA cmd write sw annotation.*/
void FDMACmdReplacePTA_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_REPLACE_PTA));
    DEF_CRG1(SA, 1, CrgRange(0, 1));
    DEF_CRG1(size, 1, CrgRange(1, 3)); /* 1: lower 32B, 2:upper 32B, 3: all 64B */
	DEF_CRG1(massive_test, 1, CrgRange(1, 1));
    for (i = 0; i < 64; i++) {
        DEF_CRG1(replace_content[i], 1, CrgRange(0, 0xFF));
    }
}

/* FDMA cmd checksum working frame */
void FDMACmdChecksumWorkingFrame_CMDClass :: set_constraints() {
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CHECKSUM_WORKING_FRAME));
    DEF_CRG1(offset, 1, CrgRange(0, 10));
    DEF_CRG1(size, 1, CrgRange(50, 100));
    DEF_CRG1(frame_handle, 1, CrgRange(0, 0));
};


/*6. FDMA cmd init frame presentation*/
void FDMACmdInitFramePresentation_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_INITIAL_FRAME_PRESENTATION));
    DEF_CRG1(seg_offset, 1, CrgRange(100, 100));
    DEF_CRG1(present_size, 1, CrgRange(100, 100));
    DEF_CRG1(asa_offset,1, CrgRange(0, 0));
    DEF_CRG1(asa_size,1, CrgRange(1, 1)); 	/* 256B ASA area */
    DEF_CRG1(SR, 1, CrgRange(0, 0));
    DEF_CRG1(NDS, 1, CrgRange(0, 0));
}

/* FDMA cmd init frame presentation*/
void FDMACmdInitFramePresentationExplicit_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_INITIAL_FRAME_PRESENTATION_EXPLICIT));
    DEF_CRG1(seg_offset, 1, CrgRange(100, 110));
    DEF_CRG1(present_size, 1, CrgRange(0, 100));
    DEF_CRG1(asa_offset, 1, CrgRange(0, 0));
    DEF_CRG1(asa_size, 1, CrgRange(4, 4));
    DEF_CRG1(NDS, 1, CrgRange(0, 1));
    DEF_CRG1(SR, 1, CrgRange(0, 1));
    
}

/*6. FDMA cmd init frame presentation no segment */
void FDMACmdInitNoSeg_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_INIT_FRAME_NO_SEG));
}

/* FDMA cmd init frame presentation no segment exp */
void FDMACmdInitNoSegExp_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_INIT_FRAME_NO_SEG_EXP));

    DEF_CRG1(NDS, 1, CrgRange(0, 1));
    DEF_CRG1(AS, 1, CrgRange(0, 1));
    DEF_CRG1(VA, 1, CrgRange(0, 1));
    DEF_CRG1(PL, 1, CrgRange(0, 1));
    
}

/*7. FDMA cmd presentation default segment*/
void FDMACmdPresentDefaultSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_PRESENT_SEGMENT));
}

/*7. FDMA cmd presentation segment*/
void FDMACmdPresentSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_PRESENT_SEGMENT));
    DEF_CRG1(size, 1, CrgRange(10, 40));
    DEF_CRG1(SR, 1, CrgRange(1, 1));
    DEF_CRG1(offset, 1, CrgRange(300, 400));
}

/*7. FDMA cmd presentation frame segment*/
void FDMACmdPresentFrameSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_PRESENT_FRAME_SEGMENT));
    DEF_CRG1(size, 1, CrgRange(10, 40));
    DEF_CRG1(SR, 1, CrgRange(1, 1));
    DEF_CRG1(offset, 1, CrgRange(300, 400));
    DEF_CRG1(frame_handle, 1, CrgRange(0, 0));
}


/*8. FDMA cmd externd presentation*/
void FDMACmdExtendPresentation_CMDClass :: set_constraints()
{
	uint8_t test_type=randomInt(1,1);
	if(test_type==1) //Extend Data seg
	{
	    DEF_CRG1(extend_size, 1, CrgRange(0, 100));
	    DEF_CRG1(ST, 1, CrgRange(0, 0));
	}
	else//extend ASA
	{
		DEF_CRG1(extend_size, 1, CrgRange(0, 4));
	    DEF_CRG1(ST, 1, CrgRange(1, 1));
	}
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_EXTEND_PRESENTATION));

}

/* ----------- NEW ------------------*/
/*9. FDMA cmd store default frame data*/
void FDMACmdStoreDefaultFrameData_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_STORE_DEFAULT_FRAME_DATA));
    DEF_CRG1(spid, 1, CrgRange(0, 0xFF));

}


/* ----------- NEW ------------------*/
/*9. FDMA cmd store frame data*/
void FDMACmdStoreFrameData_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_STORE_FRAME_DATA));
    DEF_CRG1(frame_handle, 1, CrgRange(0, 0));
    DEF_CRG1(spid, 1, CrgRange(0, 0xFF));

}


/* ----------- NEW ------------------*/
/*10. FDMA cmd enqueue default working frame*/
void FDMACmdEnqueueDefaultWorkingFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_DEFAULT_WORKING_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(0, 0));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0));
    DEF_CRG1(PS, 1, CrgRange(0, 1));
    DEF_CRG1(TC, 1, CrgRange(0, 1));//(0.2)
    DEF_CRG1(EIS, 1, CrgRange(0, 1));
    DEF_CRG1(spid, 1, CrgRange(0, 0xff));
}



/* ----------- NEW ------------------*/
/*10. FDMA cmd enqueue frame*/
void FDMACmdEnqueueWorkingFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_WORKING_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(0, 0));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0));
    DEF_CRG1(PS, 1, CrgRange(0, 1));
    DEF_CRG1(TC, 1, CrgRange(0, 1));//(0,2)
    DEF_CRG1(EIS, 1, CrgRange(0, 1));
    DEF_CRG1(spid, 1, CrgRange(0, 0xff));
    DEF_CRG1(handle, 1, CrgRange(0, 0));
}


/*10. FDMA cmd enqueue default frame*/
void FDMACmdEnqueueDefaultFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_DEFAULT_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(0, 0));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0));
    DEF_CRG1(PS, 1, CrgRange(0, 1));
    DEF_CRG1(TC, 1, CrgRange(0, 1));//(0,3)
    DEF_CRG1(EIS, 1, CrgRange(0, 1));
    DEF_CRG1(icid, 1, CrgRange(0, 0xff));
    DEF_CRG1(VA, 1, CrgRange(0, 1));
	DEF_CRG1(PL, 1, CrgRange(0, 1));
	DEF_CRG1(BDI, 1, CrgRange(0, 1));
}



/*10. FDMA cmd enqueue frame*/
void FDMACmdEnqueueFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ENQUEUE_FRAME));
    DEF_CRG1(qd_fqid, 1, CrgRange(0, 0));
    DEF_CRG1(hash_value, 1, CrgRange(0, 0));
    DEF_CRG1(qd_priority, 1, CrgRange(0, 0));
    DEF_CRG1(PS, 1, CrgRange(0, 1));
    DEF_CRG1(TC, 1, CrgRange(0, 1));//(0,3)
    DEF_CRG1(EIS, 1, CrgRange(0, 1));
    DEF_CRG1(icid, 1, CrgRange(0, 0xff));
    DEF_CRG1(VA, 1, CrgRange(0, 1));
	DEF_CRG1(PL, 1, CrgRange(0, 1));
	DEF_CRG1(BDI, 1, CrgRange(0, 1));
}

/* FDMA cmd discard frame*/
void FDMACmdDiscardFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_DISCARD_WORKING_FRAME));
    DEF_CRG1(TC, 1, CrgRange(0, 0));//(0,1)
}

/* FDMA cmd discard default frame*/
void FDMACmdDiscardDefaultFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_DISCARD_DEFAULT_WORKING_FRAME));
    DEF_CRG1(TC, 1, CrgRange(0, 0));
}

/* FDMA cmd discard fd*/
void FDMACmdDiscardFD_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_DISCARD_FD));
//    DEF_CRG1(TC, 1, CrgRange(0, 0));
}

/* FDMA cmd force discard default frame*/
void FDMACmdForceDiscardFD_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_FORCE_DISCARD_FD));
//   DEF_CRG1(TC, 1, CrgRange(0, 0));
}

/* FDMA cmd terminate*/
void FDMACmdTerminate_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TERMINATE_TASK));
}


/*16. FDMA cmd replicate frame*/
void FDMACmdReplicateFrame_CMDClass :: set_constraints()
{
	DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_REPLICATE_FRAME));
	DEF_CRG1(hash_value, 1, CrgRange(0, 0xffff));
	DEF_CRG1(spid, 1, CrgRange(0, 0xff));
	DEF_CRG1(qd_fqid, 1, CrgRange(0, 0xFF));
	DEF_CRG1(qd_priority, 1, CrgRange(0, 0xFF));
	DEF_CRG1(PS, 1, CrgRange(1, 1));
	DEF_CRG1(EIS, 1, CrgRange(0, 1));		//FQID
//	DEF_CRG1(ENQ, 1, CrgRange(0, 0));		//not enqueue
//	DEF_CRG1(DSF, 1, CrgRange(0, 1));
	DEF_CRG1(CFA, 1, CrgRange(0, 3));
}

/*17. FDMA cmd concatenate frame*/
void FDMACmdConcatenateFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CONCATENATE_FRAME));
    DEF_CRG1(frame1, 1, CrgRange(0, 0));
    DEF_CRG1(frame2, 1, CrgRange(1, 1));
    DEF_CRG1(spid, 1, CrgRange(0, 0)); /* not support */
    DEF_CRG1(trim, 1, CrgRange(0, 0xff));
    DEF_CRG1(SF, 1, CrgRange(0, 1));
    DEF_CRG1(PCA, 1, CrgRange(1, 1));
}

/*18. FDMA cmd split frame*/
void FDMACmdSplitFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_SPLIT_FRAME));
    DEF_CRG1(seg_offset, 1, CrgRange(0, 0x64));
    DEF_CRG1(present_size, 1, CrgRange(0, 0x64));
    DEF_CRG1(split_size_sf, 1, CrgRange(0, 0x64));
    DEF_CRG1(source_frame_handle, 1, CrgRange(0, 0)); /* fix 0, for testing */
    DEF_CRG1(spid, 1, CrgRange(0, 0)); /* not support */
    DEF_CRG1(CFA, 1, CrgRange(0, 3));
    DEF_CRG1(PSA, 1, CrgRange(1, 1));
    DEF_CRG1(SM, 1, CrgRange(0, 0));
    DEF_CRG1(SR, 1, CrgRange(0, 1));
}

/*19. FDMA cmd trim segment*/
void FDMACmdTrimSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_TRIM_SEGMENT));
    DEF_CRG1(offset, 1, CrgRange(0, 50));
    DEF_CRG1(size, 1, CrgRange(10, 50));
}

/* ----------- NEW ------------------*/
/*20. FDMA cmd modify segment*/
void FDMACmdModifySegment_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_MODIFY_SEGMENT));
    DEF_CRG1(offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(size, 1, CrgRange(24, 40));

    for (i = 0; i < 24; i++) {
        DEF_CRG1(data[i], 1, CrgRange(0, 0xFF));
    }
};
/* ----------- NEW ------------------*/
/*20. FDMA cmd replace segment*/
void FDMACmdReplaceSegment_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_REPLACE_SEGMENT));
    DEF_CRG1(to_offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(to_size, 1, CrgRange(0, 50));
    DEF_CRG1(from_size, 1, CrgRange(0, 128));
    DEF_CRG1(SA, 1, CrgRange(0, 1)); 			/* 0: keep open, 1: represent segment, 2: colse */

    for (i = 0; i < 3; i++) {
        DEF_CRG1(reserved[i], 1, CrgSingleton(0));
    }


    for (i = 0; i < 255; i++) {
        DEF_CRG1(replace_content[i], 1, CrgRange(0, 0xFF));
    }

    DEF_CRG1(size_rs, 1, CrgRange(1, 45)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
    DEF_CRG1(offset_rs, 1, CrgRange(300, 350)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
};


/*21. FDMA cmd insert data*/
void FDMACmdInsertData_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_INSERT_DATA));
    DEF_CRG1(SA, 1, CrgRange(0, 1));
    DEF_CRG1(offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(size, 1, CrgRange(0, 20));

    for (i = 0; i < 255; i++) {
        DEF_CRG1(insert_content[i], 1, CrgRange(0, 0xFF));
    }

    DEF_CRG1(size_rs, 1, CrgRange(1, 45)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
    DEF_CRG1(offset_rs, 1, CrgRange(300, 350)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */

}


/*21. FDMA cmd insert data exp */
void FDMACmdInsertDataExplicit_CMDClass :: set_constraints()
{
    uint8_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_INSERT_DATA));
    DEF_CRG1(SA, 1, CrgRange(0, 1));
    DEF_CRG1(offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(size, 1, CrgRange(0, 20));
    DEF_CRG1(frame_handle, 1, CrgRange(0, 0));
    DEF_CRG1(seg_handle, 1, CrgRange(0, 0));

    for (i = 0; i < 255; i++) {
        DEF_CRG1(insert_content[i], 1, CrgRange(0, 0xFF));
    }

    DEF_CRG1(size_rs, 1, CrgRange(1, 45)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
    DEF_CRG1(offset_rs, 1, CrgRange(300, 350)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */

}


/*22. FDMA CMD delete data*/
void FDMACmdDeleteDefaultData_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_DELETE_DEFAULT_DATA));
    DEF_CRG1(SA, 1, CrgRange(0, 2));
    DEF_CRG1(offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(delete_target_size, 1, CrgRange(0, 50));

    DEF_CRG1(size_rs, 1, CrgRange(1, 45)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
    DEF_CRG1(offset_rs, 1, CrgRange(300, 350)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
}

/*22. FDMA CMD delete data*/
void FDMACmdDeleteData_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_DELETE_DATA));
    DEF_CRG1(SA, 1, CrgRange(0, 2));
    DEF_CRG1(offset, 1, CrgRange(0, 0x10)); /* DEBUG: offset should lower than destination size */
    DEF_CRG1(delete_target_size, 1, CrgRange(0, 50));
    DEF_CRG1(size_rs, 1, CrgRange(1, 45)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
    DEF_CRG1(offset_rs, 1, CrgRange(300, 350)); /* 8 byte(segment presentation area 32*8 + MAX_SEGMENT_DELTA) */
	DEF_CRG1(frame_handle, 1, CrgRange(0, 0));
	DEF_CRG1(seg_handle, 1, CrgRange(0, 0));
}


/* close segment */
void FDMACmdCloseDefaultSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CLOSE_DEFAULT_SEGMENT));
}

/* close segment */
void FDMACmdCloseSegment_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CLOSE_SEGMENT));
	DEF_CRG1(frame_handle, 1, CrgRange(0, 0));
	DEF_CRG1(seg_handle, 1, CrgRange(0, 0));
}


/* create frame */
void FDMACmdCreateFrame_CMDClass :: set_constraints()
{
	uint16_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CREATE_FRAME));
	DEF_CRG1(size, 1, CrgRange(256, 512));
    for (i = 0; i < 1024; i++) {
        DEF_CRG1(insert_content[i], 1, CrgRange(0, 0xFF));
    }
}

/* create fd */
void FDMACmdCreateFD_CMDClass :: set_constraints()
{
	uint16_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CREATE_FD));
	DEF_CRG1(size, 1, CrgRange(256, 512));
    for (i = 0; i < 1024; i++) {
        DEF_CRG1(insert_content[i], 1, CrgRange(0, 0xFF));
    }
}

/* create frame arp request broadcast */
void FDMACmdCreateFrameArpRequestBroadcast_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CREATE_FRAME_ARP_REQUEST_BROADCAST));
	DEF_CRG1(local_ip, 1, CrgRange(0x00000000, 0xffffffff));
	DEF_CRG1(target_ip, 1, CrgRange(0x00000000, 0xffffffff));
	DEF_CRG1(dframe, 1, CrgRange(0, 1));
}

/* create frame arp request */
void FDMACmdCreateFrameArpRequest_CMDClass :: set_constraints()
{
	uint16_t i;

    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_CREATE_FRAME_ARP_REQUEST));
	DEF_CRG1(local_ip, 1, CrgRange(0x00000000, 0xffffffff));
	DEF_CRG1(target_ip, 1, CrgRange(0x00000000, 0xffffffff));
	DEF_CRG1(dframe, 1, CrgRange(0, 1));
    for (i = 0; i < 6; i++) {
        DEF_CRG1(target_eth[i], 1, CrgRange(0, 0xFF));
    }
}

/* copy frame */
void FDMACmdCopyFrame_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_COPY_FRAME));
	DEF_CRG1(copy_size, 1, CrgRange(0, 0xff));
	DEF_CRG1(SM, 1, CrgRange(0, 1));
	DEF_CRG1(DM, 1, CrgRange(0, 1));
}

/* dma data */
void FDMACmdDMAData_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_DMA_DATA));
	DEF_CRG1(copy_size, 1, CrgRange(0, 0xff));
	DEF_CRG1(icid, 1, CrgRange(0, 0));
	DEF_CRG1(DA, 1, CrgRange(0, 3));
	DEF_CRG1(VA, 1, CrgRange(0, 1));
	DEF_CRG1(BMT, 1, CrgRange(0, 1));
	DEF_CRG1(PL, 1, CrgRange(0, 1));		
}

/* acquire buffer */
void FDMACmdAcquireBuffer_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_ACQUIRE_BUFFER));
	DEF_CRG1(icid, 1, CrgRange(0, 0xff));
	DEF_CRG1(bpid, 1, CrgRange(0, 0xff));
	DEF_CRG1(bdi, 1, CrgRange(0, 1));
}

/* release buffer */
void FDMACmdReleaseBuffer_CMDClass :: set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_RELEASE_BUFFER));
	DEF_CRG1(icid, 1, CrgRange(0, 0xff));
	DEF_CRG1(bpid, 1, CrgRange(0, 0xff));
	DEF_CRG1(bdi, 1, CrgRange(0, 1));
}


void FDMACmdListClass::set_constraints()
{
     DEF_CRG1(maxNumOfCmd, 1, CrgRange(3, 3));
}

void FDMATerminate_CMDClass::set_constraints()
{
    DEF_CRG1(cmd, 1, CrgSingleton(e_AIOP_FDMA_OPERATION_LAST));
}


#if 0 /* full command support */
void FDMACmdClass::set_constraints(){
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &CMD.get_crg_var(), 2,\
					  CrgRange (e_AIOP_INITIAL_FRAME_PRESENTATION,e_AIOP_EXTEND_PRESENTATION),\
					  CrgRange (e_AIOP_ENQUEUE_FRAME,e_AIOP_CLOSE_SEGMENT));
}
#else /* single command */
void FDMACmdClass::set_constraints(){ /* randomSeed = 712563249 */
    Crg_Range_Con(__CRG_FILE_NAME, __LINE__, CrgSetBase, 0,Crg_NON_WEAK_CON, Crg_REGULAR_CON, Crg_NOT_TEMP, Crg_NOT_BY_ARR, \
					  Crg_NOT_SORT, Crg_NO_DET_VARS, &CMD.get_crg_var(), 2,\
					  CrgRange (e_AIOP_INITIAL_FRAME_PRESENTATION, e_AIOP_INITIAL_FRAME_PRESENTATION),\
					  CrgRange (e_AIOP_INITIAL_FRAME_PRESENTATION, e_AIOP_INITIAL_FRAME_PRESENTATION));
}
#endif



void   FrameInputClass:: set_constraints() 
{
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
	}
	else
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
	//DEF_CRG1( TunnelHeaderEn, 1, CrgRange (0,0) );//10
	//DEF_CRG1( TunnelHeaderType, 1, CrgRange (e_IPV4,e_IPV6) );//10
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
	//DEF_CRG1( NextHeader, 1, CrgRange (0,0) );
	//DEF_CRG1( PayloadLength, 1, CrgRange (0,0) );
	//DEF_CRG1( Reserved, 1, CrgRange (0,0) );
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
	//DEF_CRG1( TunnelHeaderEn, 1, CrgRange (0,0) );//10
	//DEF_CRG1( TunnelHeaderType, 1, CrgRange (e_IPV4,e_IPV6) );//10
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

