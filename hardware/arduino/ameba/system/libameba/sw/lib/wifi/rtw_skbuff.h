#ifndef _RTW_SKBUFF_H_
#define _RTW_SKBUFF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "rt_queue.h"

#define MAX_SKB_BUF_SIZE		1650

//
#define 	HAL_INTERFACE_CMD_LEN			0
#define 	HAL_INTERFACE_CMD_STATUS_LEN   	0
#define 	HAL_INTERFACE_OVERHEAD			(HAL_INTERFACE_CMD_LEN+HAL_INTERFACE_CMD_STATUS_LEN)

//
#ifndef TXDESC_SIZE
#define TXDESC_SIZE 40
#endif

#ifndef WLAN_HDR_A4_QOS_LEN
#define WLAN_HDR_A4_QOS_LEN	32
#endif

#ifndef WLAN_MAX_IV_LEN
#define WLAN_MAX_IV_LEN		8
#endif

#ifndef WLAN_SNAP_HEADER
#define WLAN_SNAP_HEADER	8
#endif

#ifndef WLAN_ETHHDR_LEN
#define WLAN_ETHHDR_LEN		14
#endif


#define SKB_WLAN_TX_EXTRA_LEN	(TXDESC_SIZE + WLAN_HDR_A4_QOS_LEN + WLAN_MAX_IV_LEN + WLAN_SNAP_HEADER - WLAN_ETHHDR_LEN)

//
// structures
//


struct  sk_buff_head {
	struct list_head	*next, *prev;
	unsigned int 		qlen;
};


struct sk_buff {
	/* These two members must be first. */
	struct sk_buff		*next;		/* Next buffer in list */
	struct sk_buff		*prev;		/* Previous buffer in list */
	
	struct sk_buff_head	*list;		/* List we are on */	
	unsigned char		*head;		/* Head of buffer */
	unsigned char		*data;		/* Data head pointer */
	unsigned char		*tail;		/* Tail pointer	*/
	unsigned char		*end;		/* End pointer */ 
	//void	*dev;		/* Device we arrived on/are leaving by */	
	unsigned int 		len;		/* Length of actual data */	
};


typedef	struct sk_buff		_pkt;

//
// macro functions
//
#define dev_kfree_skb_any(skb)	kfree_skb(skb)


//
// inline functions
//

static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb->tail = skb->data + offset;
}

static inline unsigned char *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	skb->len-=len;
	skb->data = (unsigned char *)(((unsigned int)skb->data) + len);

	return skb->data;
}


//
// extern functions
//
extern void init_skb_pool(void);
extern void init_skb_data_pool(void);
extern struct sk_buff *dev_alloc_skb(unsigned int length, unsigned int reserve_len);
extern unsigned char *skb_put(struct sk_buff *skb, unsigned int len);
extern void skb_reserve(struct sk_buff *skb, unsigned int len);
extern void kfree_skb(struct sk_buff *skb);



#ifdef __cplusplus
}
#endif



#endif
