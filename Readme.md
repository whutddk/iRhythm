# iRhythm Internet MP3 Radio

This appilcation is a prototype of **Internet Radio** designed on ARC EM Starter Kit,which could find music from online audition websites,like *douban FM*,*Baidu FM*,*KuWo*,Download mp3 file among the music,and decode them by using **ARC DSP Instruction** and **optimized code** for mp3 decode.

-----------------------------------------

* [Introduction](#Introduction)
    - [Function](#function)
    - [System Architecture](#system-architecture)
    - [Appearance](#appearance)


-------------------------------

## Intruduction
iRhythm is a Internet Radio,which can download music from online audition websites,decompression the file and play,while the music file in SD card can also be decoded and played.
With the help of a FPGA, the digital music data in SPI format can be converted into I2S format,which can be recognize by audio D/A convertor.
Through an amplifier outside,the online music can be play out from two big speaker.And it's a really good feeling!

### Function
* Online Music Crawling and Download
* Decode Mp3 file 
* Protocol transit form SPI to I2S 
* Power Amplification 
* Performance data display

### System Architecture
![System Architecture][1]

### Appearance
* System Top View -- Control Part
![Control Part][2]

* System Middle View -- Signal Processing Part
![Signal Processing Part][2]

* System Bottom View -- Output Part
![Output Part][3]



[1]: ./doc/pic/sysarch.png
[2]: ./doc/pic/top.jpg
[3]: ./doc/pic/mid.jpg
[4]: ./docpic/bottom.jpg



<!-- ## Hardware and Software Setup

### Required Hardware
* Necessary Hardware:
    - DesignWare ARC EM Starter Kit(EMSK)        *1
    - SD card                                    *1
    - ESP8266                                    *1
    - Cmod A7 XC7A35T FPGA develop board         *1
    - Pmod I2S                                   *1
    - OLED(12864)                                *1
    - Headphone                                  *1
* Additional Hardware:
    - Lipo Battery                               *1
    - Buck power regulator                       *2
    - Audio Amplifiers                           *1
    - Speaker                                    *1


### Required Software
* Metaware or ARC GNU Toolset
* Serial port terminal, such as putty, tera-term or minicom
* Any bitstream file can config FPGA as a SPI to I2S protocol interface
* More than one MP3 file in SD card to provent avoid waiting while downloading

### Hardware Connection

## User Manual

### Before Running This Application

### Run This Application





 -->