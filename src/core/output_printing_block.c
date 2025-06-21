#include "../include/inc.h"


// Структура узла дерева
typedef struct TreeNode // node stuct
{
    char *name;
    int is_dir;
    int is_exec;
    struct TreeNode **children; // children
    size_t child_count;
} TreeNode;

static TreeNode *create_node(const char *name) { // create new node
    TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));
    if (!node) return NULL;

    node->name = strdup(name ? name : "");
    node->is_dir = 0;
    node->is_exec = 0;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

static char *build_full_path(char **segments, size_t count) { // build full path (can`t use sb struct)
    // Example: ["test_tree","dir2","readme.txt"] -> "/test_tree/dir2/readme.txt"
    if (count == 0)
        return strdup("");


    size_t total_len = 0;
    for (size_t i = 0; i < count; i++)
        total_len += strlen(segments[i]);
    total_len += (count - 1); // tabs
    total_len += 1; // \0

    char *full_path = malloc(total_len);
    if (!full_path) return NULL;
    full_path[0] = '\0';

    for (size_t i = 0; i < count; i++) {
        strcat(full_path, segments[i]);
        if (i < count - 1)
            strcat(full_path, "/");
    }
    return full_path;
}

static TreeNode *find_or_create_child(TreeNode *parent, const char *child_name) { // add node to tree

    for (size_t i = 0; i < parent->child_count; i++)
        if (strcmp(parent->children[i]->name, child_name) == 0)
            return parent->children[i];

    TreeNode *new_child = create_node(child_name); // if cant find create child
    if (!new_child)
        return NULL;

    TreeNode **temp = realloc(parent->children, (parent->child_count + 1) * sizeof(TreeNode *));
    if (!temp) {
        free(new_child->name);
        free(new_child);
        return NULL;
    }
    parent->children = temp;
    parent->children[parent->child_count] = new_child;
    parent->child_count++;

    return new_child;
}

static void insert_path_into_tree(TreeNode *root, char **segments, size_t count) {
    TreeNode *current = root;
    for (size_t i = 0; i < count; i++) {
        if (!segments[i] || strcmp(segments[i], "") == 0)
            continue;

        current = find_or_create_child(current, segments[i]);
        if (i < count - 1) {
            current->is_dir = 1;
            current->is_exec = 0;
        } else {
            current->is_dir = 0;

            char *full_path = build_full_path(segments, count);

            if (full_path) {
                struct stat st;

                if (stat(full_path, &st) == 0) {
                    if (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
                        current->is_exec = 1;
                } else {
                    if (full_path[0] != '/') { // add / to beginning to make absolute path
                        size_t len = strlen(full_path);
                        char *alt_path = malloc(len + 2);
                        if (alt_path) {
                            alt_path[0] = '/';
                            strcpy(alt_path + 1, full_path);

                            // Пробуем снова stat
                            if (stat(alt_path, &st) == 0) {
                                if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
                                    current->is_exec = 1;
                            } else
                                current->is_exec = 0;
                            free(alt_path);
                        }
                    } else
                        current->is_exec = 0;
                }
                free(full_path);
            }
        }
        if (!current)
            return;
    }
}

static char **split_path(const char *full_path, size_t *out_count) {
    char *temp = strdup(full_path);
    if (!temp) {
        *out_count = 0;
        return NULL;
    }

    size_t capacity = 4;
    size_t count = 0;
    char **parts = malloc(capacity * sizeof(char *));
    if (!parts) {
        free(temp);
        *out_count = 0;
        return NULL;
    }

    char *tok = strtok(temp, "/");
    while (tok) {
        if (count >= capacity) {
            capacity *= 2;
            char **newparts = realloc(parts, capacity * sizeof(char *));
            if (!newparts) {
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

    char **final_arr = realloc(parts, count * sizeof(char *));
    if (final_arr)
        parts = final_arr;
    *out_count = count;
    free(temp);

    return parts;
}

static void print_tree_recursive(const TreeNode *node, const char *prefix, int is_last) {
    if (node->name && node->name[0] != '\0') {
        if (node->is_dir) { // blue - dir
            printf("%s%s\x1b[36m%s/\x1b[0m\n",
                   prefix,
                   (is_last ? "└── " : "├── "),
                   node->name
            );
        } else {
            if (node->is_exec) { // red - exec
                printf("%s%s\x1b[31m%s\x1b[0m\n",
                       prefix,
                       (is_last ? "└── " : "├── "),
                       node->name
                );
            } else { // white - file
                printf("%s%s%s\n",
                       prefix,
                       (is_last ? "└── " : "├── "),
                       node->name
                );
            }
        }
    }

    char new_prefix[1024];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, (is_last ? "    " : "│   "));

    for (size_t i = 0; i < node->child_count; i++) {
        // Определяем, последний ли это ребёнок
        int child_is_last = ((i + 1) == node->child_count);
        print_tree_recursive(node->children[i], new_prefix, child_is_last);
    }
}

static void free_tree(TreeNode *node) {
    if (!node) return;
    for (size_t i = 0; i < node->child_count; i++)
        free_tree(node->children[i]);
    free(node->children);
    free(node->name);
    free(node);
}

void print_matches(void) {
    if (global_matches_len == 0) {
        printf("\n%s\n\tNo files are matching conditions\n%s\n", STRIPE_SMALL, STRIPE_SMALL);
        return;
    }

    TreeNode *root = create_node("");
    if (!root)
        print_error_message("Can`t create node");

    for (size_t i = 0; i < global_matches_len; i++) {
        size_t segment_count = 0;
        char **segments = split_path(global_matches[i], &segment_count);
        if (!segments)
            continue;

        insert_path_into_tree(root, segments, segment_count);

        for (size_t j = 0; j < segment_count; j++)
            free(segments[j]);

        free(segments);
    }

    if (root->child_count == 1
        && strcmp(root->children[0]->name, ".") == 0) {

        TreeNode *dot = root->children[0];
        printf("./\n");
        for (size_t i = 0; i < dot->child_count; i++) {
            int is_last = (i + 1 == dot->child_count);
            print_tree_recursive(dot->children[i], "", is_last);
        }
    } else {
        for (size_t i = 0; i < root->child_count; i++) {
            int is_last = ((i + 1) == root->child_count);
            print_tree_recursive(root->children[i], "", is_last);
        }
    }

    free_tree(root);
}