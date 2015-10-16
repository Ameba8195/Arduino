 /******************************************************************************
  *
  * Name: sys-support.h - System type support for Linux
  *       $Revision: 1.1.1.1 $
  *
  *****************************************************************************/

#ifndef __OS_SUPPORT_H__
#define __OS_SUPPORT_H__

#include <basic_types.h>


#ifdef CONFIG_TIMER_MODULE
extern _LONG_CALL_ u32 HalDelayUs(u32 us);
#define __Delay(t)                  HalDelayUs(t)
#else
static __inline__ u32 __Delay(u32 us)
{
    DBG_8195A("No Delay: please enable hardware Timer\n");
}
#endif


#define Mdelay(t)					__Delay(t*1000)
#define Udelay(t)					__Delay(t)


#define ASSERT(bool)					do { } while (!(bool))


#endif /* __SYS_SUPPORT_H__ */
