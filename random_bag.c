/* game_update.c
* -----------------------------------
* Author: Anjali Sreenivas (anjalisr)
* 
* FILL IN FILE HEADER
*/

#include "random_bag.h"
#define BAG_SIZE 28

static struct {
    int random_bag[BAG_SIZE];
    int size;
} rand_bag;

void random_bag_init(void) {
    for (int i = 0; i < BAG_SIZE; i++) {
        rand_bag.random_bag[i] = i % 7;
    }
    rand_bag.size = BAG_SIZE;
}

bool random_bag_isEmpty(void) {
    return (rand_bag.size == 0);
}

int random_bag_choose(void) {
    if (random_bag_isEmpty()) {
        random_bag_init();      // replenish random bag by resetting state
    }
    int randInd = timer_get_ticks() % rand_bag.size;
    int chosen = rand_bag.random_bag[randInd];
    if (randInd != (rand_bag.size - 1)) {
        rand_bag.random_bag[randInd] = rand_bag.random_bag[rand_bag.size - 1];
    }
    rand_bag.size--;
    return chosen;
}   