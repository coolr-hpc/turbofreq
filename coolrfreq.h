#ifndef __COOLRFREQ_H_DEFINED__
#define __COOLRFREQ_H_DEFINED__

/* from turbofreq.c */
extern int  core_get_pstate_local(void);
extern int  core_get_min_pstate_local(void);
extern int  core_get_max_pstate_local(void);
extern int  core_get_turbo_pstate_local(void);
extern void core_set_pstate_local(int pstate);
extern int  core_get_scaling_local(void);

/* from pstate_user.c */
int  init_pstate_user_dev(void);
void fini_pstate_user_dev(void);

#endif
