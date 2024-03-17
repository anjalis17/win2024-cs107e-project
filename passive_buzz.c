/* passive_buzz.c
 * Module to play buzzer tones to buzzer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * Note that, if you're using this module, 100% of the mango pi's attention is towards the song. 
 *  to multitask, check out passive_buzz_intr.h/.c for interrupt-based control of pitch
 */

#include "gpio.h"
#include "timer.h"
#include "printf.h"
#include "passive_buzz.h"
#include "interrupts.h"
#include "hstimer.h"
#include "music.h"

#define TICKS_PER_USEC 24 // 24 ticks counted per one microsecond
#define uSEC_IN_SEC 1000000

static gpio_id_t buzzer_id ;
static int tempo ; // in beats per minute

// 'buzzer_init'
// initializes buzzer
void buzzer_init(gpio_id_t id) {
    gpio_set_output(id) ;
    buzzer_id = id ;
    tempo = TEMPO_DEFAULT;
}


// 'buzzer_set_tempo'
// set the tempo for the buzzer
// returns false if invalid tempo
// TODO test tempo ranges!! > andante doesn't work
bool buzzer_set_tempo(int new_tempo) {
    if (new_tempo > 50 && new_tempo < 200) {
        tempo = new_tempo ;
        return true ;
    }
    return false ;
}

// 'buzzer_get_tempo'
// returns current tempo
int buzzer_get_tempo(void) {
    return tempo ;
}

// 'buzzer_play_note'
// plays a note on the buzzer
// code citation: https://blog.tarkalabs.com/digital-audio-101-playing-audio-from-a-microcontroller-5df1463616c
// "The formula for calculating period is period (in sec) = 1/frequency (in Hz)."
void buzzer_play_note(int frequency, int duration_msec) {
    printf("\nfreq: %d\n", frequency) ;

    duration_msec = duration_msec * 60 / tempo ; // updates speed of song based on tempo

    unsigned long busy_wait_until = (timer_get_ticks() + (duration_msec*1000*TICKS_PER_USEC))  ; // turn into milliseconds
    int note_frequency = frequency ; 
    int note_period = (uSEC_IN_SEC / note_frequency) ; // keep it approximate

    while (timer_get_ticks() < busy_wait_until) {
        double total_duration = 0 ;
        while(total_duration <= (uSEC_IN_SEC/4)) {
            total_duration += note_period; 
            gpio_write(buzzer_id, 1);
            timer_delay_us(note_period/2);
            gpio_write(buzzer_id, 0);
            timer_delay_us(note_period/2);
        }
    }
}

