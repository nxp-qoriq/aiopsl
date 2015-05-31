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
@defgroup AIOPSL   AIOP Service Layer API 
@details The primary audience for this material is AIOP application software developers looking for reference examples that describe in detail the low level functions of the AIOP service layer.
@{
*//***************************************************************************/
/*Begin group AIOP General */	
	/** @defgroup AIOP_GENERAL AIOP General
	@ingroup AIOPSL
	@{
*/
/** @defgroup AIOP_General_Definitions AIOP SL General Definitions
	@ingroup AIOP_GENERAL
	@{
*/
		/** @defgroup ldpaa_g  LDPAA API
			@ingroup AIOP_General_Definitions
			@{
	    */
			/** @defgroup LDPAA_FD_DEFINITIONS LDPAA FD Definitions
				@ingroup ldpaa_g
			*/
			/** @defgroup LDPAA_FD_GETTERS_SETTERS LDPAA FD GETTER/SETTER MACROs
				@ingroup ldpaa_g
			*/
		/** @} */
		/** @defgroup AIOP_Return_Status  AIOP Return Status
			@ingroup AIOP_General_Definitions
		*/
		/** @defgroup AIOP_HWC_Definitions AIOP HWC Definitions
			@ingroup AIOP_General_Definitions
			@{
		*/	
			/** @defgroup AIOP_HWC_General_Definitions  AIOP HWC General Definitions
				@ingroup AIOP_HWC_Definitions
			*/		
			/** @defgroup AIOP_ADC_Definitions  AIOP ADC Definitions
				@ingroup AIOP_HWC_Definitions
				@{
			*/
			/** @defgroup AIOP_ADC_Getters  AIOP ADC Getters
				@ingroup AIOP_ADC_Definitions
			*/		
			/** @} */
			/** @defgroup AIOP_PRC_Definitions  AIOP PRC Definitions
				@ingroup AIOP_HWC_Definitions
				@{
			*/
			/** @defgroup AIOP_PRC_Getters  AIOP PRC Getters
				@ingroup AIOP_PRC_Definitions
			*/		
			/** @} */
		/** @} */
		/** @defgroup AIOP_DEFAULT_TASK_Params  AIOP Default Task Params
			@ingroup AIOP_General_Definitions   
		*/

    /** @} */
/** @} */	
/*End group AIOP General */	
/*Begin group ACCEL */		
	/** @defgroup ACCEL Accelerators APIs
	 	@ingroup AIOPSL	
		@{ */
/*Begin group FDMA */
			/** @defgroup FSL_AIOP_FDMA FDMA
				@ingroup ACCEL
				@{	*/
					/** @defgroup FDMA_General_Definitions FDMA General Definitions
						@ingroup FSL_AIOP_FDMA
					*/			 
					/** @defgroup FDMA_Enumerations FDMA Enumerations
						@ingroup FSL_AIOP_FDMA
					*/	
					/** @defgroup FDMA_Commands_Flags FDMA Commands Flags
						@ingroup FSL_AIOP_FDMA
						@{
					*/
						/** @defgroup FDMA_Present_Frame_Flags  FDMA Present Frame Flags
							@ingroup FDMA_Commands_Flags
						*/	
						/** @defgroup FDMA_PRES_Flags  FDMA PRES Flags
							@ingroup FDMA_Commands_Flags
						*/
						/** @defgroup FDMA_EXT_Flags  FDMA EXT Flags
							@ingroup FDMA_Commands_Flags
						*/	
						/** @defgroup FDMA_ENWF_Flags  FDMA ENWF Flags
							@ingroup FDMA_Commands_Flags
						*/				
						/** @defgroup FDMA_ENF_Flags  FDMA ENF Flags
							@ingroup FDMA_Commands_Flags
						*/	
						/** @defgroup FDMA_Discard_WF_Flags  FDMA Discard WF Flags
							@ingroup FDMA_Commands_Flags
						*/
						/** @defgroup FDMA_Replicate_Flags  FDMA Replicate Flags
							@ingroup FDMA_Commands_Flags
						*/	
						/** @defgroup FDMA_Concatenate_Flags  FDMA Concatenate Flags
							@ingroup FDMA_Commands_Flags
						*/					
						/** @defgroup FDMA_Split_Flags  FDMA Split Flags
							@ingroup FDMA_Commands_Flags
						*/	
						/** @defgroup FDMA_Replace_Flags  FDMA Replace Flags
							@ingroup FDMA_Commands_Flags
						*/
						/** @defgroup FDMA_Copy_Flags  FDMA Copy Flags
							@ingroup FDMA_Commands_Flags
						*/
						/** @defgroup FDMA_ACQUIRE_BUFFER_Flags  FDMA ACQUIRE BUFFER Flags
							@ingroup FDMA_Commands_Flags
						*/
						/** @defgroup FDMA_RELEASE_BUFFER_Flags  FDMA RELEASE BUFFER Flags
							@ingroup FDMA_Commands_Flags
						*/						
						/** @defgroup FDMA_ISOLATION_ATTRIBUTES_Flags  FDMA ISOLATION ATTRIBUTES Flags
							@ingroup FDMA_Commands_Flags
						*/	
					/**@} */			
					/** @defgroup FDMA_STATUS FDMA Status
						@ingroup FSL_AIOP_FDMA
					*/							
					/** @defgroup FDMA_Structures FDMA Structures
						@ingroup FSL_AIOP_FDMA
					*/
					/** @defgroup FDMA_Functions FDMA Functions
						@ingroup FSL_AIOP_FDMA
					*/			
			/**@} */
/*End group FDMA */
/*Begin group CDMA */			
			/** @defgroup FSL_CDMA CDMA
				@ingroup ACCEL			
			@{	*/
				/** @defgroup CDMA_Commands_Flags CDMA Commands Flags
					@ingroup FSL_CDMA
					@{
				*/
					/** @defgroup CDMA_DMA_MUTEX_ModeBits  CDMA DMA MUTEX ModeBits
						@ingroup CDMA_Commands_Flags
					*/	
					/** @defgroup CDMA_MUTEX_ModeBits  CDMA MUTEX ModeBits
						@ingroup CDMA_Commands_Flags
					*/
				/**@} */			
				/** @defgroup CDMA_Functions CDMA Functions
					@ingroup FSL_CDMA
				*/			
			/**@} */			
/*End group CDMA */
/*Begin group FSL_PARSER */				
			
	/** @defgroup FSL_PARSER  PARSER
	 	@ingroup ACCEL
			@{ */
			/** @defgroup FSL_PARSER_MACROS PARSER Macros
					@ingroup FSL_PARSER
					@{ */
					/** @defgroup FSL_PARSER_FRAME_ATTRIBUTES_EXTENSION_MASKS Frame Attributes Extension Masks
							@ingroup FSL_PARSER_MACROS
					 */					
					/** @defgroup FSL_PARSER_FRAME_ATTRIBUTES_PART_1_MASKS Frame Attributes Masks 1
							@ingroup FSL_PARSER_MACROS
					 */
					/** @defgroup FSL_PARSER_FRAME_ATT_ERROR_PART_1_MASKS Frame Attributes Error Masks 1
							@ingroup FSL_PARSER_MACROS
					 */
					/** @defgroup FSL_PARSER_FRAME_ATTRIBUTES_PART_2_MASKS Frame Attributes Masks 2
							@ingroup FSL_PARSER_MACROS
					 */
					/** @defgroup FSL_PARSER_FRAME_ATT_ERROR_PART_2_MASKS Frame Attributes Error Masks 2
							@ingroup FSL_PARSER_MACROS
					 */
					/** @defgroup FSL_PARSER_FRAME_ATTRIBUTES_PART_3_MASKS Frame Attributes Masks 3
							@ingroup FSL_PARSER_MACROS
					 */
					/** @defgroup FSL_PARSER_FRAME_ATT_ERROR_PART_3_MASKS Frame Attributes Error Masks 3
							@ingroup FSL_PARSER_MACROS
					 */	
					/** @defgroup FSL_PARSER_ERROR_CODES PARSER Error Codes
							@ingroup FSL_PARSER_MACROS
					 */	
					/** @defgroup FSL_PARSER_ERROR_QUERIES Parse Result Error Queries
							@ingroup FSL_PARSER_MACROS
					 */	
					/** @defgroup FSL_PARSER_PR_QUERIES Parse Result Attributes Queries
							@ingroup FSL_PARSER_MACROS
					 */	
					/** @defgroup FSL_PARSER_PR_GETTERS Parse Result Getters
							@ingroup FSL_PARSER_MACROS
					 */	
					/** @defgroup FSL_PARSER_POINTER_IN_FRMAE_GETTERS Pointer in Frame Getters
							@ingroup FSL_PARSER_MACROS
					 */	
					/** @defgroup FSL_PARSER_SETTERS PARSER Setters
							@ingroup FSL_PARSER_MACROS
					 */						 
					/** @defgroup FSL_PARSER_GEN_PARSE_RESULT_FLAGS Flags for parse_result_generate function
							@ingroup FSL_PARSER_MACROS
					 */					 
					/** @defgroup FSL_PARSER_HXS_CONFIG PARSER HXS configuration in parse profile defines
							@ingroup FSL_PARSER_MACROS
					 */	
			/**@} */
			/** @defgroup PARSER_Enumerations PARSER Enumerations
					@ingroup FSL_PARSER
			 */	
			/** @defgroup FSL_PARSER_STRUCTS PARSER Structures
				@ingroup FSL_PARSER
			 */	
			/** @defgroup FSL_PARSER_Functions PARSER Functions
					@ingroup FSL_PARSER
			 */		
			/** @} */
/*End group FSL_PARSER */
/*Begin group FSL_KEYGEN */	


			/** @defgroup FSL_KEYGEN KEYGEN
				@ingroup ACCEL
				@{ */
				/** @defgroup FSL_KEYGEN_MACROS KEYGEN Macros
					@ingroup FSL_KEYGEN
					@{ */
					/** @defgroup FSL_KEYGEN_KCR_ATTRIBUTES Key Composition Rule Attributes
						@ingroup FSL_KEYGEN_MACROS
					*/
				/** @} */
				/** @defgroup FSL_KEYGEN_Enumerations KEYGEN Enumerations
					@ingroup FSL_KEYGEN
				*/
				/** @defgroup FSL_KEYGEN_STRUCTS KEYGEN Structures
					@ingroup FSL_KEYGEN
				*/
				/** @defgroup FSL_KEYGEN_Functions KEYGEN Functions
					@ingroup FSL_KEYGEN
				*/				
			/** @} */	
/*End group FSL_KEYGEN */
/*Begin group FSL_TABLE */				
			
			/** @defgroup FSL_TABLE TABLE
				@ingroup ACCEL
				@{ */
					/** @defgroup FSL_TABLE_MACROS  TABLE Macros
						@ingroup FSL_TABLE
						@{
					*/
						/** @defgroup FSL_TABLE_ATTRIBUTES  TABLE Attributes
							@ingroup FSL_TABLE_MACROS
							@{
						*/
							/** @defgroup FSL_TABLE_ATTRIBUTE_TYPE TABLE Type Attribute
								@ingroup FSL_TABLE_ATTRIBUTES
							*/
							/** @defgroup FSL_TABLE_ATTRIBUTE_LOCATION TABLE Location Attribute
								@ingroup FSL_TABLE_ATTRIBUTES
							*/
							/** @defgroup FSL_TABLE_ATTRIBUTE_MR TABLE Miss Result Attribute
								@ingroup FSL_TABLE_ATTRIBUTES
							*/
						/** @} */
						/** @defgroup FSL_TABLE_RESULT_TYPES TABLE Results Types
							@ingroup FSL_TABLE_MACROS
						*/
						/** @defgroup FSL_TABLE_RULE_OPTIONS TABLE Rule Options
							@ingroup FSL_TABLE_MACROS
							@{
						*/						
							/** @defgroup FSL_TABLE_RULE_OPTIONS_TIMESTAMP TABLE Rule Timestamp Options
								@ingroup FSL_TABLE_RULE_OPTIONS
							*/							
						/** @} */
						/** @defgroup FSL_TABLE_KEY_DEFINES TABLE Rule Key
							@ingroup FSL_TABLE_MACROS
						*/
						/** @defgroup FSL_TABLE_LOOKUP_FLAG_DEFINES TABLE Lookup Flags
							@ingroup FSL_TABLE_MACROS
						*/
						/** @defgroup FSL_TABLE_STATUS Status returned to calling function
							@ingroup FSL_TABLE_MACROS
						*/						
					/** @} */						
					/** @defgroup FSL_TABLE_Enumerations TABLE Enumerations
						@ingroup FSL_TABLE
					*/							
					/** @defgroup FSL_TABLE_STRUCTS TABLE Structures
						@ingroup FSL_TABLE
					*/							
					/** @defgroup FSL_TABLE_Functions TABLE Functions
						@ingroup FSL_TABLE
					*/						
				/** @} */		
/*End group FSL_TABLE  */
/*Begin group FSL_AIOP_OSM */	


			/** @defgroup FSL_AIOP_OSM OSM
				@ingroup ACCEL
				@{	*/
				/** @defgroup OSM_Structures OSM Structures
					@ingroup FSL_AIOP_OSM
				*/			 
				/** @defgroup OSM_Commands_Flags OSM Commands Flags
					@ingroup FSL_AIOP_OSM
					@{
				*/
					/** @defgroup OSMScopeEnterModeBits Scope enter mode bits
						@ingroup OSM_Commands_Flags
					*/			
				/**@} */
				/** @defgroup OSM_Functions OSM Functions
					@ingroup FSL_AIOP_OSM
				*/	
			/**@} */
/*End group FSL_AIOP_OSM  */
/*Begin group TMAN */	

			/** @defgroup TMAN TMAN
				@ingroup ACCEL
				@{	*/
					/** @defgroup TMANMacroes TMAN MACROS
						@ingroup TMAN
					*/						
					/** @defgroup TMANDataStructures TMAN Data Structures
						@ingroup TMAN
					*/	
					/** @defgroup TMAN_Flags TMAN Flags
						@ingroup TMAN
						@{
					*/
						/** @defgroup TMANInsDeleteModeBits TMAN instance delete flags
							@ingroup TMAN_Flags
						*/	
						/** @defgroup TMANTimerDeleteModeBits TMAN timer delete flags
							@ingroup TMAN_Flags
						*/	
						/** @defgroup TMANTimerCreateModeBits TMAN timer create flags
							@ingroup TMAN_Flags
						*/	
					/**@} */
					/** @defgroup TMAN_Functions TMAN functions
						@ingroup TMAN
					*/
			/**@} */
/*End group TMAN  */
/*Begin group STE */		
			
			/** @defgroup STATS  STE (Statistics)
				@ingroup ACCEL
				@{	*/
					/** @defgroup StatsFlags Statistics flags
						@ingroup STATS
					*/			 
					/** @defgroup StatsCompoundFlags Statistics Compound commands flags
						@ingroup STATS
					*/
					/** @defgroup StatsErrorRegAddr Statistics error registers addresses
						@ingroup STATS
					*/		
					/** @defgroup StatsErrorStatusReg Statistics status register bits definitions
						@ingroup STATS
					*/	
					/** @defgroup StatsErrorCtrlBits Statistics control register bits definitions
						@ingroup STATS
					*/	
					/** @defgroup StatsErrCap1 Statistics capture attributes definitions
						@ingroup STATS
					*/	
					/** @defgroup StatsErrMacroes Statistics error macros
						@ingroup STATS
					*/						
					/** @defgroup STE_functions Statistics functions
						@ingroup STATS
					*/
			/**@} */
/*End group STE  */

		/** @} */
/*End group ACCEL  */
			/** @defgroup grp_dplib_aiop DPLIB
				@details Management complex firmware provides datapath objects for initialization, configuration and management of DPAA2 hardware resources. DPLIB functions enable AIOP developers to access these MC firmware capabilities.
				@ingroup AIOPSL
				@{
			*/
				/** @defgroup dpci_drv_g DPCI DRV
					@ingroup grp_dplib_aiop
				*/
					/** @defgroup DPCI_DRV_INIT_PR_Flags DPCI Entry Point flags
						@ingroup dpci_drv_g
					 */

				/** @defgroup dpni_drv_g DPNI DRV
					@ingroup grp_dplib_aiop
				*/
					/** @defgroup DPNI_DRV_LINK_OPT Link Options
					@ingroup dpni_drv_g
					*/
					/** @defgroup DPNI_DRV_BUF_LAYOUT_OPT Buffer Layout modification options
					@ingroup dpni_drv_g
					*/

			/** @} */
	
	
	/** 
		@defgroup NETF NETF (Network Libraries)
		@details NETF provides various frame handling functions including IP reassembly, IP fragmentation and various header modification functions.
		@ingroup AIOPSL
		@{
      */
	  
		/** @defgroup AIOP_NETF_GENERAL GENERAL
			*ingroup NETF
			@{
		*/

			/** @defgroup FSL_AIOP_FRAME_OPERATIONS AIOP Frame Operations
				@ingroup AIOP_NETF_GENERAL
				@{
			*/
			
				/** @defgroup FRAME_OPERSTIONS_Functions Frame Operations Functions
					@ingroup FSL_AIOP_FRAME_OPERATIONS
				*/
			/** @} */
		/** @} */		
		
		/** @defgroup AIOP_IP IP
			@ingroup NETF
			@{
		*/
		
			/** @defgroup AIOP_IP_HM IP Header Modification
				@ingroup AIOP_IP
				@{
			*/		

				/** @defgroup HM_IP_Modes HM IP Modes
					@ingroup AIOP_IP_HM
					@{
				*/
					/** @defgroup HMIPv4ModeBits IPv4 header modification mode bits
						@ingroup HM_IP_Modes
					*/
					/** @defgroup HMIPv4MangleBits IPv4 header Mangle bits
						@ingroup HM_IP_Modes
					*/	
					/** @defgroup HMIPv6MangleBits IPv6 header Mangle bits
						@ingroup HM_IP_Modes
					*/						
					/** @defgroup HMIPv6ModeBits IPv6 header modification mode bits
						@ingroup HM_IP_Modes
					*/
					/** @defgroup HMIPv4EncapModeBits IPv4 header encapsulation mode bits
						@ingroup HM_IP_Modes
					*/
					/** @defgroup HMIPv6EncapModeBits IPv6 header encapsulation mode bits
						@ingroup HM_IP_Modes
					*/
					/** @defgroup HMIPDecapModeBits IP header decapsulation mode bits
						@ingroup HM_IP_Modes
					*/	
					/** @defgroup HMIPCksumCalcModeBits IP header checksum calculation mode bits
						@ingroup HM_IP_Modes
					*/	
					/** @defgroup IPv4TimestampOptions IPv4 time-stamp options
						@ingroup HM_IP_Modes
					*/	
					
				/** @} */
					/** @defgroup FSL_HM_IP_Functions HM IP related functions
						@ingroup AIOP_IP_HM
					*/					
			/** @} */		
		/** @} */	

		/** @defgroup AIOP_L2 L2
			@ingroup NETF
			@{
		*/
		
			/** @defgroup AIOP_L2_HM L2 Header Modification
				@ingroup AIOP_L2
				@{
			*/			
					/** @defgroup FSL_HM_L2_Functions HM L2 related functions
						@ingroup AIOP_L2_HM
					*/	
			/** @} */		
		/** @} */	



		/** @defgroup AIOP_L4 L4
			@ingroup NETF
			@{
		*/
		
			/** @defgroup AIOP_L4_HM L4 Header Modification
				@ingroup AIOP_L4
				@{
			*/					
					/** @defgroup HM_L4_Modes HM L4 Modes
						@ingroup AIOP_L4_HM
						@{
					*/
						/** @defgroup HMUDPModeBits UDP header modification mode bits
							@ingroup HM_L4_Modes
						*/
						/** @defgroup HMTCPModeBits TCP header modification mode bits
							@ingroup HM_L4_Modes
						*/
						/** @defgroup HML4UDPTCPCksumCalcModeBits L4 UDP TCP Checksum Calculation mode bits
							@ingroup HM_L4_Modes
						*/
					/** @} */					
					/** @defgroup FSL_HM_L4_Functions HM L4 related functions
						@ingroup AIOP_L4_HM
					*/						
			/** @} */
			
			/** @defgroup FSL_AIOP_GSO GSO
				@ingroup AIOP_L4
				@{
			*/					
				/** @defgroup GSO_MACROS GSO Macros
					@ingroup FSL_AIOP_GSO
					@{
				*/
					/** @defgroup TCP_GSO_GENERAL_DEFINITIONS TCP GSO General Definitions
						@ingroup GSO_MACROS
					*/
					/** @defgroup TCP_GSO_FLAGS TCP GSO Flags
						@ingroup GSO_MACROS
					*/
					/** @defgroup TCP_GSO_GENERATE_SEG_STATUS  TCP GSO Statuses
						@ingroup GSO_MACROS
					*/
				/** @} */					
				/** @defgroup GSO_Functions GSO Functions
					@ingroup FSL_AIOP_GSO
				*/
			/** @} */	

			
		/** @} */	

		
		
		/** @defgroup AIOP_NAT NAT
			@ingroup NETF
			@{
		*/
		
			/** @defgroup AIOP_NAT_HM NAT Header Modification
				@ingroup AIOP_NAT
				@{
			*/						
					/** @defgroup HM_NAT_Modes HM NAT Modes
						@ingroup AIOP_NAT_HM
						@{
					*/
						/** @defgroup HMNATModeBits NAT header modification mode bits
							@ingroup HM_NAT_Modes
						*/
					/** @} */										
					/** @defgroup FSL_HM_NAT_Functions HM NAT related functions
						@ingroup AIOP_NAT_HM
					*/						
			/** @} */		
		/** @} */
		
		/** @defgroup FSL_IPF IPF
			@ingroup NETF
			@{
		*/
			/** @defgroup IPF_MACROS IP Fragmentation Macros
				@ingroup FSL_IPF
				@{
			*/
				/** @defgroup IPF_GENERAL_DEFINITIONS IPF General Definitions
					@ingroup IPF_MACROS
				*/
				/** @defgroup IPF_FLAGS IPF Flags
					@ingroup IPF_MACROS
				*/
				/** @defgroup IPF_GENERATE_FRAG_STATUS IPF Return Status
					@ingroup IPF_MACROS
				*/		
			/** @} */	
			/** @defgroup IPF_Functions IPF Functions
				@ingroup FSL_IPF
			*/
		/** @} */	


		/** @defgroup FSL_IPR IPR
			@ingroup NETF
			@{
		*/
			/** @defgroup IPR_MACROS IPR Macros
				@ingroup FSL_IPR
			*/
			/** @defgroup IPR_STRUCTS IPR Data Structures
				@ingroup FSL_IPR
			*/			
			/** @defgroup FSL_IPR_Modes_And_Flags IPR Modes And Flags
				@ingroup FSL_IPR
				@{
			*/
				/** @defgroup FSL_IPRInsFlags IPR instance flags
					@ingroup FSL_IPR_Modes_And_Flags
				*/
				/** @defgroup FSL_IPRStatsFlags IPR stats flags
					@ingroup FSL_IPR_Modes_And_Flags
				*/
				/** @defgroup FSL_IPRUpdateFlags IPR Update flags
					@ingroup FSL_IPR_Modes_And_Flags
				*/	

				/** @defgroup FSL_IPRCreateReturnStatus IPR create instance return status
					@ingroup FSL_IPR_Modes_And_Flags
				*/
				/** @defgroup FSL_IPRReassReturnStatus IPR functions return status
					@ingroup FSL_IPR_Modes_And_Flags
				*/
				/** @defgroup FSL_IPRTOCallbackFlags IPR Time Out Callback flags
					@ingroup FSL_IPR_Modes_And_Flags
				*/					
			/** @} */	
			/** @defgroup FSL_IPR_Functions IPR functions
				@ingroup FSL_IPR
			*/
		/** @} */	






/** @} */		 


/** @defgroup group_util   Utilities
	@ingroup AIOPSL
	@{
*/
	/** @defgroup net_utils_g AIOP Service Layer Network Utilities
		@ingroup group_util
	*/
	/** @defgroup FSL_DEBUG_GROUP Debug Utilities
		@ingroup group_util
	*/
	/** @defgroup accessor_g Accessor API
		@ingroup group_util
	*/	
	/** @defgroup error_g Error Handling
		@ingroup group_util
	*/	
	/** @defgroup time_g Time Queries
		@ingroup group_util
	*/
	/** @defgroup fsl_os_g  FSL OS Interface (System call hooks)
		@ingroup group_util
	*/	
/** @} */


/** @defgroup kernel_g Kernel
	@ingroup AIOPSL
	@{
*/
	/** @defgroup mem_mng_g_id Memory Management
		@ingroup kernel_g
		@{
	*/
		/** @defgroup malloc_g_id Dynamic Memory Allocation
			@ingroup mem_mng_g_id
		*/	
		/** @defgroup slab_g SLAB
			@ingroup mem_mng_g_id
		*/
		/** @defgroup shbp_aiop_g Shared Buffer Pool
			@ingroup mem_mng_g_id
		*/		
	/** @} */
	/** @defgroup Synchronization_g_id Synchronization
		@ingroup kernel_g
		@{
	*/
		/** @defgroup Spinlock_Functions Spin-lock functions
			@ingroup Synchronization_g_id
		*/	
		/** @defgroup atomic_operations Atomic operations
			@ingroup Synchronization_g_id
		*/		
	/** @} */
	
	/** @defgroup cmdif_g  Command Interface
	@ingroup kernel_g
	@{ */
		/** @defgroup cmdif_client_g  Command Interface - Client
		 	@ingroup cmdif_g
			@{	
		*/	
			/** @defgroup CMDIF_SEND_ATTRIBUTES Send Attributes
			*/
		/** @} */
	/** @defgroup cmdif_server_g  Command Interface - Server
		 		@ingroup cmdif_g
		*/	
	/** @} */	
	
	/** @defgroup ic_g Isolation Context
	@ingroup kernel_g
	@{ @} */
	
	
	
/** @} */

/**@} */ /*close group AIOPSL   AIOP Service Layer API*/
	




