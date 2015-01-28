/*
 * RTEMS multi-tasking support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/rpc.h>
#include <rtems.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

/*
 * RPC variables for single-thread
 */
static struct _rtems_rpc_task_variables rpc_default = {
	-1,		/* svc_maxfd */
	{{0}},		/* svc_svc_fdset */
	NULL,		/* svc_xports */
	0,		/* svc_xportssize */
	0,		/* svc__svc_fdsetsize */		
	0,		/* svc__svc_fdset */
	NULL,		/* svc_svc_head */
	0,		/* clnt_perror_buf */
	0,		/* clnt_raw_private */
	0,		/* call_rpc_private */
	0,		/* svc_raw_private */

	0,		/* svc_simple_proglst */
	0,		/* svc_simple_pl */
	0,		/* svc_simple_transp */

	0,		/* rpcdname_default_domain */
	0		/* svc_auths_Auths */
};

/*
 * RPC values for initializing a new per-task set of variables
 */
static const struct _rtems_rpc_task_variables rpc_init = {
	-1,		/* svc_maxfd */
	{{0}},		/* svc_svc_fdset */
	NULL,		/* svc_xports */
	0,		/* svc_xportssize */
	0,		/* svc__svc_fdsetsize */		
	0,		/* svc__svc_fdset */
	NULL,		/* svc_svc_head */
	0,		/* clnt_perror_buf */
	0,		/* clnt_raw_private */
	0,		/* call_rpc_private */
	0,		/* svc_raw_private */

	0,		/* svc_simple_proglst */
	0,		/* svc_simple_pl */
	0,		/* svc_simple_transp */

	0,		/* rpcdname_default_domain */
	0		/* svc_auths_Auths */
};

/*
 * Per-task pointer to RPC data
 */
static pthread_once_t rtems_rpc_task_variable_once = PTHREAD_ONCE_INIT;
static pthread_key_t rtems_rpc_task_variable_key;

/*
 * Return the current task variable pointer.
 */
struct _rtems_rpc_task_variables *rtems_rpc_task_variables_get (void)
{
	void *ptr = pthread_getspecific(rtems_rpc_task_variable_key);
	if (ptr == NULL) {
		ptr = &rpc_default;
	}
	return (struct _rtems_rpc_task_variables *) ptr;
}

/*
 * Key create function for task_variable_key.
 */
static void rtems_rpc_task_variable_make_key (void)
{
	int eno = pthread_key_create(&rtems_rpc_task_variable_key, NULL);
	assert (eno == 0);
	/*
	 * FIXME: Should have destructor which cleans up
	 * all RPC stuff:
	 *  - Close all files
	 *  - Go through and free linked list elements
	 *  - Free other allocated memory (e.g. clnt_perror_buf)
	 */
}

/*
 * Set up per-task RPC variables
 */
int rtems_rpc_task_init (void)
{
	struct _rtems_rpc_task_variables *tvp;
	int eno = 0;

	eno = pthread_once(
		&rtems_rpc_task_variable_once,
		rtems_rpc_task_variable_make_key
	);
	assert (eno == 0);

	tvp = pthread_getspecific (rtems_rpc_task_variable_key);
	if (tvp == NULL) {
		tvp = malloc (sizeof *tvp);
		if (tvp == NULL)
			return RTEMS_NO_MEMORY;

		eno = pthread_setspecific (rtems_rpc_task_variable_key, (void *) tvp);
		if (eno != 0) {
			free (tvp);
			return RTEMS_INTERNAL_ERROR;
		}
		*tvp = rpc_init;
	}
	return RTEMS_SUCCESSFUL;
}
