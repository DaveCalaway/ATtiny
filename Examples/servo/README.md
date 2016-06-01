# ATtiny Servo Motor
Before used the Servo library in Arduino, you must add a new base code for abilitated the 16Mhz internal clock.  

Open the IDE, go to preferences, and click on last link in a page, referenced to *preferences.txt*.  
Go to attiny -> hardware - > avr -> 1.0.1 .  
Open the *board.txt* file.  

Now search this lines:  
```attiny.menu.clock.internal8=8 MHz (internal)
attiny.menu.clock.internal8.bootloader.low_fuses=0xe2
attiny.menu.clock.internal8.bootloader.high_fuses=0xdf
attiny.menu.clock.internal8.bootloader.extended_fuses=0xff
attiny.menu.clock.internal8.build.f_cpu=8000000L```

And add under:  
```attiny.menu.clock.internal16=16 MHz (internal)
attiny.menu.clock.internal16.bootloader.low_fuses=0xf1
attiny.menu.clock.internal16.bootloader.high_fuses=0xdf
attiny.menu.clock.internal16.bootloader.extended_fuses=0xff
attiny.menu.clock.internal16.build.f_cpu=16000000L```

Now rebout the IDE.  
Congratulations you have activated the 16 Mhz internal clock options.

Remember, when using this option on a new chip or changing from another clock speed, use the *Burn bootloader* menu option.