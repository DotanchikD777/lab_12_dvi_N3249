#ifndef INC_H
#define INC_H

//Libraries

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>

//defines
#define A_USED 0
#define O_USED 1
#define N_USED 2
#define P_USED 3
#define H_USED 4
#define V_USED 5

//Globals

extern int opt_used_counter[6];

extern bool environment_debug_var;

//Functions

void opt_errors();

void get_debug_status_mode_error_handlers_functions(bool flag);



#endif