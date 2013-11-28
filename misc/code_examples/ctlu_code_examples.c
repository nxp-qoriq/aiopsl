#include "fsl_ctlu.h"

#define eight_byte_user_data 0xDEADC0DEDEADC0DE

int ctlu_exact_match_example();
int ctlu_longest_prefix_match_example();

int ctlu_exact_match_example()
{
	uint16_t table_id;
	struct ctlu_table_create_params table_params;
	struct ctlu_table_rule rule1;
	uint8_t  keysize;
	uint8_t  keyid;
	struct ctlu_lookup_result lookup_res;

	/* Key Profile creation, refer to Key Composition Rule section */
	/* create_key_profile(..., &keyid, &keysize); TODO REMOVE COMMENT AND
	 * CHANGE COLOR! */

	/* Table Parameter Initialization 
	 * External (located in DDR) Exact Match Table, with a miss result, a
	 * guaranteed number of 1000 rules and a maximum number of 2000 rules.
	 * */
	table_params.attributes =	CTLU_TBL_ATTRIBUTE_TYPE_EM |
					CTLU_TABLE_ATTRIBUTE_LOCATION_EXT |
					CTLU_TBL_ATTRIBUTE_MR_MISS;
	table_params.committed_rules = 1000;
	table_params.max_rules = 2000;
	table_params.key_size = keysize;

	/* Table Miss Result initialization - Type Opaque */
	table_params.miss_result.type = CTLU_RULE_RESULT_TYPE_OPAQUES;
	/* TODO REMOVE COMMENT AND CHANGE COLOR */ 
	table_params.miss_result.op_rptr_clp.opaque0 = /*<8-byte user data >*/ 0;
	table_params.miss_result.opaque1 = /*<8-byte user data >*/ 0;
	table_params.miss_result.opaque2 = /*<1-byte user data >*/ 0;

	/* Table Creation */
	if(ctlu_table_create(&table_params,&table_id)) {
		/* Error handling */
		return -1;
	}

	/* Initialize a new table rule - Type Opaque */
	/* Rule's key generation */
	if(ctlu_gen_key(keyid, &rule1.key, &keysize)) {
		/* Error handling */
		return -1;
	}
	rule1.result.type = CTLU_RULE_RESULT_TYPE_OPAQUES;
	/* TODO REMOVE COMMENT AND CHANGE COLOR */
	rule1.result.op_rptr_clp.opaque0 = /*<8-byte user data >*/ 0;
	rule1.result.opaque1 = /*<8-byte user data >*/ 0;
	rule1.result.opaque2 = /*<1-byte user data >*/ 0;
	rule1.options = CTLU_RULE_TIMESTAMP_NONE;

	/* Add the rule to the table */
	if(ctlu_table_rule_create(table_id, &rule1, keysize)) {
		/* Error handling */
		return -1;
	}

	/* Perform a lookup */
	if(ctlu_table_lookup_by_key(table_id, &rule1.key, keysize,
				    &lookup_res)) {
		/* Error handling */
		return -1;
	}

	/* It is expected that after the lookup: 
	 * lookup_res.opaque0_or_reference == rule1.result.op_rptr_clp.opaque0
	 * lookup_res.opaque1 == rule1.result.opaque1
	 * lookup_res.opaque2 == rule1.result.opaque2
	 * */

	return 0;
}

int ctlu_longest_prefix_match_example()
{
	uint16_t table_id;
	struct ctlu_table_create_params table_params;
	struct ctlu_table_rule rule1;
	uint8_t  keysize;
	uint8_t  keyid;
	struct ctlu_lookup_result lookup_res;

	/* Key Profile creation, refer to Key Composition Rule section */
	/* create_key_profile(..., &keyid, &keysize); REMOVE COMMENT*/

	/* Table Parameter Initialization */
	table_params.attributes =	CTLU_TBL_ATTRIBUTE_TYPE_LPM |
					CTLU_TABLE_ATTRIBUTE_LOCATION_EXT |
					CTLU_TBL_ATTRIBUTE_MR_NO_MISS;
	table_params.committed_rules = 10000;
	table_params.max_rules = 10000;
	table_params.key_size = CTLU_KEY_LPM_IPV6_SIZE;

	/* Table Creation */
	if(ctlu_table_create(&table_params,&table_id)) {
		/* Error handling */
		return -1;
	}

	/* Initialize a new table rule with 2620:0000:0861:0001::/64 */
	rule1.key.key_lpm_ipv6.prefix_length = 64;
	rule1.key.key_lpm_ipv6.addr0 = 0x2620000008610001;
	rule1.key.key_lpm_ipv6.addr1 = 0x0000000000000000;
	rule1.key.key_lpm_ipv6.exact_match = 0x0;

	rule1.result.type = CTLU_RULE_RESULT_TYPE_OPAQUES;
	/* REMOVE COMMENT AND CHANGE COLOR */
	rule1.result.op_rptr_clp.opaque0 = /*<8-byte user data >*/ 0;
	rule1.result.opaque1 = /*<8-byte user data >*/ 0;
	rule1.result.opaque2 = /*<1-byte user data >*/ 0;
	rule1.options = CTLU_RULE_TIMESTAMP_NONE;

	/* Add a rule to the table */
	if(ctlu_table_rule_create(table_id, &rule1, keysize)) {
		/* Error handling */
		return -1;
	}

	/* Perform a lookup */
	if(ctlu_table_lookup_by_keyid(table_id, keyid, &lookup_res)) {
		/* Error handling */
		return -1;
	}

	/* It is expected that after the lookup: 
	 * lookup_res.opaque0_or_reference == rule1.result.op_rptr_clp.opaque0
	 * lookup_res.opaque1 == rule1.result.opaque1
	 * lookup_res.opaque2 == rule1.result.opaque2
	 * */

	return 0;
}
