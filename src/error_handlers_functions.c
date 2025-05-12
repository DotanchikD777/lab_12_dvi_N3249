#include "inc.h"




static bool DEBUG;
int opt_used_counter[6];

void get_debug_status_mode_error_handlers_functions(bool flag){
    if(flag){
        DEBUG = true;
    }
}


void opt_errors(){
    for (int i = 0; i < 6; i++){ //too much usage of options
        if(opt_used_counter[i] > 1){
            printf("\nError: option abuse\n");
            exit(EXIT_FAILURE);
        }
    }

    if(opt_used_counter[A_USED] && opt_used_counter[O_USED]){
        printf("\nError: A option and O option usage\n");
    }

    if(DEBUG){
        printf("\nDebug: Options passed without errors\n");
    }
}