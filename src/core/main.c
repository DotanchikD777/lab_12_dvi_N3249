#include "../include/inc.h"
#include "../include/plugin_api.h"



// var only in this file






int main(int argc, char *argv[]){
    int opt;
    int option_index = 0;
    
    bool A_flag = false;
    bool O_flag = false;
    bool N_flag = false;
    char *P_dir = ".";
    

    bool DEBUG = (getenv("LAB1DEBUG") != NULL);

    get_debug_status_mode_error_handlers_functions(DEBUG);

    const char *optstring = "P:AONvh";


    static struct option long_options[] = {
        {"P",   required_argument,  0,    'P'},
        {"A",   no_argument,        0,    'A'},
        {"O",   no_argument,        0,    'O'},
        {"N",   no_argument,        0,    'N'},
        {"help",   no_argument,     0,    'h'},
        {"version",   no_argument,  0,    'v'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1){

        switch (opt) {
            case 'A':
                A_flag = true;
                opt_used_counter[A_USED] += 1;
                break;
            case 'O':
                O_flag = true;
                opt_used_counter[O_USED] += 1;
                break;
            case 'N':
                N_flag = true;
                opt_used_counter[N_USED] += 1;
                break;
            case 'P':
                P_dir = optarg;
                opt_used_counter[P_USED] += 1;
                break;
            case 'h':
                print_standart_message('h');
                opt_used_counter[H_USED] += 1;
                break;
            case 'v':
                print_standart_message('v');
                opt_used_counter[V_USED] += 1;
                break;
            case '?':
                print_error_message("corrupted options");
                break;
            default:
                print_error_message("impossible error");
                break;
        }
        
        if (DEBUG){
            printf("Debug: opt='%c', A=%d, O=%d, N=%d, P=%s\n",
                    (char)opt, A_flag, O_flag, N_flag, 
                    P_dir ? P_dir : "(none)" );
        }
    }

    if(!opt_used_counter[O_USED] && !opt_used_counter[A_USED]){
        A_flag = true;
        if(DEBUG){
            printf("\nDebug: default case --> A=%d", A_flag);
        }
    }

    opt_errors();
// testing
    char buf[strlen(P_dir) + strlen(argv[optind]) + 1];
    strcpy(buf, P_dir);
    strcat(buf, argv[optind]);

    void *dynamic_lib = dlopen("./lib/libpic.so", RTLD_LAZY);
    if (!dynamic_lib){
        print_error_message("can`t open dynamic library");
    }

    int (*plug_func_plugin_get_info)(struct plugin_info* ppi);


    plug_func_plugin_get_info = dlsym(dynamic_lib, "plugin_get_info");
    if (!plug_func_plugin_get_info) {
        fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
        exit(1);
    }

    typedef int (*pgi_func_t)(struct plugin_info*);
    pgi_func_t pgi_func = (pgi_func_t)plug_func_plugin_get_info;

    struct plugin_info plug_info_st = {0};

    int ret = pgi_func(&plug_info_st);
    if (ret < 0) {
        fprintf(stderr, "ERROR: plugin_get_info() failed\n");
        exit(1);
    }

    printf("\n%s\n%s\n%zu\n", plug_info_st.plugin_purpose, plug_info_st.plugin_author, plug_info_st.sup_opts_len);













}