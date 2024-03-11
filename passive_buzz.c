/* passive_buzz.c
 * Module to play buzzer tones to buzzer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include "timer.h"
#include "printf.h"

#define TICKS_PER_USEC 24 // 24 ticks counted per one microsecond
#define SEC_IN_uSEC 1000000

static gpio_id_t buzzer_id ;

// 'buzzer_init'
// initializes buzzer
void buzzer_init(gpio_id_t id) {
    gpio_set_output(id) ;
    buzzer_id = id ;
}

// 'buzzer_play_note'
// plays a note on the buzzer
// code citation: https://blog.tarkalabs.com/digital-audio-101-playing-audio-from-a-microcontroller-5df1463616c
// "The formula for calculating period is period (in sec) = 1/frequency (in Hz)."
void buzzer_play_note(int frequency, int duration_msec) { // todo finish writing

    // todo incorporate duration

    printf("\nfreq: %d\n", frequency) ;

    unsigned long busy_wait_until = (timer_get_ticks() + (duration_msec*1000*TICKS_PER_USEC))  ; // turn into milliseconds
    int note_frequency = frequency ; // todo let this make use of params
    int note_period = (SEC_IN_uSEC / note_frequency) ; // keep it approximate

    while (timer_get_ticks() < busy_wait_until) {
        double total_duration = 0 ;
        while(total_duration <= (SEC_IN_uSEC/4)) { // todo incorporate the duration from params!!
            total_duration += note_period; 
            gpio_write(buzzer_id, 1);
            timer_delay_us(note_period/2);
            gpio_write(buzzer_id, 0);
            timer_delay_us(note_period/2);
        }
    }
}
