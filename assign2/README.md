Hi!! THANK YOU so much for reading! Please find here notes/documentation for my clock extension -

General notes:
- I deliberately separated my clock helper functions from my countdown helpers to ensure that my extension code
did not interfere with my core functionality
- SUPER IMPORTANT (!!!) - Please test only using LONG BUTTON PRESSES, and have a slight delay in between your subsequent button presses
for proper registry of button pressing. I check all potential button presses (readings of 0) through my debouncer helper
(debounced_reading()) which verifies that a potential changed state remains across a small delay. 
- My clock represents time in military time (00:00 - 23:59).

Functionality:
1) START the clock by entering the RESET MODE. You should see a blinking 0000 on the LED display. 
2) ENTER RESET MODE (to set time) by simultaneously pressing both buttons (PG12 and PG13). You should see blinking digits. 
3) SET TIME within reset mode by long pressing on the hour button (PG12) to increment hours, and long 
pressing on the minute button (PG13) to increment minutes*. You CANNOT press both buttons at the same time (see point (4))!
4) EXIT RESET MODE to fix your set time by once again simultaneously pressing both buttons (PG12 and PG13). You will now be in normal clock
mode!! You should no longer see blinking and the clock should be functional!
5) To re-configure set time at any point, simply repeat steps 2 through 4. You can do this any number of times. YAY! Enjoy!!

*You can press and hold the appropriate button to continuously increment the hour or minute count!
