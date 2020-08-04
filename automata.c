/*This file models cellular automata and draws the patterns formed from advancing
each generation to the next and understanding which cells die and which are alive.
The dead cells are represented by empty strings and the live cells are represented by
inverted box.
*/

#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define INVERTED_BOX "\e[7m \e[0m"

/* uncomment below to swap in "+" as definition of LIVE_STR if your
 * terminal doesn't play nice with inverted box
 */
#define LIVE_STR INVERTED_BOX
//#define LIVE_STR "+"
#define EMPTY_STR " "
#define DEFAULT_GEN_0 (1L << 32)
#define N_GENERATIONS 32

/*This function takes as input an unsigned long representing the current generation
and an unsigned char for the ruleset to base the automata model off of. The function returns
the next generation as a form of an unsigned long and utilizes bitmasks, shift oerators, and 
other bitwise operators to craft this new generation.
 */
unsigned long advance(unsigned long cur_gen, unsigned char ruleset) {

    unsigned long new_gen = 0;
    unsigned long mask = 1;
    unsigned long mask_newgen = 1;
    
    unsigned long right_end = cur_gen << 1;
    unsigned long neighborhood = right_end & 0x7;

    //Operation with default zero neighbor to last bit on right
    mask = mask << (neighborhood);
  
    if (ruleset & mask){
        new_gen = new_gen | mask_newgen;
    }
    mask_newgen = mask_newgen << 1;
    
    //Operation for all non-edge bits
    for (int i=1; i <= 64; i ++){
       mask = 1;
       neighborhood = cur_gen & 0x7;
       mask = mask << (neighborhood);
       
       if (ruleset & mask){
           new_gen = new_gen | (mask_newgen);
       }
       mask_newgen = mask_newgen << 1;
       cur_gen  = cur_gen >> 1;
    }
    
    
    
    return new_gen;
}

/*This function takes in as input an unsigned long indicative of the generation 
to draw for. It doesn't return any value; however, it prints out the entire 
sequence of empty strings or live strings representing each of the 64 cells
 */
void draw_generation(unsigned long gen) {
    // each cell in gen is drawn as LIVE_STR or EMPTY_STR depending on whether cell is on or off
    for (int i = 1; i <= 64; i ++){
        unsigned long mask = 1L << (64-i);
        if ((gen & mask) == 0){
            printf(EMPTY_STR);
        }
        else{
            printf(LIVE_STR);
        }
    }
    printf("\n");
    
}


/*This function takes as input a char * representing the ruleset or initial generation, 
along wih the minimum and maximum values for an unsigned char (for ruleset) or long (for
initial generation), and the argument name. This function checks for errors if an invalid number
or an out-of-range number is given. If no errors persist, then the unsigned version of the ruleset
or the initial generation is returned.
 */
unsigned long convert_arg(const char *str, unsigned long low, unsigned long high, const char *argname) {
    char *end;
    unsigned long n = strtoul(str, &end, 0);
    if (*end != '\0') {
        error(1, 0, "Invalid number '%s' for %s", str, argname);
    }
    if (n < low || n > high || errno == ERANGE) {
        error(1, 0, "%s %s is not in range [%#lx, %#lx]", argname, str, low, high);
    }
    return n;
}

/*The function takes in the user's arguments as input and checks if there are any missing arguments. 
If not, the generations are drawn abiding the ruleset up to the number of generations defined a the top of this
file. 
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        error(1, 0, "Missing argument. Please supply ruleset and optional initial generation.");
    }

    unsigned long gen_0 = DEFAULT_GEN_0;
    unsigned char ruleset = convert_arg(argv[1], 0, UCHAR_MAX, "ruleset");
    if (argc > 2) {
        gen_0 = convert_arg(argv[2], 0, ULONG_MAX, "initial generation");
    }

    if (ruleset == 0) {
        draw_generation(gen_0);
    } else {
        unsigned long cur_gen = gen_0;
        for (int i = 0; i < N_GENERATIONS; i++) {
            draw_generation(cur_gen);
            unsigned long next_gen = advance(cur_gen, ruleset);
            if (next_gen == cur_gen) {
                break;
            }
            cur_gen = next_gen;
        }
    }
    return 0;
}
