#include "testing.h"

#include "game_update.h"
#include "uart.h"
#include "assert.h"
#include "timer.h"
#include "printf.h"

#include "gpio.h"
#include "servo.h"
#include "remote.h"
#include "LSD6DS33.h"
#include "i2c.h"
#include "passive_buzz.h"

void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}

int get_keystroke(const char *message) {
    if (message) printf("\n%s\n", message);
    // printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
    return ch;
}

void test_random_init(void) {
    for (int i = 0; i < 10; i++) {
        timer_init();
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();
        pause("start");
    }
}

void test_basic_block_motion(void) {
    timer_init();
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();
    while (1) {
        pause("key press to move down");
        move_down(&piece);
        if (piece.fallen) {
            printf("fallen; new piece spawning");
            piece = init_falling_piece();
        }
    }
}

void test_motions(void) {
    timer_init();
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();
    while (1) {
        int ch = get_keystroke("key press to move down (S) / left (A) / right (D) or rotate (R)");
        if (ch == 's') move_down(&piece);
        else if (ch == 'a') move_left(&piece);
        else if (ch == 'd') move_right(&piece);
        else if (ch == 'r') rotate(&piece);
        if (piece.fallen) {
            printf("fallen; new piece spawning");
            piece = init_falling_piece();
        }
    }
}

void test_remote(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0) ; 
    interrupts_global_enable() ; 
    printf("\nremote tests enabled\n") ;
    while(1) {
        // tilt blocks
        int tilt_status = get_tilt();
        printf("\n device.state %s\n", tilt_status==HOME?"home":(tilt_status==LEFT?"left":"right")) ;
        
        // drop blocks
        if (is_drop()) {printf("\n*** x dropped ***\n");}
    }
}

void test_motions_integrated(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0) ; 
    interrupts_global_enable() ;
    
    game_update_init(20, 10);


    falling_piece_t piece = init_falling_piece();

    printf("\nin test_motions_integrated; initialized\n") ;


    while(1) {
        // tilt blocks
        int tilt_status = get_tilt();
        printf("\n device.state %s\n", tilt_status==HOME?"home":(tilt_status==LEFT?"left":"right")) ;
        
        // todo fix the aclr drifting readings - impl recalibration at each reading?

        if (tilt_status == LEFT) move_left(&piece);
        else if (tilt_status == RIGHT) move_right(&piece);
        if (is_drop()) { // TODO add a drop version where it goes faster vs it just drops the full way
            printf("\n*** x dropped ***\n"); 
            while(!piece.fallen && is_drop()){ 
                move_down(&piece);
            }
        }
        // else if (tilt_status == LEFT) move_left(&piece);
        // else if (tilt_status == RIGHT) move_right(&piece);

        // else if () rotate(&piece); // TODO add to the button intrpt
        
        if (piece.fallen) {
            printf("fallen; new piece spawning");
            piece = init_falling_piece();
        }

        timer_delay_ms(500) ;
        move_down(&piece); // todo make time consistent per loop
    }
}
