## Project title
Tiltris 

 TODO!!

## Team members
Anjali Sreenivas (anjalisr), Aditi Bhaskar (aditijb)

## Project description
Tetris, but with a tiltable controller

## Features
# remote (remote, servo, lsd6ds33, i2c):
 - novel design (by Aditi!): 
   - ergonomic shape and compact design. button clicks well!!!
 - game:
   - tilt left or right to control horizontal movement of falling piece
   - tilt down to drop piece faster
   - tilt up to to swap piece with next piece in queue
   - button (using queued interrupts on button press) to turn game piece clockwise
# leaderboard (game_interlude):
 - user-friendly design to add players to leaderboard:
   - use button to iterate/select on-screen and tilt down to continue to next screen
# game (game_update, random_bag):
 - // TODO ADD GAME FEATURES @anjali
# music (passive_buzz):
 - plays the song! music library allows user to initialize/reset tempo, and play notes for a duration (whole/half/quarter/eighth) with a frequency (note letter)
 - very friendly to people who know western classical music.


## Member contribution
TODO A short description of the work performed by each member of the team.
Aditi:
 - for the most part, hardware-side + leaderboard
 - files: remote, servo, lsd6ds33, i2c, passive_buzz, game_interlude 
Anjali:
 - for the most part, the game itself
 - files: game_update, random_bag
Together:
 - calibrating remote angles, deciding on "new"/next features to add, debugging
 - we mostly worked together because of the nature of our project (need remote and monitor) 

## References
 - all references are also inline in our code, but here is a short summary:
   - i2c and lsd6ds33 libraries were borrowed and modified from Julie's week 8 Sensors lecture.

Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work.

# Sharing Code
Our code for i2c.c (a slighly more robust version which checked for outgoing addresses to be ACK'ed) was shared with Sazzad and Aditri's teams. They both said they would cite me. Thought it was worth mentioning here for good measure.

## Self-evaluation
# How well was your team able to execute on the plan in your proposal?  
TODO
# Any trying or heroic moments you would like to share? Of what are you particularly proud: the effort you put into it? the end product? the process you followed? what you learned along the way? Tell us about it!
TODO

## Photos
You are encouraged to submit photos/videos of your project in action. 
Add the files and commit to your project repository to include along with your submission.

TODO