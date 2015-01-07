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

#include "fsl_table.h"
#include "fsl_keygen.h"

#define EIGHT_BYTE_USER_DATA_1 0xDEADC0DEDEADC0DE
#define EIGHT_BYTE_USER_DATA_2 0xDEADC0DEDEADC0DE
#define EIGHT_BYTE_USER_DATA_3 0xDEADC0DEDEADC0DE
#define EIGHT_BYTE_USER_DATA_4 0xDEADC0DEDEADC0DE

#define ONE_BYTE_USER_DATA_1 0xDE
#define ONE_BYTE_USER_DATA_2 0xDE

#define SOME_CONSTANT 0x00
#define TABLE_EXAMPLE_EM_KEY_SIZE 124

int table_exact_match_example();
int table_longest_prefix_match_example();

int table_exact_match_example()
{
	uint16_t                    table_id;
	uint8_t                     keysize = TABLE_EXAMPLE_EM_KEY_SIZE;
	struct table_create_params  table_params;
	struct table_rule           rule1;
	union table_lookup_key_desc lkup_key_desc;
	uint8_t                     em_key[TABLE_EXAMPLE_EM_KEY_SIZE];
	struct table_lookup_result  lookup_res;
	uint8_t                     keyid;
	uint8_t                     *kcr;
	struct kcr_builder          kc_builder;

	/* KCR building, Please refer to Keygen section*/
	keygen_kcr_builder_init(&kc_builder);
	if (keygen_kcr_builder_add_constant_fec(SOME_CONSTANT,
						TABLE_EXAMPLE_EM_KEY_SIZE,
						&kc_builder)) {
		/* Error Handling */
		return -1;
	}
	kcr = kc_builder.kcr;

	/* KCR creation, refer to Keygen section */
	if (keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU, kcr, &keyid)) {
		/*Error handling */
	}

	/* Table Parameter Initialization 
	 * External (located in DDR) Exact Match Table, with a miss result, a
	 * guaranteed number of 1000 rules and a maximum number of 2000 rules.
	 * */
	table_params.attributes =	TABLE_ATTRIBUTE_TYPE_EM |
					TABLE_ATTRIBUTE_LOCATION_EXT1 |
					TABLE_ATTRIBUTE_MR_MISS;
	table_params.committed_rules = 1000;
	table_params.max_rules = 2000;
	table_params.key_size = keysize;

	/* Table Miss Result initialization - Type Opaque */
	table_params.miss_result.type = TABLE_RESULT_TYPE_OPAQUES;
	table_params.miss_result.op0_rptr_clp.opaque0 = EIGHT_BYTE_USER_DATA_1;
	table_params.miss_result.opaque1 = EIGHT_BYTE_USER_DATA_2;
	table_params.miss_result.opaque2 = ONE_BYTE_USER_DATA_1;

	/* Table Creation */
	if(table_create(TABLE_ACCEL_ID_CTLU, &table_params,&table_id)) {
		/* Error handling */
		return -1;
	}

	/* Initialize a new table rule - Type Opaque */
	/* Rule's key generation. Please refer to Keygen section.*/
	if(keygen_gen_key(KEYGEN_ACCEL_ID_CTLU,
			  keyid,
			  0,
			  &(rule1.key_desc.em.key),
			  &keysize)) {
		/* Error handling */
		return -1;
	}

	/* Rule Result Initialization */
	rule1.result.type = TABLE_RESULT_TYPE_OPAQUES;
	rule1.result.op0_rptr_clp.opaque0 = EIGHT_BYTE_USER_DATA_3;
	rule1.result.opaque1 = EIGHT_BYTE_USER_DATA_4;
	rule1.result.opaque2 = ONE_BYTE_USER_DATA_2;
	rule1.options = TABLE_RULE_TIMESTAMP_NONE;

	/* Add the rule to the table */
	if(table_rule_create(TABLE_ACCEL_ID_CTLU, table_id, &rule1, keysize)) {
		/* Error handling */
		return -1;
	}
	/* Initialize a Lookup Key Descriptor*/
	if(keygen_gen_key(KEYGEN_ACCEL_ID_CTLU,
			  keyid,
			  0,
			  em_key,
			  &keysize)) {
		/* Error handling */
		return -1;
	}
	lkup_key_desc.em_key = em_key;

	/* Perform a lookup */
	if(table_lookup_by_key(TABLE_ACCEL_ID_CTLU,
			       table_id,
			       lkup_key_desc,
			       keysize,
			       &lookup_res)) {
		/* Error handling */
		return -1;
	}

	/* It is expected that after the lookup: 
	 * lookup_res.opaque0_or_reference == rule1.result.op0_rptr_clp.opaque0
	 * lookup_res.opaque1 == rule1.result.opaque1
	 * lookup_res.opaque2 == rule1.result.opaque2
	 * */

	return 0;
}

int table_longest_prefix_match_example()
{
	uint16_t                   table_id;
	struct table_create_params table_params;
	struct table_rule          rule1;
	uint8_t                    keyid;
	struct table_lookup_result lookup_res;
	uint8_t                    *kcr;
	struct kcr_builder         kc_builder;

	/* KCR building, Please refer to Keygen section*/
	keygen_kcr_builder_init(&kc_builder);
	if (/* Fills the LPM exact match field with zeros */
	    keygen_kcr_builder_add_constant_fec(0x00,
						4,
						&kc_builder)
	    /* Extract the first IP header source address */
	    || keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_IPSRC_1_FECID,
			NULL,
			&kc_builder)
	    /* Fills the maximum prefix so a lookup on all prefixes will be
	     * performed */
	    || keygen_kcr_builder_add_constant_fec(0xFF,
				1,
				&kc_builder)) {

		/* Error Handling */
		return -1;
	}
	kcr = kc_builder.kcr;

	/* KCR creation, refer to Keygen section */
	if (keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU, kcr, &keyid)) {
		/*Error handling */
	}

	/* Table Parameter Initialization 
	 * PEB (Packet Express Buffer Memory) LPM Table, without a miss result,
	 * with a guaranteed number of 1000 rules and a maximum number of 2000
	 * rules.
	 * */
	table_params.attributes = TABLE_ATTRIBUTE_TYPE_LPM |
				  TABLE_ATTRIBUTE_LOCATION_PEB |
				  TABLE_ATTRIBUTE_MR_NO_MISS;
	table_params.committed_rules = 1000;
	table_params.max_rules = 2000;
	table_params.key_size = TABLE_KEY_LPM_IPV6_SIZE;

	/* Table Creation */
	if(table_create(TABLE_ACCEL_ID_CTLU, &table_params,&table_id)) {
		/* Error handling */
		return -1;
	}

	/* Initialize a new table rule with 2620:0000:0861:0001::/64 */
	rule1.key_desc.lpm_ipv6.exact_match = 0x0;
	rule1.key_desc.lpm_ipv6.prefix_length = 64;
	rule1.key_desc.lpm_ipv6.addr0 = 0x2620000008610001;
	rule1.key_desc.lpm_ipv6.addr1 = 0x0000000000000000;

	rule1.result.type = TABLE_RESULT_TYPE_OPAQUES;
	/* REMOVE COMMENT AND CHANGE COLOR */
	rule1.result.op0_rptr_clp.opaque0 = EIGHT_BYTE_USER_DATA_1;
	rule1.result.opaque1 = EIGHT_BYTE_USER_DATA_2;
	rule1.result.opaque2 = ONE_BYTE_USER_DATA_1;
	rule1.options = TABLE_RULE_TIMESTAMP_ENABLE;

	/* Add a rule to the table */
	if(table_rule_create(TABLE_ACCEL_ID_CTLU,
			     table_id,
			     &rule1,
			     TABLE_KEY_LPM_IPV6_SIZE)) {
		/* Error handling */
		return -1;
	}

	/* Check if packet is IPv6 */
	if (!PARSER_IS_OUTER_IPV6_DEFAULT()) {
		/* Error handling */
		return -1;
	}

	/* Perform a lookup */
	if(table_lookup_by_keyid(TABLE_ACCEL_ID_CTLU,
				 table_id,
				 keyid,
				 TABLE_LOOKUP_FLAG_NONE,
				 NULL,
				 &lookup_res)) {
		/* Error handling */
		return -1;
	}

	/* It is expected that after the lookup, if the frame suits
	 * 2620:0000:0861:0001::/64, the following will be true: 
	 * lookup_res.opaque0_or_reference == rule1.result.op0_rptr_clp.opaque0
	 * lookup_res.opaque1 == rule1.result.opaque1
	 * lookup_res.opaque2 == rule1.result.opaque2
	 * lookup_res.timestamp is filled with the timestamp
	 * */

	return 0;
}
