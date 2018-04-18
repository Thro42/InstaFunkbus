# InstaFunkbus
This is a Arduino Library for Accessing INSTA-Funkbus witch ist used be Gira, Becker and Jung until 2017. In case of the ending of Support from this firms this is only a proof of concept and I will not finish it. But I hope others can use my work. 
Many information about INSTA-Funkbus I found on https://www.mikrocontroller.net/topic/158243
But not all of this information was correct.
#### Protokoll
```sh
 ____ ____ ____ ________________ ____ ________________ _____
|****|xxxx|xxxx|xxxxxxxxxxxxxxxx|xxxx|xxxxxxxxxxxxxxxx|xxxxx|
|    |    |    |                |    |                |     |
 4Bit 4Bit 4Bit      16Bit       4Bit       16Bit      5Bit
       |    |         |           |          |          |
       |    |         |           |          |          #--Checksum
       |    |         |           |          #--Data          
       |    |         |           #--Battery state
       |    |         #--serialnumber
       |    #--subtype
       #--sendertype
```
'* = 4ms Hi signal on start
The transport ist AM bitmodulation on 433 Mhz
```sh
  __ __       __    __ __    __
 |     |     |  |  |     |  |  |
_|     |__ __|  |__|     |__|  |__.....
 |  0  |  0  | 	1  |  0  |  1  |
```
My Remotecontrol has sendertype 0010 and subtype 1100
#### Databits

	0000-0xxx-xx0x-xxxx
	      ||| || | ||||#-Parity bit
	      ||| || | |##-- button Presstime/Sequenz 
	      ||| || | |##-- 00 First in Seq., 10 short press seq. 2
	      ||| || | |##-- 00 First in Seq., 01 long press  seq. 2, 11 long press seq. 3
	      ||| || | #---- OFF (OFF = 1)
	      ||| || #------ ON  (ON  = 1)
	      ||| ||
	      ||| ##-------- 00 = CH A
	      ||| ##-------- 10 = CH B
	      ||| ##-------- 01 = CH C
	      ||| ##-------- 11 = Licht senen modus (LS)
	      |||
	      ###----------- 000 = button 1 or LS1
	      ###----------- 100 = button 2 or LS2
	      ###----------- 010 = button 3 or LS3
	      ###----------- 110 = button 4 or LS4
	      ###----------- 001 = button 5 or LS5
	      ###----------- 101 = button 6 or LS6
	      ###----------- 011 = button 7 or LS7
	      ###----------- 111 = button 8 or LS8

##### Sequenz
The remote control sends a a sequence of package. Minimum 3 packages. On a short press of a button the first package is difrent from the other. On as long press of a button it sends three types of packages. The differnts I found in the "Presstime/Sequenz" bits.
```sh
00 | First package in sequence
10 | package 2 and folow in sequence for short press of a button
01 | package 2 and 3 in sequence for long press of a button
11 | package 4 and folow in sequence for long press of a button
```

#### Checksum
The Checksum is a big problem In the Article on https://www.mikrocontroller.net/topic/158243 "Wireless Avans" writes that he found the algorithm, that's wrong.
###### What I known
```sh
 1234
 xxxx1--- Last bit always 1
 #### ---- 1. negate the pattern from ON if OFF
   #------ 2. negate this if bit 1 of "Presstime/Sequenz" bits is 1
 #-------- 3. negate this if bit 2 of "Presstime/Sequenz" bits is 1
```
