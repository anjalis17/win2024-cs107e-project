## Title of your project
--------------------------
Tiltris 
(It's Tetris, but you control lateral motion of the falling piece by tilting a 3D printed remote control!)

## Team members
--------------------------
Anjali Sreenivas (anjalisr), Aditi Bhaskar (aditijb)

## Project description
--------------------------

Layer 1 (P1) - Building Blocks
---------------------
•   Clean falling of colored tetris pieces 
•   Working accelerometer that translates into correct ID of motion (left/right/down) --> interrupt system (based on time) set up
•   Correct handling of left/right/down motion (software/graphics)
•   Single row clearing functionality – clears all filled rows after move
•   Scoring system
•   End game functionality

Layer 2 (P2) - Tying Game Together!
---------------------
•   Ability to rotate falling tetris pieces (in increments of 90 deg clockwise) -- software side
•   Button configuration (interrupt system) to support user-controlled piece rotation
•   Support for double/triple row animation clearing
•   Servo motor/vibration triggered when user clears row
•   Visible queue of next 3 pieces 
•   Graphics optimized to the extent where the whole screen is not redrawn for each change made

Layer 3 (P3) - The Cherries on Top :)
---------------------
•   Gradient squares in tetris pieces!
•   3D printed remote control to encapsulate all sensors
•   Replay screen
•   Tetris theme song
•   Leaderboard (name + score) -- keyboard integration


## Hardware, budget
--------------------------

# Budget - $38 ish

# Things we still need (to borrow):
 * 3-5ft HDMI cable -- borrow
 * 3-5ft usbC-usbA cable -- borrrow
 * mango pi (additional) -- $20

# Things we already borrowed:
 * 1 accelerometer: LSM6DS33 -- $10
 * 1 active buzzer on breakout -- <$2
 * 1 passive buzzer on breakout -- <$2

# PARTS / PHYSICAL MODULES
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
 * for songs!
 * https://blog.tarkalabs.com/digital-audio-101-playing-audio-from-a-microcontroller-5df1463616c 
has code

Itemize what hardware, if any, you need and your plan for acquiring it.
Rough budget for the project. We can reimburse cost of parts up to $20 per person.

## Tasks, member responsibilities
--------------------------
Anjali (Graphics / Game Display) 
- Core software underlying game functionality/display
    - Graphics display & update algorithms 
    - Falling tetris pieces
    - Handling motion of pieces (right/left/down)
    - Handling rotation of pieces
    - Row clearing

Aditi (Sensors / Actuators / Input-Display Integration)
- User input & actuators reading/processing + integration with game
    - CAD/engineering of 3D print remote control
    - Accelerometer module & interrupt config
    - Button system
    - Servo/vibration output
    - Piezo buzzer (song)
    - Scoring / additional game feature support

## Schedule, midpoint milestones
--------------------------
What are the one-week goals you plan to meet for next week?
ANJALI
After high-level architecture plan...
- Set up underlying grid graphics system structure
- Set up updating / piece motion framework
- Set up left/right/down movement in conjunction with falling piece - temporarily use keyboard interrupts
- Random config of falling piece
- Stretch goal -- start workig on piece rotation  

ADITI
XXXXXX FILL IN XXXXX

## Resources needed, issues
 * Are there books/code/tools/materials that you need access to? 
 * What are the possible risks to overcome? What are you concerned about? 
 * How can we help you succeed?

# References
 * References linked in materials section

# Risks
HARDWARE
 * accelerometer (Aditi wants to write her own driver for the MSA311?? maybe?) 
 * mechanical failures
 SOFTWARE
 * Parallel operating (how do we run the music at the same time as displaying the game? lots of interrupts??)
 * Game optimization / speed when it comes to redrawing -- efficiently handling rotation, lateral motion
 * Multiple interrupts changing the same static variables (cooridnate position of falling piece)

# Route for success
 * I (Aditi) will definitely be coming by to office hours/posting on the Ed for help
 * Anjali - encouragement along the way, especially when I get stuck or nervous! Patience / willingness to let me walk through my code and potentially offer a debug idea when I run into a bug I can't figure out :'/ 
 THANK YOU in advance!!! <3 We appreciate you all!
