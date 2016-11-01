
THis is an example for USB Video Capture specifically for motion-jpeg capturing. 

Please MAKE SURE to reserve enough heap size for UVC by raising configTOTAL_HEAP_SIZE in freeRTOSconfig.h & turning off some functions (e.g. WPS, JDSMART, ATcmd for internal and system) since image frame storing could consume quite large memory space.

TO switch on UVC example, make sure CONFIG_USB_EN is enabled (in platform_autoconf.h) & set CONFIG_EXAMPLE_UVC to 1 (in platform_opts.h).

TO combine uvc with rtsp server, make sure wlan module is enabled & set UVC_WLAN_TRANSFER to 1 (in example_uvc.h).