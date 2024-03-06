## Title of your project
Tetris: XXXX

## Team members
Anjali Sreenivas (anjalisr), Aditi Bhaskar (aditijb)

## Project description

Layer 1 (P1) – 
•	Clean falling of colored tetris pieces 
•	Working accelerometer that translates into correct ID of motion (left/right/down) --> interrupt system (based on time) set up
•	Single row clearing – clear bottom most filled row
•	3D printed remote control
•	Scoring
Layer 2 (P2) – 
•	Ability to rotate (increments of 90 deg clockwise) falling tetris piece (button)
•	Single AND double row clearing support – vibration from servo
•	Visible queue of next 3 pieces 
•	Gradient squares in tetris pieces :)
Layer 3 (P3) –
•	Some creative power up?
•	Color mode switching?

### Hardware, budget

# Budget - $38 ish

# Things we still need (to borrow):
 * 3-5ft HDMI cable -- borrow
 * 3-5ft usbC-usbA cable -- borrrow
 * mango pi (additional) -- $20

# Things we already borrowed:
 * 1 accelerometer: LSM6DS33 -- $10
 * 1 active buzzer on breakout -- <$2
 * 1 passive buzzer on breakout -- <$2

## Parts/Physical modules
 * Handheld controller device (think Wii remote but simpler)
 * Music (via. buzzer)
 * Connects to monitor via. long hdmi cable (tethered controller) & long usbC-usbA cable (download code + power)

# MangoPi (microcontroller) (processor)
 * NEED FROM 107E LAB - another mango pi? (to solder directly to have a more compact remote)

# Accelerometer (tilt) (input)
 * FROM 107E LAB -- LSM6DS33 -- Polulu accelerometer
 * MSA311 datasheet (MY_I2C_ADDR = 0x62) -- if I decide to use Adafruit accelerometer

# Buttons (as necessary) (input)
 * 1 button configured w/ interrupt

# Servo (for vibration/haptic feedback) (output)
 * 1 servo configured for vibrate
 * https://learn.sparkfun.com/tutorials/basic-servo-control-for-beginners/all 
brown GND, red 4.8-6V, orange CTRL (PB1/PWM4)
 * https://learn.sparkfun.com/tutorials/hobby-servo-tutorial#servo-motor-background 
The pulses occur at a 20 mSec (50 Hz) interval, and vary between 1 and 2 mSec in width.
 * https://www.makerguides.com/servo-arduino-tutorial/ - PWM control

# Active/passive buzzer (music!) (output)
 * FROM 107E LAB
 * https://blog.tarkalabs.com/digital-audio-101-playing-audio-from-a-microcontroller-5df1463616c 
has code

Itemize what hardware, if any, you need and your plan for acquiring it.
Rough budget for the project. We can reimburse cost of parts up to $20 per person.

## Tasks, member responsibilities
Anjali
Each team member should be responsible for a clearly identifiable part of the project.

## Schedule, midpoint milestones
Aditi - put yours
I'll put mine
What are the one-week goals you plan to meet for next week?

## Resources needed, issues
 * Are there books/code/tools/materials that you need access to? 
 * What are the possible risks to overcome? What are you concerned about? 
 * How can we help you succeed?

### references
 * references linked in materials section

### rishs
 * accelerometer (Aditi wants to write her own driver for the MSA311?? maybe??) 
 * parallel operating (how do we run the music at the same time as displaying the game? lots of interrupts??)
 * mechanical failures (obviously)

### route for success
 * I (Aditi) will definitely be coming by to office hours/posting on the Ed for help

