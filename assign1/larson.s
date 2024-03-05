/* File: larson.s
 * --------------
 * Anjali Sreenivas 
 * CS107E | Assignment 1
 * 01/20/2024
 */

/* The below program is a larson scanner designed for 8 LEDs
 * configured on GPIO pins PB0-PB7. The light appears to travel back
 * and forth across the LEDs in an infinite loop, with a period of ~2
 * seconds (left to right and back again).
 * 
 * Additionally, the light travels with varying levels of brightness!! :)
 * In the moving chain of 5 lit LEDs (1 2 3 4 5 from left to right),
 * 3 is the brightest, 2 and 4 are second-brightest, and 1 and 5 are least bright.
 * Variation in brightness is modulated by frequency of blinking / how long the LED 
 * is on in the given loop iteration. 
 */

    lui     a0, 0x2000              # a0 holds base addr PB group = 0x2000000
    lui     a1, 0x11111
    addi    a1, a1, 0x111           # a1 holds 0x11111111 -- value to place in PB config reg 0
    sw      a1, 0x30(a0)            # config PB0 - PB7  as outputs via PB config reg 0

    # Initializing registers to store variable data config patterns used to control
    # LED on/off cycling in below loop code 
    addi    a7, zero, 0b11111       # "all-on" config 
    addi    a2, zero, 0b1110        # "semi-on" config 
    addi    a3, zero, 0b100         # "brightest only" config 
   
    # Storing constant values used in loop code to determine when to switch light movement direction 
    addi    a5, zero, 0b100000000            

    # Cycling which LED is brightest in ascending pin order (PB2 -> PB3 -> .. -> PB7) driven by left shifting
    move_left: 
    # "semi-on" config (brightest + adjacent 2 LEDs on)
    sw      a2, 0x40(a0)      # set data value of PB0 
    lui     a4, 0x400         # a4 = init countdown value for delay
    delay_1: 
    addi    a4, a4, -1        # decrement a4
    bne     a4, zero, delay_1  # keep counting down until a4 is zero

    # "brightest on" config - only brightest LED on 
    sw      a3, 0x40(a0)     
    lui     a4, 0x800
    delay_2: 
    addi    a4, a4, -1
    bne     a4, zero, delay_2
 
    # "all on" config - all 5 LEDs (brightest + 2 adj on each side) on
    sw      a7, 0x40(a0)
    lui     a4, 0x200
    delay_3:
    addi    a4, a4, -1
    bne     a4, zero, delay_3
    
    # left shift all config values ("all-on", "semi-on", "brightest only") by 1 to advance to next loop iteration
    slli    a7, a7, 1
    slli    a2, a2, 1      
    slli    a3, a3, 1 
  
    # if "semi-on" (a3) value is not the constant in a5, jump back to move_left to continue left movement
    bne     a3, a5, move_left

    # Start rightward motion once left motion is finished (a3 == a5)...
    # Right shift all config values by 2 places -- encodes for configs where PB6 is connected to central LED,
    # since left motion loop ends with PB7 as brightest LED
    srli     a2, a2, 2       
    srli     a3, a3, 2         
    srli     a7, a7, 2
   
    # Cycling which LED is brightest in descending pin order (PB6 -> PB5 -> .. -> PB0) driven by right shifting
    move_right:
    # "semi-on" config (brightest + adj 2 LEDs)
    sw      a2, 0x40(a0)      
    lui     a4, 0x400        
    delay_1r:
    addi    a4, a4, -1      
    bne     a4, zero, delay_1r 

    # "brightest only" config (only brightest LED on)
    sw      a3, 0x40(a0)     
    lui     a4, 0x800
    delay_2r:
    addi    a4, a4, -1
    bne     a4, zero, delay_2r
 
    # "all-on" config (brighted + 2 adj LEDs on both sides on)
    sw      a7, 0x40(a0)
    lui     a4, 0x200
    delay_3r:
    addi    a4, a4, -1
    bne     a4, zero, delay_3r
    
    # right shift all config values by 1 to set up for next loop iteration 
    srli    a7, a7, 1
    srli    a2, a2, 1        
    srli    a3, a3, 1 
  
    # if "semi-on" (a3) value is not 0, jump back to move_right to continue rightward movement w/ new data config values
    bne     a3, zero, move_right

    # else begin left movement again...
    # special case needed for where PB1 is connected to the brightest LED (since its "all-on" config only has four 1s) 
    addi    a7, zero, 0b1111        # "all-on" config 
    addi    a2, zero, 0b111         # "semi-on" config 
    addi    a3, zero, 0b10          # "brightest only" config 
    # "semi-on" config (brightest + adjacent 2 LEDs on)
    sw      a2, 0x40(a0)      # set data value of PB0 to a2
    lui     a4, 0x400         # a4 = init countdown value for delay
    delay_1_PB1: 
    addi    a4, a4, -1             # decrement a3
    bne     a4, zero, delay_1_PB1  # keep counting down until a3 is zero

    # "brightest on" config - only brightest LED on (PB1) 
    sw      a3, 0x40(a0)     
    lui     a4, 0x800
    delay_2_PB1: 
    addi    a4, a4, -1
    bne     a4, zero, delay_2_PB1
 
    # "all on" config - all 5 LEDs (brightest + 2 adj on each side) on
    sw      a7, 0x40(a0)
    lui     a4, 0x200
    delay_3_PB1:
    addi    a4, a4, -1
    bne     a4, zero, delay_3_PB1  

    # reset config values and begin left movement loop again (starts with PB2 as brightest LED)
    addi    a7, zero, 0b11111       # "all-on" config 
    addi    a2, zero, 0b1110        # "semi-on" config 
    addi    a3, zero, 0b100         # "brightest only" config 
   
    j move_left 
