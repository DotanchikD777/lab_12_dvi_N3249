#ifndef INC_H
#define INC_H

//Libraries

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>

//defines
#define A_USED 2-2
#define O_USED 3-2
#define N_USED 4-2
#define P_USED 5-2
#define H_USED 6-2
#define V_USED 7-2

//Globals

extern int opt_used_counter[6];

extern bool environment_debug_var;

//Functions

void opt_errors();

void get_debug_status_mode_error_handlers_functions(bool flag);



#endif