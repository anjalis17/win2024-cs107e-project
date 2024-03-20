## Project title
### Tiltris

## Team members
Anjali Sreenivas (anjalisr), Aditi Bhaskar (aditijb)

## Project description
Tetris, but with a tiltable controller!

## Features
### 3D printed remote (remote, servo, lsd6ds33, i2c):
 - novel design (Aditi created the CAD using OnShape; conceptually based on a Wii remote) 
   - ergonomic shape
   - compact design 
   - button clicks well!!!
   - clay-based mango-shaped cushion remote handle (removable)
   - I totally made all the measurements for my CAD with a bendy ruler :)
 - game controls (accelerometer-driven, button-driven, servo for haptic feedback):
   - tilt left or right to control horizontal movement of falling piece
   - tilt down to drop piece faster
   - tilt up to to swap piece with next piece in queue
   - button (using queued interrupts on button press) to turn game piece clockwise
### Tetris game! (game_update, random_bag):
 - Start screen (with 107E mango drawing from Tetris pieces!)
 - Lateral motion of piece (left/right)
 - Piece falling / downward motion
 - Collision ("fallen") detection -- between piece and top of stack (or ground) and lateral pieces
 - Graphics!
   - Distinction in appearance between "falling" and "fallen" pieces
   - Optimizations in gl to make console clearing and square drawing super fast (helped us minimize lag and enabled us to do a full redraw with each update)
 - Creation and use of a random bag to generate next falling piece (adding some control/regularity to randomness :))
 - Queue -- display of next piece (top right square)
 - Swap (ability to swap with next piece in queue)
 - Scoring: display and algorithm to reward more lines cleared simultaneously
 - Codebase architecture that easily supported adding new features!! :)
 We are proud to have implemented all the big Tetris features! YAY!
### leaderboard (game_interlude):
 - user-friendly design to add players to leaderboard:
   - use of button to iterate/select on-screen and tilt down to continue to next screen
### music (passive_buzz_intr, music):
 - Plays the song! This means MULTITASKING! YAY! 
 - Libraries allow user to initialize/change tempo, and play a song on repeat using interrupts! Very friendly interface for people who know western classical music
 - Notes that compose a song are specified by a (1) frequency (note letter) and (2) duration (whole/half/quarter/eighth)
 - passive_buzz files are from a previous version of the passive buzzer implementation: they play notes directly instead of controlling the PWM via interrupts
 - Ability to pause/resume music on start screen for the upcoming game. This means that (if the beautiful music is giving you a headache) you can configure on/off for music at the beginning of each game using button presses.

## Member contribution
 - Aditi did everything on the hardware-side and created the leaderboard!
   - files: remote, servo, lsd6ds33, i2c, passive_buzz, passive_buzz_intr, game_interlude, testing/main 
   - written with future users in mind! thoroughly commented
 - Anjali built all the software underlying the game itself! 
   - files: game_update, random_bag, testing/main
 - Together, we did lots of fine-tuning, debugging, play testing, and calibrating of remote angling. We also decided on "new"/next features to add, and perhaps most importantly, had lots of fun and discovered new things together :)

## References
 - all references are also inline in our code, but here is a short summary:
   - i2c and lsd6ds33 libraries were borrowed and modified from Julie's week 8 Sensors lecture. The code is commented to deliniate what is borrowed vs. new
   - Talked to Javier, Antonio briefly about multitasking and where the timer interrupt clear functions were
 - https://codeincomplete.com/articles/javascript-tetris/ -- Our hex representation of the rotational configurations for each tetris piece was inspired from this site. This structure facilitated a super efficient way to iterate over the squares of each tetris piece using bit shifting! (The rest of the Javascript code on this site was not used, consulted, or referenced.)

## Sharing Code
Our code for i2c.c (a slighly more robust version which checked for outgoing addresses to be ACK'ed) was shared with Sazzad and Aditri's teams. They both said they would cite us. Thought it was worth mentioning here for good measure.

## Self-evaluation
### How well was your team able to execute on the plan in your proposal?  
Our team executed our goals super well! We are really proud of our end-product, the fetaures we were able to pull together, our integration, and how well we worked together! <3 We think that our planning in advance, our well-broken-down schedule, our outlining of manageable steps, and our constant checking in with each other helped a lot. We managed to execute all our P1 tasks within the first week, and continued adding features both from our initial P2/P3 specs (eg. music, swap) but also additional features that we thought would be cool/useful (eg. leaderboard). We ended up accomplishing all the tasks we wanted to accomplish, and then some more, from our initial requirements list!

### Any trying or heroic moments you would like to share? Of what are you particularly proud: the effort you put into it? the end product? the process you followed? what you learned along the way? Tell us about it!
 - Aditi: 
   - i2c!! This took a lot longer than expected to get running given that it was Julie's code. However, after spending quite a few hours under the logic analyzer and reading up on how the protocol worked, I could implement a checker to see whether appropriate packets were being ack'ed/nak'ed. I learned a lot in this process about the i2c protocol and ended up with a working version! Yay! 
   - multitasking the music!! I tried a few different methods of how to get music running in parallel with the game. The most straightforward one was to write a PWM module, but clearly I decided not to do that. (1) My first idea was to play the music during the 20% offtime in the game loop. It was very staccato and cute, and the tune was recognizable... but not enough. (2) My next idea was to (as often as possible) check the ticks and if modding it gave a certain range, then set the buzzer's gpio out to 0 or 1. Turned out that I was not updating the buzzer often enough, so I got a weird staticky/random sound, plus the staccato tones I had been getting in my first idea's implementation. (3) Timer interrupts galore! I used one timer interrupt to handle the notes' durations and another timer interrupt to handle the pitch of each note (because, I am physically updating the 0/1 I send to the buzzer) based on the note's frequency. This worked! It interrupts every 200-400us to keep the pitch of the note. Wonderful!
   - in case I haven't mentioned it: I love the remote
 - Anjali: 
   - the codebase architecture of the game!! From the structuring of the tetris game pieces, the use of function pointers in being able to iterate through game pieces with a variety of functions, and the decomposition scheme, I think the design is efficient and easy to work with :) It made adding features super easy! 
     - "Anjali's architecture is super modular and easy to work with" ~ Aditi, when she changed the blocks to be beveled on the spot during lab :O
   - TUCK! (This is the feature where the user can slide an immediately fallen piece left/right before it locks in place... an advanced Tetris tactic! ;)) This tuck feature was tricky to implement, but I was able to integrate it quite smoothly (also thanks to my architecture; I'm thankful I spent time drawing it out / thinking about it in the beginning!). It adds so much to the game, we think!
   - It took quite an adventure to figure out that in C header files, when declaring const variables, they must be declared using extern... otherwise, we get "multiple definition" errors from the linker :O
   - I'm proud of both mine and Aditi's effort, how we worked together/divided up the project, our integration, and our enthusiasm to keep adding new features and touches and push ourselves! <3 I'm also super happy seeing our end product! YAY!
 - Both:
   - the game is addictive! I'm not sure if we spent more time debugging or playing hahaha :)

## Photos
### Photos & writeups are in the /documents directory 
 - /documents/demo : contains video demos of Tiltris! 
   - cs107e-tiltris-demo.mp4 features Aditi playing Tiltris. It includes endgame (blocks stack to signal end of game), leaderboard (entering user info; updated leaderboard), and starting a new game (tuck, clear line, swap). 
   - cs107e-tiltris-demo-intermediary-screens.mp4 features Anjali playing Tiltris. It includes more footage of swap, the *newly* added start screen, toggling on/off music, and more!
   - cs107e-tiltris-danielshighscore features high score on Tiltris is 92 lines, by Daniel
   - cs107e-start-screen features a recent (but not final) version of the start screen
 - /documents/remote : contains pictures of the remote (including earlier breadboard stages to show the transformation)
 - /documents/specs+initialnotes : contains a list of features we wanted initially in *Tetris.pdf + Aditi's initial hardware notes in *HW_notes.pdf
