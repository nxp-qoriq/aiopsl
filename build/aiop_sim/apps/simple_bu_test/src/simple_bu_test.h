
int parser_init(uint8_t *prpid);
int simple_bu_gal_test(void);
int simple_bu_ilan_test(void);
int simple_bu_yariv_test(void);
void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags);
int create_default_frame_wa(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t *frame_handle);
void test_fdma_copy_data();
