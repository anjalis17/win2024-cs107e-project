#ifndef _TESTING_H
#define _TESTING_H

// void pause(const char *message);
void test_random_init(void);
void test_basic_block_motion(void);
void test_motions(void);
int get_keystroke(const char *message);
void test_remote(void) ;
void test_motions_integrated(void) ; // keyboard
void tetris_theme_song(void) ;
void integration_test_v2(void); // with remote
void test_leaderboard(void) ;
void integration_test_v3(void) ; // with interlude
void integration_test_v4(void) ; // chromatic scale
void integration_test_v5(void) ; // tetris theme staccato
void integration_test_v6(void) ; // testing with tuck, polishing game loop
// void integration_test_v7(void) ; // tetris theme multithreading
void integration_test_v8(void) ; // tetris theme intrp with blinking screen
void integration_test_v9(void) ; // tetris theme intrp with game

#endif