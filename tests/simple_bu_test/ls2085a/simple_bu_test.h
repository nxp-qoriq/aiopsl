
int parser_init(uint8_t *prpid);
int simple_bu_lpm_test(void);
int simple_bu_hagit_test(void);
int simple_bu_parser_test(uint8_t generator_id,
							uint8_t event_id,
							uint64_t app_ctx,
							void *event_data);
int simple_bu_gal_test(void);
int simple_bu_ilan_test(void);
int simple_bu_yariv_test(void);
int simple_bu_ohad_test(void);
int simple_bu_ipf_ipr_test(void);
int simple_bu_test_ipsec(void);
void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags);
void test_fdma_copy_data();
