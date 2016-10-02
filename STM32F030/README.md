Alarm detection for hearing impaired

"DMA mem func & reorg"
- use DMA for memset() and memmove()
- reorganize DMA, SPI calls from Nokia5110 into hardware.c
---------------------------------
"Visualization"
- added a simple graphic library
- added battery status and volume envelope
---------------------------------
"Lost in Space" pre-pre release
The PCB I have ordered has been lost in the mail.  This
is the 2nd one in a row and I don't know if/when they
would fix up the shipping.  This has major impact to the
schedule of this project.  Since there don't seem be much 
interest in this, I have decided to do an premptive
release of the firmware at the early stage.

Source code is under GPL3.0 with the following 3 files 
under their own licenses.

ST: startup_stm32f030.s
Elm-chan's: intfft.c, intfft.h
