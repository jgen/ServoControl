ServoControl
============

This is the Servo Control program for the servo board produced by [WE Bots](http://www.eng.uwo.ca/webots/).

Status
------

Please note that this is a work in progress! The master branch should be in a running state at all times. No assurances
for the development branches though.

Setup
-----

This project is built using the development environment [Qt Creator](http://qt.nokia.com/products/developer-tools/).

(You need to have Qt installed before compiling and working on this.)

Contributing
------------

Feel free to fork this project, open issues, report bugs, and open pull requests. Please try to keep the master branch working at all
times, this project is used actively for robotics projects.

Documentation
-------------

To generate nice HTML based documentation from the source code, you can use [doxygen](http://www.doxygen.org/).
A configuration file is provided with some basic settings to get you started.
You can run the following command line, or use the GUI/wizard to generate the documentation.

doxygen <config-file>



License
-------

This project is released under the [GNU GPL version 2](http://www.opensource.org/licenses/gpl-2.0.php).


Copyright
---------

Sections Copyright (c) 2011-2012 Jeff Genovy.

Sections Copyright (c) 2011-2012 Western Engineering Robotics Design and Engineering Club

Sections Copyright (c) 2012 Adam Wilson..




# File Formats for *.SER and *.SERVO Files

There are two file formats that can be created or read by the program for the storage of servo sequences. This functionality is designed to be used for testing a system through the playback of a predetermined set of motions, or to design motion sequences for statically stable robots.

The two formats are supported for compatibility with the older versions of the program.  The SERVO files are the preferred  choice for use with this program as all options can be saved in them, including sweeps and starting positions. The SER files can be read by the various versions of the Servo Controller written by Eugen Porter from 2009-2012. These do not support advanced features such as starting positions and variable delays and sweeps. 

If there is an option use of the newer SERVO file type is preferred. No guarantees are made as to when support for older files will be continued until, but every effort will be made to include support in future versions.


## Legacy File Format (SER):

This is the file format that was read by the older versions of the program. It contained a header with 6 fields and a body with as many lines as needed.

<pre>
AAA,BBB,CCC,DDD,EEE,FFF
*GGG,HHH[Repeat up to 12 Pairs]
[Repeat as needed]
</pre>

### Run format (AAA)

This indicated how the data would be sent to the board. Direct run meant that the sequence would be sent position by position and each one would be played as soon as it arrived at the board. Sequence 12, 3, and 1 are used for storing sequences in the EEPROM of the micro. Currently this functionality is unimplemented on the board or in the program. The freeze command was used to stop the board from changing the duty cycle of the output, but is no longer used in this capacity, reading 004 from a SERVO file should be considered an error.
<pre>
  0=======Direct Run
	1=======Sequence 12
	2=======Sequence 3
	3=======Sequence 1
	4=======Freeze
</pre>
### Data Bank (BBB)

This was used to control the data bank that the sequence is being written into. This is unused in both SER and SERVO files, but was kept to ensure backwards compatibility.

	<pre>000 -> 005 </pre>


### PWM Repeat (CCC)

This is used to store the number of times that a PWM value will be sent out (with an 8ms time base), before the sweep value is added to it. The value in the left hand column is what will be sent to the board and the value on the right is the number of times that it is repeated.

<pre>
	0=======1
	1=======2
	2=======10
	3=======25
	4=======50	
	5=======100
	6=======150
	7=======200
</pre>

### PWM Sweep (DDD)

This is the value that will be added or subtracted from the previous PWM value after the number of repeats are exhausted. To turn this feature off, set the value to zero and there will be no sweep in the values, instead it will jump to the next one immediately.
<pre>
	000 -> 015
</pre>

### Sequence Delay (EEE)

This is the number seconds between positions in a sequence being played back to the board. Despite the fact that it will accept values from 0 to 15, a 0 value will cause the board to hang later in execution and need to be restarted. Thus any zero values will be ignored silently at transfer time. 
<pre>
	000 -> 015
</pre>

### Sequence Replay (FFF)

This is the number of times that a sequence will be played. The number on the left is the value that is sent to the board, and the value on the right is the number of times that the sequence will be replayed. The mapping between the values is done using a lookup table embedded in the program code of the mirco (to save memory, there is a ton of program memory on the chip), and thus cannot be changed without changing the firmware on the chip. 

<pre>
	0=======1
	1=======2
	2=======10
	3=======25
	4=======50	
	5=======100
	6=======150
	7=======200
</pre>

### Address (GGG) Data (HHH) Pairs

The address data pairs are comma delimited values that indicate the address of a servo to send a command to and the data value to be sent to it. The address ranges from 1 to 12, with the outputs clearly labelled on the board. The data values range from 1 to 97 and correspond to the pulse width, ranging from 0.5ms to 2.5ms, with an 8ms period. The mapping between the data and the position or speed of a motor when attached to such a signal is device dependant and thus no assumptions are made towards angles or speeds. 

There can be up to 12 pairs per line in the body of the file Each address can only be used once, but every address does not have to be used on every line. The start of the line is marked with the * symbol along with a preceding newline (\n) character. 

<pre>
 Address of Servo
	001 -> 012

  Data for servo
	001 -> 097
</pre>


## Current File Format (*.SERVO)

The SERVO file format is the replacement for the older SER file to allow for the use of newer features that have been added to the board since the first version. It allows for the use of line scope sweeps and delays, starting positions, and freezes, along with support for commenting. It is preferred that new files are saved in this format, as there is no risk of losing information on the read/write process.  

The file contains a mandatory 6 field header, options start positions, and a body which can contain as many comments and position lines as required. The header fields are kept identical to simplify the reading of different file formats although some of the fields are depreciated. 

<pre>
AAA,BBB,CCC,DDD,EEE,FFF
Start:*GGG,HHH [optional, must have 12 pairs]
#This is a comment
*GGG,HHH [repeat up to 12 pairs]
&GGG,HHH [repeat up to 12 pairs]
[repeat mixing * and & lines as needed.]
</pre>

### Run format (AAA)

As of this point the run format is ignored on reading, but it can be used to confirm if a position is the correct format for writing to the EEPROM. The value on the left is what will be stored in the file and the value on the right is its meaning. 

<pre>
	0=======Direct Run
	1=======Sequence 12
	2=======Sequence 3
	3=======Sequence 1
	4=======Freeze
</pre>

### Data Bank (BBB)

This is no longer used, but is kept for compatibility reasons. 
<pre>
	000 -> 005
</pre>

<a name="PWMRep"/>
### PWM Repeat (CCC)

This is the global value for PWM repeat that will be used if there is no value supplied on a given line. PWM Repeat is the number of times that a calculated value will be resent to the output (8ms timebase) before the PWM Sweep value is added to it. This value has no meaning if the sweep value is 0, as there will be no values calculated. 

Note that PWM repeat and sweep must be found on a line in a pair to override the global values, otherwise they will be ignored. 
<pre>
	0=======1
	1=======2
	2=======10
	3=======25
	4=======50	
	5=======100
	6=======150
	7=======200
</pre>

<a name="PWMSweep"/>
### PWM Sweep (DDD)             
This is the value that will be added to the previous value for a servo when the number of repeats has been finished. This is used to control the how course the sweeping motion of the PWM will be.

For example if the sweep is 5 and the repeat is 2 with the PWM starting at the initial value of 10. If then a command to change it to 30 is sent then sweep will be added to value (5 +10) and the resulting value will be sent and held for 2 repeats (2 * 8ms = 16ms) then 5 will be added again and 20 will be held for 16ms. This is repeated until either 30 is reached or another command is sent to the board. 

<pre>
	000 -> 015
</pre>

### Sequence Delay (EEE)

This is the delay between lines that will be used if there is no line level delay specified. If the value given is 0, it will be changed as it is not possible to have a zero delay between lines. 
<pre>
	000 -> 015
</pre>


### Sequence Replay (FFF)

This is the number of times that a sequence will be played. The number on the left is the value that is sent to the board, and the value on the right is the number of times that the sequence will be replayed. The mapping between the values is done using a lookup table embedded in the program code of the mirco (to save memory, there is a ton of program memory on the chip), and thus cannot be changed without changing the firmware on the chip. 

<pre>
	0=======1
	1=======2
	2=======10
	3=======25
	4=======50	
	5=======100
	6=======150
	7=======200
</pre>

### Start Position

This sets the starting position for the sequence that can be flashed into EEPROM to have the board always powerup at the same position. The position following Start: must be in the older format (no freeze, PWM, or delay values) and must have values for all 12 addresses. If these conditions are not met the start position will be ignored. 

<pre>
     *001,HHH,002,HHH,003,HHH,004,HHH,005,HHH,006,HHH,007,HHH,008,HHH,009,HHH,010,HHH,011,HHH,012,HHH
      Where HHH is in the range 1 --> 97
</pre>


### Line Start Characters

The first character on a position line must be either a * or an &. The * indicates a line is sent the same way as Eugen's code, where there is no freeze/unfreeze sent to the board. The & means there needs to be a freeze sent at the start of the line and unfreeze when the entire line is sent.

The freeze unfreeze means that in situations where timing is crucial. Instead of changes being applied as they arrive over a 9600 baud line, which can more than 25ms, a freeze command is sent first which stops the board from changing the outputs until all the data is received and an unfreeze command is sent. 


### Address (GGG) Data (HHH) Pairs

The address data pairs are comma delimited values that indicate the address of a servo to send a command to and the data value to be sent to it. The address ranges from 1 to 12, with the outputs clearly labelled on the board. The data values range from 1 to 97 and correspond to the pulse width, ranging from 0.5ms to 2.5ms, with an 8ms period. The mapping between the data and the position or speed of a motor when attached to such a signal is device dependant and thus no assumptions are made towards angles or speeds. 

There can be up to 12 pairs per line in the body of the file Each address can only be used once, but every address does not have to be used on every line. The start of the line is marked with the * symbol along with a preceding newline (\n) character. 

<pre>
 Address of Servo
	001 -> 012

  Data for servo
	001 -> 097
</pre>

### Line Scope PWMSweep and PWMRepeat 

There is the option to include line scope PWM sweep and repeat values in the file, they will be used for the single line that they are found on and overwritten with the global values after the lines has been played. The [PWM sweep](#PWMSweep) and [PWM repeat](#PWMRep) obey the same rules as the global scope ones. The PWM repeat is the right hand value (eg 50) not the left hand one (eg. 3) from the enumerator. 

<pre>
PWMRep,000,PWMSweep,000
</pre>
 

### Line Level Delay

This is used optionally to set the delay between this line and the next line in the sequence. The value in seconds must be between 1 and 15.

<pre>
SeqDelay,000 
</pre>

### Comments

A line starting with # will be  ignored and treated as a comment. Comments will appear in the display box in the program in the same location as they were saved in the file. There are no inline comments available. 


## Reasoning

This section is intended for advanced users and developers to understand the reason behind a number of choices that were made. 

+ The header was unchanged so that older files can be read of the start and most of the information is still needed, so it can be used for now. 

+ New files should be able to be converted to older format files through only deletion, allowing for easier transfer to machines that only run the older versions. 





