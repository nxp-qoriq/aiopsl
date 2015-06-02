#ifndef __FSL_RCU_H
#define __FSL_RCU_H

/**************************************************************************//**
@Function	rcu_synchronize

@Description	Activate the callback after all the active AIOP tasks
		(except those that called rcu_read_unlock) are done

@Param[in]	cb	Callback to be called after all the active readers
			are done
@Param[in]	param	Callback parameter

@Return		0 on succees, POSIX error code otherwise  \ref error_g

@Cautions	Set APP_RCU_COMMITTED, APP_RCU_MAX, APP_RCU_TIMER_DURATION at
		apps.h before using the RCU module
*//***************************************************************************/
int rcu_synchronize(void (*cb)(uint64_t), uint64_t param);

/**************************************************************************//**
@Function	rcu_read_unlock

@Description	Remove task from readers list

@Cautions	This function should only be called if the calling task
		does not need any resources(e.g. allocated buffers, DP objects).
		Once this function is called, there is no guarantee that these
		resources will remain valid.

		Every AIOP task that is created automatically holds an
		RCU read lock.

		Must call rcu_read_unlock_cancel() after rcu_read_unlock().
		Otherwise the next task with the same task id will
		be considered as a task that does not hold any resource.
*//***************************************************************************/
void rcu_read_unlock();


/**************************************************************************//**
@Function	rcu_read_unlock_cancel

@Description	Add the task back to the readers list;
		It reverts the rcu_read_unlock().

@Caution	Every AIOP task that is created automatically holds an
		RCU read lock.

		Must call rcu_read_unlock_cancel() after rcu_read_unlock().
		Otherwise the next task with the same task id will
		be considered as a task that does not hold any resource.
*//***************************************************************************/
void rcu_read_unlock_cancel();


#endif /* __FSL_RCU_H */
