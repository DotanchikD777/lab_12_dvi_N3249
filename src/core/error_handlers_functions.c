#include "../include/inc.h"




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
        punish_dummy_user("A and O option usage");
    }

    if(DEBUG){
        printf("\nDebug: Options passed without errors\n");
    }
}

void punish_dummy_user(const char *err_msg){
    const char *taunts[] = {
            "Oh, brilliant. Did you really think that would work?",
            "Thanks for demonstrating what \033[1;31mNOT\033[0m to do.",
            "Maybe try reading a manual next time."
    };
    size_t n = sizeof(taunts) / sizeof(taunts[0]);

    // Покрасим текст в красный (\033[1;31m … \033[0m)
    fprintf(stderr, "\033[1;31mError: %s\033[0m\n", err_msg);

    for (size_t i = 0; i < n; i++) {
        printf("\a");                // звуковой сигнал
        fprintf(stderr, "%s\n", taunts[i]);
        sleep(3);                    // пауза 1 секунда
    }

    // Финальное сообщение и выход
    fprintf(stderr, "\033[1;31mTURN OFF THE PC!\033[0m It will be better for everyone.\n");
    exit(EXIT_FAILURE);

}

void print_error_message(const char *err_msg){

    fprintf(stderr, "Error: %s\n", err_msg);

    exit(EXIT_FAILURE);

}