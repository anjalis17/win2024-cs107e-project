Game Design Architecture -- lowest to highest module abstraction layers

1) fb - framebuffers
2) gl - drawing into framebuffers
3) draw_game - game screen / tetris piece drawing routines (like console)
4) game_update - math/logic to handle motion of game pieces 
5) game loop - main