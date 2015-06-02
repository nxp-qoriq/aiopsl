static void rcu_read_unlock()
{
	SPIN_LOCK(SW_CTSTWS);
	/* 1 - need to wait for task
	 * 0 - no need to wait for task
	 * must be atomic write */
	temp = SW_CTSTWS & ~(1 << task_id);
	SW_CTSTWS = temp;
	SPIN_UNLOCK(SW_CTSTWS);
}

static void rcu_read_unlock_cancel()
{
	SPIN_LOCK(SW_CTSTWS);
	/* 1 - need to wait for task
	 * 0 - no need to wait for task
	 * must be atomic write */
	temp = SW_CTSTWS | (1 << task_id);
	SW_CTSTWS = temp;
	SPIN_UNLOCK(SW_CTSTWS);
}

/************** Implementation based on the TMAN **************
 * Note init_one_shot_timer() can be easily replaced by cmdif_send() */

void rcu_init()
{
	/* rcu_slab is used for rcu_list, it is a static global variable
	 * max and commited are going to be user parameters at compile time
	 * TODO must have slab early init */
	rcu_slab = slab_create(DDR, committed, max);

	/* rcu_list is limited by max of rcu_slab,
	 * it is a static global variable */
	rcu_list = init_list();

	/* one shot timer duration is going to be user parameter
	 * at compile time
	 * TODO No need to reserve TMI for RCU sync, it maybe user parameter */
	tman_create_tmi();

	/* One SW register per one HW register */
	SW_CTSTWS = 0xffffffff;
}

int rcu_synchronize(void (*cb)(uint64_t), uint64_t param)
{
	/* rcu_list is spinlock protected */
	size = enqueue(cb, param);

	if (size == 1)
		err = init_one_shot_timer(tman_cb, -1);

	return err;
}

static void tman_cb(uint64_t ignore, uint16_t batch_size)
{
	rcu_read_unlock();

	if (batch_size == -1) {
		/* rcu_list is spinlock protected */
		batch_size = rcu_list_size();

		/* next prime will be only after this one has finished */
		CTSTWS = 0;
	}

	if ((CTSTWS & SW_CTSTWS) == 0) {

		for (i = 0; i < batch_size; i++) {

			/* rcu_list is spinlock protected */
			(cb, param) = dequeue();
			cb(param);
		}

		if (!empty(rcu_list))
			init_one_shot_timer(tman_cb, -1);
	} else {
		init_one_shot_timer(tman_cb, batch_size);
	}

	rcu_read_unlock_cancel();
	fdma_terminate_task();
}
