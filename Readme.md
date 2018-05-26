# iRhythm Internet MP3 Radio

This appilcation is a prototype of **Internet Radio** designed on ARC EM Starter Kit,which could find music from online audition websites,like *douban FM*,*Baidu FM*,*KuWo*,Download mp3 file among the music,and decode them by using **ARC DSP Instruction** and **optimized code** for mp3 decode.By adding a I2S peripheral with a **FPGA**,song can be sent out and outputted from two **speaker**.

-----------------------------------------

* [Introduction](#Introduction)
    - [Function](#function)
    - [System Architecture](#system-architecture)
    - [Appearance](#appearance)
* [Hardware and Software Setup](#hardware-and-software-setup)
    - [Required Hardware](#required-hardware)
    - [Required Software](#required-software)
    - [Hardware Connection](#hardware-connection)
* [User Manual](#user-manual)
    - [Before Running This Application](#before-running-this-application)
    - [Run This Application](#run-this-application)


-------------------------------

## Intruduction
iRhythm is a Internet Radio,which can download music from online audition websites,decompress the file and play,while the music file in SD card can also be decoded and played.
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
* **System Top View -- Control Part**
![Control Part][2]

* **System Middle View -- Signal Processing Part**
![Signal Processing Part][3]

* **System Bottom View -- Output Part**
![Output Part][4]



## Hardware and Software Setup

-------------------------------
### Required Hardware
* Necessary Hardware:
    - DesignWare ARC EM Starter Kit(EMSK)        *1
    - SD card                                    *1
    - ESP8266                                    *1
    - Cmod A7 XC7A35T FPGA develop board         *1
    - Pmod I2S                                   *1
    - OLED(12864)                                *1
    - Headphone                                  *1

![Necessary Hardware][5]
--------------------------------

* Additional Hardware:
    - Lipo Battery                               *1
    - Buck power regulator                       *2
    - Audio Amplifiers                           *1
    - Speaker                                    *1

![Additional Hardware][6]

---------------------------------

### Required Software
* Metaware or ARC GNU Toolset
* Serial port terminal, such as putty, tera-term or minicom
* Any bitstream file can config your FPGA as a SPI to I2S protocol interface
* More than one MP3 file in SD card to avoid waiting while downloading

### Hardware Connection
1. Insert your SD card with mp3 file in it.
2. Make sure all your module has been mounted on the board.
3. Make sure your power regulator connect is correct,including the voltage and polarity, or the prototype may be damaged
4. Connect your 3.5 inch Audio cable.
5. After all connector has been connected correctly,connect the Li-po battery to power up the design.

## User Manual

### Before Running This Application
* Download source code of iRhythm from github.
* Download ./doc/spi2i2s.bit,which is the bitstram file of Cmod A7 to extend a i2s peripherals
* Make sure all connection is correct again.
* Make sure iRhythm is in the Wifi environment,which is matching with the setting in the code,including wifi name and wifi password.
* Check the switch 1 and 2 of EMSK are in off state to boot as ARCem_7d core.
* 
### Run This Application
* Download with USB or use bootloader to boot the program.
* After iRhythm connect the wifi,initialization will complete very fast and OLED may light to show the song reading from SD card.
* After that song can be heard from the speaker and OLED shows download speed and decode speed.If not ,try to rotate the volume bar to increase the volume.
* the user can control with the key any time.






[1]: ./doc/pic/sysarch.png
[2]: ./doc/pic/top.jpg
[3]: ./doc/pic/mid.jpg
[4]: ./doc/pic/bottom.jpg
[5]: ./
[6]: ./
