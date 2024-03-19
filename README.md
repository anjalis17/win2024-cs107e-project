## Project title
Tiltris

## Team members
Anjali Sreenivas (anjalisr), Aditi Bhaskar (aditijb)

## Project description
Tetris, but with a tiltable controller

## Features
### remote (remote, servo, lsd6ds33, i2c):
 - novel design (by Aditi!) 
   - ergonomic shape
   - compact design 
   - button clicks well!!!
   - mango-cushion remote handle (removable)
 - game:
   - tilt left or right to control horizontal movement of falling piece
   - tilt down to drop piece faster
   - tilt up to to swap piece with next piece in queue
   - button (using queued interrupts on button press) to turn game piece clockwise
### leaderboard (game_interlude):
 - user-friendly design to add players to leaderboard:
   - use button to iterate/select on-screen and tilt down to continue to next screen
### game (game_update, random_bag):
 - // @anjali TODO ADD GAME FEATURES 
### music (passive_buzz_intr):
 - Plays the song! This means MULTITASKING!! 
 - Music library allows user to initialize/change tempo, and play a song on repeat USING INTERRUPTS! 
 - Notes that compose a song are specified by a (1) frequency (note letter) and (2) duration (whole/half/quarter/eighth)
 - Very friendly interface for people who know western classical music
 - passive_buzz files are from a previous version of the passive buzzer implimentation: they play notes directly instead of controlling the PWM via interrupts

## Member contribution
TODO A short description of the work performed by each member of the team.
Aditi:
 - for the most part, hardware-side + leaderboard/etc.
 - files: remote, servo, lsd6ds33, i2c, passive_buzz, passive_buzz_intr, game_interlude 
Anjali:
 - for the most part, the game itself
 - files: game_update, random_bag
Together:
 - calibrating remote angles, deciding on "new"/next features to add, debugging
 - we mostly worked together because of the nature of our project (need remote and monitor) 

## References
 - all references are also inline in our code, but here is a short summary:
   - i2c and lsd6ds33 libraries were borrowed and modified from Julie's week 8 Sensors lecture. The code is commented to deliniate what is borrowed vs. new
   - Talked to Javier, Antonio briefly about multitasking and where the timer interrupt clear functions were

Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work.

# Sharing Code
Our code for i2c.c (a slighly more robust version which checked for outgoing addresses to be ACK'ed) was shared with Sazzad and Aditri's teams. They both said they would cite me. Thought it was worth mentioning here for good measure.

## Self-evaluation
### How well was your team able to execute on the plan in your proposal?  
Our team executed our goals very well. This is probably due to our well-thought-out schedule and the manageable steps we outlined. We managed to execute all p1 tasks within the first week, and continued adding features both from our initial specs (eg. music, swap) but also features that we found cool/useful (eg. leaderboard). We ended up accomplishing all the tasks we wanted to accomplish, and then some, from our initial requirements list!

### Any trying or heroic moments you would like to share? Of what are you particularly proud: the effort you put into it? the end product? the process you followed? what you learned along the way? Tell us about it!


TODO

## Photos
Photos are in the /documents directory 

You are encouraged to submit photos/videos of your project in action. 
Add the files and commit to your project repository to include along with your submission.

TODO
