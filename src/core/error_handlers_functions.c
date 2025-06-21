#include "../include/inc.h"




static bool DEBUG;
int opt_used_counter[6];

void get_debug_status_mode_error_handlers(bool flag){
    if(flag){
        DEBUG = true;
    }
}

void opt_errors(void){
    for (int i = 0; i < 6; i++) //too much usage of options
        if(opt_used_counter[i] > 1){
            print_error_message("option abuse");
            exit(EXIT_FAILURE);
        }

    if(opt_used_counter[A_USED] && opt_used_counter[O_USED])
        punish_dummy_user("A and O option usage");

    if(DEBUG)
        printf("\nDEBUG: Options passed without errors\n");
}

void punish_dummy_user(const char *err_msg){
    const char *taunts[] = {
            "Oh, brilliant. Did you really think that would work?",
            "Thanks for demonstrating what \033[1;31mNOT\033[0m to do.",
            "Maybe try reading a manual next time."
    };
    size_t n = sizeof(taunts) / sizeof(taunts[0]);

    fprintf(stderr, "\033[1;31mError: %s\033[0m\n", err_msg);

    for (size_t i = 0; i < n; i++) {
        printf("\a");                // звуковой сигнал
        fprintf(stderr, "%s\n", taunts[i]);
        sleep(3);                    // пауза 1 секунда
    }

    fprintf(stderr, "\n\033[1;31mTURN OFF THE PC!\033[0m It will be better for everyone.\n%s\n", STRIPE);
    exit(EXIT_FAILURE);
}

void print_error_message(const char *err_msg){
    fprintf(stderr, "\n%s\nError: %s\n%s\n", STRIPE_SMALL, err_msg, STRIPE);
    exit(EXIT_FAILURE);
}