# jank

jank is **j**ust **an**other **k**eypad.  Specifically it is a USB numeric keypad with some macro buttons.  It's kind of an evolution of another project ([macro][link_gitlab_macr0]).

Additional photos and detail can be found at [clews.pro][link_clews_jank].

Features include:
* Uses a USB type C connector (configured to identify as a USB 2.0 device).
* 21 keys total - 17 keys configured in a standard keypad arrangement, plus an addition row of four keys across the top.
* All 21 keys can be configured as any reguglar keyboard keystroke (including media control keys) or a programmable macro.
* The keys are all mechanical keyswitches - I used gateron blues which are pin-comnpatible with [Cherry MX][link_web_cherry_mx] keyswitches.
* There is a 3mm led beneath each keyswitch so that the legends are all back-lit.
* The LEDs are powered via a boost controller which is enabled by a PWM signal from the microcontroller - this allows variable brightness.  Brightness is set by a tact switch on the bottom of the unit.  Pushing the switch cycles the LEDs from off through increasing levels of brightness and a pulsing effect with three different speeds.
* There is a single smd LED on the bottom of the unit that is programmed to function as the NUM LOCK indicator.

Although any key can be configured to function as any keystroke or macro, I use jank with my laptop as a traditional numerical keypad.  Only the top four keys are configured as macros.

*********************************
**Note this is a work-in-progress**

To do:

- [ ] Hardware
        - [X] Parts selection.
        - [X] Schematic design.
        - [X] PCB layout design.
        - [X] Parts order.
        - [X] PCB assembly.
        - [ ] Enclosure.
- [ ] Firmware
        - [X] LED control.
        - [X] Regular keystrokes.
        - [X] Num lock LED.
        - [X] Simple macros.
        - [ ] Complex macros.

*********************************

[link_clews_jank]:https://clews.pro/projects/jank.php
[link_gitlab_macr0]:https://gitlab.com/clewsy/macr0
[link_web_cherry_mx]:https://www.cherrymx.de/en
