#include "../include/inc.h"




// Структура узла дерева
typedef struct TreeNode 
{
    char *name;

    int is_dir; 
    int is_exec; 
    // Дети и их количество
    struct TreeNode **children;
    size_t child_count;
} TreeNode;

// Функция создания нового узла
static TreeNode* create_node(const char *name)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (!node) return NULL;

    node->name = strdup(name ? name : "");
    node->is_dir = 0;
    node->is_exec = 0;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

// Функция для получения полного пути к файлу
static char* build_full_path(char **segments, size_t count)
{
    // Пример: ["test_tree","dir2","readme.txt"] -> "/test_tree/dir2/readme.txt"

    if (count == 0) 
    {
        return strdup("");
    }


    size_t total_len = 0;
    for (size_t i = 0; i < count; i++) 
    {
        total_len += strlen(segments[i]);
    }
    total_len += (count - 1); // под разделители
    total_len += 1; // нулевой байт

    char *full_path = malloc(total_len);
    if (!full_path) return NULL;
    full_path[0] = '\0';

    for (size_t i = 0; i < count; i++) 
    {
        strcat(full_path, segments[i]);
        if (i < count - 1) 
        {
            strcat(full_path, "/");
        }
    }
    return full_path;
}

// Поиск (или создание) дочернего узла по имени
static TreeNode* find_or_create_child(TreeNode *parent, const char *child_name)
{
    // Сначала ищем
    for (size_t i = 0; i < parent->child_count; i++) 
    {
        if (strcmp(parent->children[i]->name, child_name) == 0) 
        {
            return parent->children[i];
        }
    }
    // Не найден: создаём
    TreeNode *new_child = create_node(child_name);
    if (!new_child)
    { 
        return NULL;
    }
    // Расширим массив children
    TreeNode **temp = realloc(parent->children, (parent->child_count + 1) * sizeof(TreeNode *));
    if (!temp) 
    {
        // Если не удалось реаллок, почистим new_child
        free(new_child->name);
        free(new_child);
        return NULL;
    }
    parent->children = temp;
    parent->children[parent->child_count] = new_child;
    parent->child_count++;

    return new_child;
}

// Добавить путь (разбитый на сегменты) в дерево
static void insert_path_into_tree(TreeNode *root, char **segments, size_t count)
{
    TreeNode *current = root;
    for (size_t i = 0; i < count; i++) 
    {
        if (!segments[i] || strcmp(segments[i], "") == 0) 
        {
            continue; // пропускаем пустые сегменты 
        }
        // Вставляем/ищем ребёнка
        current = find_or_create_child(current, segments[i]);
        // на последнем шаге считаем, что это файл, на промежуточных - каталог
        if (i < count - 1) 
        {
            current->is_dir = 1; 
            current->is_exec = 0;
        } else 
        {
            // предположим, что на последнем уровне у нас файл
            
            current->is_dir = 0;

            char *full_path = build_full_path(segments, count);

            if (full_path)
            {
                struct stat st;

                if (stat(full_path, &st) == 0)
                {
                    if (st.st_mode & S_IXUSR || 
                        st.st_mode & S_IXGRP || 
                        st.st_mode & S_IXOTH)
                    {
                        current->is_exec = 1;
                    }
                }
                else
                {
                    // Попытаемся добавить '/' в начало, если там его нет
                    if (full_path[0] != '/') 
                    {
                        size_t len = strlen(full_path);
                        char *alt_path = malloc(len + 2); // +1 для '/', +1 для '\0'
                        if (alt_path)
                        {
                            alt_path[0] = '/';
                            strcpy(alt_path + 1, full_path);

                            // Пробуем снова stat
                            if (stat(alt_path, &st) == 0)
                            {
                                if ((st.st_mode & S_IXUSR) ||
                                    (st.st_mode & S_IXGRP) ||
                                    (st.st_mode & S_IXOTH))
                                {
                                    current->is_exec = 1;
                                }
                            }   
                            else
                            {
                                current->is_exec = 0;
                            }
                            free(alt_path);
                        }
                    }
                    else
                    {
                        // Если и так уже начиналось с '/', то тут делать нечего
                        current->is_exec = 0;
                    }
                }
                free(full_path);   
            }
        }
        if (!current) 
        {
            // Ошибка - нет памяти, просто прервём
            return;
        }
    }
}

// Разбиваем строку пути на сегменты (пример: "./test_tree/dir1/file.txt" => ["test_tree","dir1","file.txt"])
static char** split_path(const char *full_path, size_t *out_count)
{
    // Скопируем строку, чтобы strtok её мог резать
    char *temp = strdup(full_path);
    if (!temp) 
    {
        *out_count = 0;
        return NULL;
    }



    // Разделитель в Unix - '/'
    // Будем резать strtok, собирая сегменты в динамический массив
    size_t capacity = 4;
    size_t count = 0;
    char **parts = malloc(capacity * sizeof(char*));
    if (!parts) 
    {
        free(temp);
        *out_count = 0;
        return NULL;
    }

    char *tok = strtok(temp, "/");
    while (tok) 
    {
        if (count >= capacity) 
        {
            capacity *= 2;
            char **newparts = realloc(parts, capacity * sizeof(char*));
            if (!newparts) 
            {
                // Ошибка памяти
                free(parts);
                free(temp);
                *out_count = 0;
                return NULL;
            }
            parts = newparts;
        }
        parts[count++] = strdup(tok);
        tok = strtok(NULL, "/");
    }

    // Обрежем лишнее
    char **final_arr = realloc(parts, count * sizeof(char*));
    if (final_arr) 
    {
        parts = final_arr;
    }
    *out_count = count;
    // temp больше не нужен
    free(temp);

    return parts;
}

// Функция рекурсивного вывода дерева (ASCII-графика)
static void print_tree_recursive(const TreeNode *node, const char *prefix, int is_last)
{
    // Если это «корневой» (пустой) узел, мы его не печатаем
    if (node->name && node->name[0] != '\0') 
    {
        // Определяем, какой использовать символ └── или ├──
        if (node->is_dir) //каталог - голубой
        {
            printf("%s%s\x1b[36m%s/\x1b[0m\n",
                    prefix,
                    (is_last ? "└── " : "├── "),
                    node->name
            );   
        }
        else
        {
            if (node->is_exec) // исполняемый - красный
            {
                printf("%s%s\x1b[31m%s\x1b[0m\n",
                        prefix,
                        (is_last ? "└── " : "├── "),
                        node->name
                );
            }
            else // белый
            {
                printf("%s%s%s\n",
                    prefix,
                    (is_last ? "└── " : "├── "),
                    node->name
                );
            }


        }
    }

    // Для детей создадим префикс
    // Если текущий узел не последний, то вместо "   " будет "│  "
    char new_prefix[1024]; // на моем эеране помещается не более 170 символов в длинну
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, (is_last ? "    " : "│   "));

    for (size_t i = 0; i < node->child_count; i++) {
        // Определяем, последний ли это ребёнок
        int child_is_last = ((i + 1) == node->child_count);
        print_tree_recursive(node->children[i], new_prefix, child_is_last);
    }
}

// Функция рекурсивного освобождения (из рабства)
static void free_tree(TreeNode *node)
{
    if (!node) return;
    // Освобождаем детей
    for (size_t i = 0; i < node->child_count; i++) 
    {
        free_tree(node->children[i]);
    }
    // Освобождаем массив детей
    free(node->children);
    // Освобождаем имя узла
    free(node->name);
    // И сам узел
    free(node);
}

// Собственно print_maches, формирующий дерево и выводящий его
void print_maches(void)
{
    if (global_maches_len == 0) 
    {
        printf("Совпадений не найдено.\n");
        return;
    }

    // Создаём корневой "пустой" узел, чтобы все пути были его дочерними
    TreeNode *root = create_node("");
    if (!root) 
    {
        fprintf(stderr, "Ошибка: не удалось создать корневой узел.\n");
        return;
    }

    // Пройдёмся по всем путям из global_maches и вставим их в дерево
    for (size_t i = 0; i < global_maches_len; i++) 
    {
        // Разбиваем путь на сегменты
        size_t segment_count = 0;
        char **segments = split_path(global_maches[i], &segment_count);
        if (!segments) 
        {
            // Ошибка при split_path
            continue;
        }
        // Вставляем в дерево
        insert_path_into_tree(root, segments, segment_count);

        // Освободим segments
        for (size_t j = 0; j < segment_count; j++) 
        {
            free(segments[j]);
        }
        free(segments);
    }

    // Теперь у нас есть дерево. Выводим его рекурсивно
    
    for (size_t i = 0; i < root->child_count; i++) {
        int is_last = ((i + 1) == root->child_count);
        print_tree_recursive(root->children[i], "", is_last);
    }

    // Освобождаем дерево
    free_tree(root);
}