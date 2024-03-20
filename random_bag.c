/* random_bag.c
* -----------------------------------
* Author: Anjali Sreenivas (anjalisr)
* 
* The random_bag.c module includes a set of functions to create and pull items from 
* a random bag! 
* Citation: Julie, for giving us this awesome question on our CS106B exam in Fall 2023,
* while inspired my O(1) logic here! :)
*/

#include "random_bag.h"
#include "timer.h"
#define BAG_SIZE 28
#define NUM_ELEMS 7

static struct {
    int random_bag[BAG_SIZE];
    int size;
} rand_bag;

// Required init
void random_bag_init(void) {
    for (int i = 0; i < BAG_SIZE; i++) {
        rand_bag.random_bag[i] = i % NUM_ELEMS;
    }
    rand_bag.size = BAG_SIZE;
}

bool random_bag_isEmpty(void) {
    return (rand_bag.size == 0);
}

// Returns randomly chosen element (as a number ranging from 0 to NUM_ELEMS) from random bag
// If bag is empty, the random bag is replenished before an element is chosen.
int random_bag_choose(void) {
    if (random_bag_isEmpty()) {
        random_bag_init();      // replenish random bag by resetting state
    }
    int randInd = timer_get_ticks() % rand_bag.size;
    int chosen = rand_bag.random_bag[randInd];
    // Shifting of last element in random_bag array to ensure all elements are colocated in array and performance is optimized!
    if (randInd != (rand_bag.size - 1)) {
        rand_bag.random_bag[randInd] = rand_bag.random_bag[rand_bag.size - 1];
    }
    rand_bag.size--;
    return chosen;
}   