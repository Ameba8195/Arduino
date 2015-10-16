#ifndef _RT_QUEUE_H_
#define _RT_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef void*	_lock;


struct list_head {
	struct list_head *next, *prev;
};

struct	__queue	{
	struct	list_head	queue;
	_lock				lock;
};

typedef struct	list_head	_list;
typedef struct	__queue		_queue;
typedef unsigned long		_irqL;


//
// macro functions
//
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr)-(SIZE_T)((char *)&((type *)ptr)->member - (char *)ptr)))


//
// external functions
//

extern void list_add_tail(struct list_head *cur, struct list_head *head);

extern void rtw_init_listhead(_list *list);

extern u32 rtw_is_list_empty(_list *phead);

extern void rtw_list_delete(_list *plist);

extern void	rtw_init_queue(_queue	*pqueue);

extern u32	rtw_queue_empty(_queue	*pqueue);

extern u32 rtw_end_of_queue_search(_list *head, _list *plist);

//
// static inline functions
//
static inline void 
	__list_add(
		struct list_head * cur,
		struct list_head * prev,
		struct list_head * next)
{
	next->prev = cur;
	cur->next = next;
	cur->prev = prev;
	prev->next = cur;
}

static inline void 
	__list_del (
		struct list_head * prev,
		struct list_head * next
	)
{
	next->prev = prev;
	prev->next = next;
}

static inline int list_empty(struct list_head *head)
{
	return head->next == head;
}

static inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry); 
}

static inline _list	*get_list_head(_queue	*queue)
{
	return (&(queue->queue));
}

static inline _list *get_next(_list	*list)
{
	return list->next;
}	

static inline void rtw_list_insert_tail(_list *plist, _list *phead)
{
	list_add_tail(plist, phead);
}




#ifdef __cplusplus
}
#endif



#endif
