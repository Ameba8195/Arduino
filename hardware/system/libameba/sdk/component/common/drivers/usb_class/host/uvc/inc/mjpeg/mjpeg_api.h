#ifndef _MJPEG_API_H
#define _MJPEG_API_H


/* memory disk type */
typedef enum _medium_type{
	medium_SD,
	medium_USB,
	medium_FLASH,
	medium_CACHE
}medium_type;

/* time unit*/
typedef enum _time_unit{
	unit_HR,  // hour
	unit_MIN, // minute
	unit_SEC, //second
}time_unit;

typedef struct _mjpeg_cache{
	unsigned char* addr;
	int size;
}mjpeg_cache;

struct mjpeg_context{
	/* mjpeg size*/
	int width;//frame width
    int height;//frame height
	/* */
	medium_type disktype;
	time_unit timeunit;
	int interval;
	unsigned char* name;
	unsigned char is_periodic;
	/* store mjpeg in a RAM cache */
	mjpeg_cache cache;
};

int mjpeg_get(struct mjpeg_context *context);

#endif
