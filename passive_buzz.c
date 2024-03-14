/* passive_buzz.c
 * Module to play buzzer tones to buzzer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include "timer.h"
#include "printf.h"
#include "passive_buzz.h"

#define TICKS_PER_USEC 24 // 24 ticks counted per one microsecond
#define SEC_IN_uSEC 1000000

static gpio_id_t buzzer_id ;
static int tempo ; // in beats per minute

// 'buzzer_init'
// initializes buzzer
void buzzer_init(gpio_id_t id) {
    gpio_set_output(id) ;
    buzzer_id = id ;
    tempo = TEMPO_DEFAULT;
}

// todo aditi! keep it static for now while I figure it out
static void buzzer_init_pwm(gpio_id_t id) {

    // pg 1086: Table 9-21 PB Multiplex Function    
    //      PB6, function 5 : PWM1
    gpio_set_function(GPIO_PB6, GPIO_FN_ALT5) ;

    buzzer_id = id ;
    tempo = TEMPO_DEFAULT;

/*
aditi's pwm notes
https://cs107e.github.io/readings/d1-h_user_manual_v1.0.pdf 
PB6 = pwm 1
pg 107 - 3.2.6.54 0x07AC PWM Bus Gating Reset Register (Default Value: 0x0000_0000) 
 -> what is bus gating reset
pg 217 - 3.8.2 - functional description of interrupts
interrupt no. 34) PWM  = interrupt vector 0x0088

pg 1085 -- should I wire pwr to 3.3v instead of 5v like I have it rn??


pg 1241
The PWM has the following features:
 Supports 8 independent PWM channels (PWM0 to PWM7)
    - Supports PWM continuous mode output
    - Supports PWM pulse mode output, and the pulse number is configurable
    - Output frequency range: 0 to 24 MHz or 100 MHz
    - Various duty-cycle: 0% to 100%
    - Minimum resolution: 1/65536
*has nice diagrams pg1243

pg1245
Active state of PWM0 channel is high level (PCR0. PWM_ACT_STA = 1)
    When PCNTR0 > (PPR0. PWM_ENTIRE_CYCLE - PPR0.PWM_ACT_CYCLE), then PWM0 outputs 1 (high level).
    When PCNTR0 <= (PPR0. PWM_ENTIRE_CYCLE - PPR0.PWM_ACT_CYCLE), then PWM0 outputs 0 (low level).
The formula of the output period and the duty-cycle for PWM are as follows.
    Tperiod = (PWM01_CLK / PWM0_PRESCALE_K)-1 * (PPR0.PWM_ENTIRE_CYCLE + 1)
    Thigh-level = (PWM01_CLK / PWM0_PRESCALE_K)-1 * PPR0.PWM_ACT_CYCLE
    Tlow-level = (PWM01_CLK / PWM0_PRESCALE_K)-1 * (PPR0.PWM_ENTIRE_CYCLE + 1 - PPR0.PWM_ACT_CYCLE)
    Duty-cycle = (high level time) / (1 period time) = Thigh-level / Tperiod

pg 1247
PCR0 = the PWM_MODE = set to 0 for cycle mode; 1 for pulse mode

pg 1251 - pwm interrupt
9.11.3.11 Interrupt - The PIS bit is set to 1 automatically by hardware and cleared by software.

!!!! STEP BY STEP INSTRUCTIONS FOR CONFIG!
pg 1252 -- follow instructions on configuring clock and pwm
 -> copy over the gpio.h and edit there?

pg 1253 - all addresses of pwm stuff:
     PWM 0x02000C00

pg 1297 - reading this register could give the pwm's pulse count...?
9.11.6.23 0x010C + N*0x20 PWM Pulse Counter Register (Default Value: 0x0000_0000)


cycle mode == for buzzer

*/

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
    int note_period = (SEC_IN_uSEC / note_frequency) ; // keep it approximate

    while (timer_get_ticks() < busy_wait_until) {
        double total_duration = 0 ;
        while(total_duration <= (SEC_IN_uSEC/4)) {
            total_duration += note_period; 
            gpio_write(buzzer_id, 1);
            timer_delay_us(note_period/2);
            gpio_write(buzzer_id, 0);
            timer_delay_us(note_period/2);
        }
    }
}




// new, hands-off way to do pwm :) thanks Javier for the suggestion 
// todo test

// todo create a struct for this stuff:
static int start_ticks ;
static int note_period ;

// init for every single note change!!
void buzzer_freq_init(int start_ticks_, int note_period_) {
    start_ticks = start_ticks_ ; // the duration of time for an eighth notw to play, in ms
    note_period = note_period_ ;
}

// for client:
// input: int note_period = (SEC_IN_uSEC / music_notes[music_index]) ; 
//  aka   int note_period = (SEC_IN_uSEC / NOTE_FREQ) ;  
void buzzer_timing_play_note(void) {
    // check timer
    // if % frequency >/< 1/2 frequency, turn on/off for that note
    if ((timer_get_ticks() - start_ticks) % (note_period) < (note_period / 2)) {
        gpio_write(buzzer_id, 1);
    } else {
        gpio_write(buzzer_id, 0);
    }
}

