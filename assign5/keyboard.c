/* File: keyboard.c
 * -----------------
 * Author: Anjali Sreenivas
 *
 * The keyboard.c file defines functions that support reading keys typed 
 * on a PS/2 keyboard, acting as a keyboard driver.
 * 
 * Citation: I was perplexed for way longer than I should have been as to 
 * why hitting backspace was giving me [08] when running test_keyboard_asserts,
 * and I eventually reached out to Daniel who told me to look at the test code,
 * helping me realize that my output is indeed correct functionality given the structure
 * of the test case. Ahhh! :)
 */
#include "keyboard.h"
#include "ps2.h"
#include "ps2_keys.h"
#include "printf.h"

static ps2_device_t *dev;
static keyboard_modifiers_t modifiers;

void keyboard_init(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    dev = ps2_new(clock_gpio, data_gpio);
}

unsigned char keyboard_read_scancode(void) {
    return ps2_read(dev);
}

key_action_t keyboard_read_sequence(void) {
    key_action_t action;

    unsigned char scancode = keyboard_read_scancode();
    // extended key
    if (scancode == PS2_CODE_EXTENDED) {
        unsigned char scancode2 = keyboard_read_scancode();
        // extended key press
        if (scancode2 != PS2_CODE_RELEASE) {
            action.what = KEY_PRESS;
            action.keycode = scancode2;
        }
        // extended key release
        else {
            action.what = KEY_RELEASE;
            action.keycode = keyboard_read_scancode();
        }
    }
    // ordinary key release
    else if (scancode == PS2_CODE_RELEASE) {
        action.what = KEY_RELEASE;
        action.keycode = keyboard_read_scancode();
    }
    // ordinary key press
    else {
        action.what = KEY_PRESS;
        action.keycode = scancode;
    }

    return action;
}

static void update_modifiers(key_action_t action, keyboard_modifiers_t modifier) {
    // special handling of caps lock ("sticky" key) -- toggle existing state
    if (modifier == KEYBOARD_MOD_CAPS_LOCK) {
        // action must be key press for modifer to be updated
        if (action.what != KEY_PRESS) return;
        // if caps lock on, turn off
        if ((modifiers & modifier) == modifier) modifiers &= ~modifier;
        // otherwise, turn on
        else modifiers |= modifier;
    }
    // all other modifiers
    else {
        // modifier key pressed (turn on)
        if (action.what == KEY_PRESS) modifiers |= modifier;
        // modifier key released (turn off)
        else modifiers &= ~modifier;
    }
}

key_event_t keyboard_read_event(void) {
    key_event_t event;
    while (1) {
        key_action_t action = keyboard_read_sequence();
        // find key corresponding to action
        ps2_key_t key = ps2_keys[action.keycode];

        // handle cases where ch is a modifier
        if (key.ch == PS2_KEY_SHIFT) update_modifiers(action, KEYBOARD_MOD_SHIFT);
        else if (key.ch == PS2_KEY_ALT) update_modifiers(action, KEYBOARD_MOD_ALT);
        else if (key.ch == PS2_KEY_CTRL) update_modifiers(action, KEYBOARD_MOD_CTRL);
        else if (key.ch == PS2_KEY_CAPS_LOCK) update_modifiers(action, KEYBOARD_MOD_CAPS_LOCK); 
        // key event is for non-modifier key
        else {
            event.action = action;
            event.key = key;
            event.modifiers = modifiers;
            break;
        }
    }
    return event;
}

unsigned char keyboard_read_next(void) {
    key_event_t event = keyboard_read_event();
    // only key presses processed, not releases
    while (event.action.what != KEY_PRESS) {
        event = keyboard_read_event();
    }
    unsigned char ch = event.key.ch; // default return value
    // check if shift modifier active
    if ((modifiers & KEYBOARD_MOD_SHIFT) == KEYBOARD_MOD_SHIFT) return event.key.other_ch;
    // if not, check caps lock -- only applies to alphabetic keys
    if ((modifiers & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK) {
        if (ch >= 'a' && ch <= 'z') return event.key.other_ch;
    }
    return ch;
}
