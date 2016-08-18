"Alarm detection for hearing impaired"

Since the common block is useful for a few of my projects, 
I have splitted off the hardware design into smaller modular 
block for initial prototyping.

LCD Backpack: STM32F030 based PCB for mounting onto the back 
of a "Nokia" LCD.  It has anti-alias RC filter for ADC and an 
optional 3.3V boost mode power supply for running off two
Alkaline/NiMH batteries.

The firmware is based on the VU/Spectrum analyzer of the 
"Automatic-Audio-Switch".  I have increase the FFT resolution 
to about 37.5Hz and a bandwidth of 5kHz.
