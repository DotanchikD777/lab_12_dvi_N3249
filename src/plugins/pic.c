//
// Created by denis on 15.05.2025.
//
#include "../include/plugin_api.h"
#include "../include/inc.h"


static char *d_lib_name = "libpic.so";

static char *d_plugin_purpose = "Check if the file contains an image";

static char *d_plugin_author = "Ilchuk Denis";

#define OPT_FORMAT_STR "pic"

#define PNG 0
#define JPG 1
#define BMP 2
#define GIF 3

static struct plugin_option pl_opt_arr[] = {
        {
                {
                    OPT_FORMAT_STR,
                    required_argument,
                    0, 0,
                },
                "specify formats to find"
        },
};

static int pl_opt_arr_len = sizeof(pl_opt_arr)/sizeof(pl_opt_arr[0]);



// Парсит строку str, возвращает указатель на массив из 4 флагов.
// В порядке: [png, jpg, bmp, gif].
// При ошибке (пустой ввод, неизвестный токен, дубль, слишком много/мало) — все флаги = -1.
int *parse_extensions(const char *str) {
    if (str == NULL) return NULL;

    // Массив флагов инициализируем нулями
    int *flags = malloc(4 * sizeof(int));
    if (!flags) return NULL;
    for (int i = 0; i < 4; i++) flags[i] = 0;

    // Копируем строку во временный буфер, т.к. strtok его изменяет
    char *buf = strdup(str);
    if (!buf) {
        free(flags);
        return NULL;
    }

    int count = 0;
    char *token = strtok(buf, ",");
    while (token) {
        int idx = -1;
        if (strcmp(token, "png") == 0) idx = PNG;
        else if (strcmp(token, "jpg") == 0) idx = JPG;
        else if (strcmp(token, "bmp") == 0) idx = BMP;
        else if (strcmp(token, "gif") == 0) idx = GIF;

        // если токен не из ожидаемого списка или дубль — ошибка
        if (idx < 0 || flags[idx] == 1) {
            // помечаем ошибку
            flags[0] = -1;
            free(buf);
            return flags;
        }

        flags[idx] = 1;
        count++;
        token = strtok(NULL, ",");
    }

    free(buf);

    // проверяем общее количество найденных расширений
    if (count < 1 || count > 4) {
        flags[0] = -1;
    }

    return flags;
}


int
plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: in plugin %s: invalid argument\n", d_lib_name);
        return -1;
    }

    ppi->plugin_purpose = d_plugin_purpose;
    ppi->plugin_author = d_plugin_author;
    ppi->sup_opts_len = pl_opt_arr_len;
    ppi->sup_opts = pl_opt_arr;

    return 0;
}

int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {


    bool DEBUG = getenv("LAB12DEBUG") != NULL;


    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }

    if (DEBUG) {
        printf("\nDEBUG: %s: Processing file %s\n", d_lib_name, fname);
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s'\n",
                    d_lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }

    int *flags = parse_extensions((const char*)in_opts[in_opts_len].flag);
    if (!flags || (flags[0] == -1)) {
        if(DEBUG)
            printf("DEBUG: %s: bad argument: %s", d_lib_name, (char*)in_opts[in_opts_len].flag);
        free(flags);
        return -1;
    }


    unsigned char hdr[8];
    FILE *f = fopen(fname, "rb");
    if (!f) {
        if(DEBUG)
            printf("DEBUG: %s: cant open file: %s", d_lib_name, fname);
        free(flags);
        return -1;
    }

    size_t n = fread(hdr, 1, sizeof(hdr), f);
    fclose(f);
    if (n < 2) {
        // слишком мало данных для определения
        return -1;
    }

    // 0: PNG — первые 8 байт = 89 50 4E 47 0D 0A 1A 0A
    if (flags[PNG]) {
        const unsigned char png_sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
        if (n >= 8 && memcmp(hdr, png_sig, 8) == 0){
            free(flags);
        return 0;
        }
    }

    // 1: JPG — первые два байта 0xFF 0xD8
    if (flags[JPG]) {
        if (hdr[0] == 0xFF && hdr[1] == 0xD8) {
            free(flags);
            return 0;
        }
    }

    // 2: BMP — первые два байта 'B' 'M'
    if (flags[BMP]) {
        if (hdr[0] == 'B' && hdr[1] == 'M'){
            free(flags);
            return 0;
        }
    }

    // 3: GIF — первые шесть байт "GIF87a" или "GIF89a"
    if (flags[GIF]) {
        if (n >= 6 && (memcmp(hdr, "GIF87a", 6) == 0 || memcmp(hdr, "GIF89a", 6) == 0)){
            free(flags);
            return 0;
        }
    }

    free(flags);
    return 1;
}