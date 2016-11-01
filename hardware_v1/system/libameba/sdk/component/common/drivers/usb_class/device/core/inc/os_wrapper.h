#ifndef _OS_WRAPPER_H_
#define _OS_WRAPPER_H_

#include "osdep_api.h"

#ifndef spinlock_t
#define spinlock_t _Lock
#endif

#ifndef _atomic_spin_lock_irqsave
#define _atomic_spin_lock_irqsave(p, flags) 		SaveAndCli()
#endif
#ifndef _atomic_spin_unlock_irqrestore
#define _atomic_spin_unlock_irqrestore(p, flags)	RestoreFlags()
#endif

/* spin lock */
#ifndef spin_lock_init
	#define spin_lock_init(plock) 			RtlSpinlockInit((plock))
#endif
#ifndef spin_lock_free
	#define spin_lock_free(plock) 			RtlSpinlockFree((plock))
#endif
#ifndef spin_lock
	#define spin_lock(plock) 			RtlSpinlock((plock))
#endif
#ifndef spin_unlock
	#define spin_unlock(plock) 			RtlSpinunlock((plock))
#endif
#endif