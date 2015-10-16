Arduino core for Realtek Ameba WiFi chip
===========================================

This project brings support for Realtek Ameba chip to the Arduino environment. It lets you write sketches using familiar Arduino functions and libraries, and run them directly on Ameba, no external microcontroller required.

Realtek Ameba Arduino core comes with libraries to communicate over WiFi using TCP and UDP, HTTP client and server and relative network applications, 
use a file system in flash memory, work with SD cards, servos, SPI and I2C peripherals, and etc..


### Building latest version from source 

```
$ git clone https://github.com/Ameba8195/Arduino.git
$ cd Arduino/build
$ ant run
```


### Issues and support ###

The best place to ask questions related to this core is Realtek Ameba community forum: http://amebaforum.com/forums/.

If you encounter an issue, you are welcome to submit it here on Github: https://github.com/Ameba8195/Arduino/issues.
Please provide as much context as possible: version which you are using (you can check it in Boards Manager), your sketch code, serial output, board model, IDE settings (board selection, flash size, etc).

Also can email to neo_jou@realtek.com , any suggestion or advice would be appreciated. 

### Contributing

For minor fixes of code and documentation, go ahead and submit a pull request.

Larger changes (rewriting parts of existing code from scratch, adding new functions to the core, adding new libraries) should generally be discussed 

Feature branches with lots of small commits (especially titled "oops", "fix typo", "forgot to add file", etc.) should be squashed before opening a pull request. At the same time, please refrain from putting multiple unrelated changes into a single pull request.

### License and credits ###

Arduino IDE is developed and maintained by the Arduino team. The IDE is licensed under GPL.

