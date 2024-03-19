## Project title
### Tiltris

## Team members
Anjali Sreenivas (anjalisr), Aditi Bhaskar (aditijb)

## Project description
Tetris, but with a tiltable controller

## Features
### remote (remote, servo, lsd6ds33, i2c):
 - novel design (by Aditi; conceptually based on a Wii remote) 
   - ergonomic shape
   - compact design 
   - button clicks well!!!
   - clay-based mango-shaped cushion remote handle (removable)
   - I totally made all the measurements for my CAD with a bendy ruler :)
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
### music (passive_buzz_intr, music):
 - Plays the song! This means MULTITASKING!! 
 - Libraries allow user to initialize/change tempo, and play a song on repeat using interrupts! Very friendly interface for people who know western classical music
 - Notes that compose a song are specified by a (1) frequency (note letter) and (2) duration (whole/half/quarter/eighth)
 - passive_buzz files are from a previous version of the passive buzzer implementation: they play notes directly instead of controlling the PWM via interrupts

## Member contribution
Aditi:
 - for the most part, hardware-side + leaderboard/etc.
 - files: remote, servo, lsd6ds33, i2c, passive_buzz, passive_buzz_intr, game_interlude 
Anjali: TODO if you wanna add anything
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

## Sharing Code
Our code for i2c.c (a slighly more robust version which checked for outgoing addresses to be ACK'ed) was shared with Sazzad and Aditri's teams. They both said they would cite me. Thought it was worth mentioning here for good measure.

## Self-evaluation
### How well was your team able to execute on the plan in your proposal?  
Our team executed our goals very well. This is probably due to our well-thought-out schedule and the manageable steps we outlined. We managed to execute all p1 tasks within the first week, and continued adding features both from our initial specs (eg. music, swap) but also features that we found cool/useful (eg. leaderboard). We ended up accomplishing all the tasks we wanted to accomplish, and then some, from our initial requirements list!

### Any trying or heroic moments you would like to share? Of what are you particularly proud: the effort you put into it? the end product? the process you followed? what you learned along the way? Tell us about it!
 - Aditi: 
   - i2c!! This took a lot longer than expected to get running given that it was Julie's code. However, after spending quite a few hours under the logic analyzer and reading up on how the protocol worked, I could implement a checker to see whether appropriate packets were being ack'ed/nak'ed. I learned a lot in this process about the i2c protocol and ended up with a working version! Yay! 
   - multitasking the music!! I tried a few different methods of how to get music running in parallel with the game. The most straightforward one was to write a PWM module, but clearly I decided not to do that. (1) My first idea was to play the music during the 20% offtime in the game loop. It was very staccato and cute, and the tune was recognizable... but not enough. (2) My next idea was to (as often as possible) check the ticks and if modding it gave a certain range, then set the buzzer's gpio out to 0 or 1. Turned out that I was not updating the buzzer often enough, so I got a weird staticky/random sound, plus the staccato tones I had been getting in my first idea's implementation. (3) Timer interrupts galore! I used one timer interrupt to handle the notes' durations and another timer interrupt to handle the pitch of each note (because, I am physically updating the 0/1 I send to the buzzer) based on the note's frequency. This worked! It interrupts every 200-400us to keep the pitch of the note. Wonderful!
   - in case I haven't mentioned it: I love the remote
 - Anjali: [prollly wanna talk about the extern here lol TODO ]
 - Both:
   - the game is addictive! I'm not sure if we spent more time debugging or playing hahaha :)

## Photos
### Photos & writeups are in the /documents directory 
 - /documents/demo : contains video demo of Tiltris! includes endgame (blocks stack to signal end of game), leaderboard (entering user info; updated leaderboard), and starting a new game (tuck, clear line, swap)
 - /documents/remote : contains pictures of the remote (including earlier breadboard stages to show the transformation)
 - /documents/specs+initialnotes : contains a list of features we wanted initially + Aditi's initial HW notes
