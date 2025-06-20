//
// Created by denis on 21.06.2025.
//
#include "../include/inc.h"


// Structure for tree node
typedef struct TreeNode {
    char *name;
    struct TreeNode **children;
    size_t child_count;
    size_t child_cap;
    bool is_file;
} TreeNode;

static TreeNode *create_node(const char *name, bool is_file){
    TreeNode *n = malloc(sizeof(TreeNode));
    if(!n) return NULL;
    n->name = name ? strdup(name) : NULL;
    n->children = NULL;
    n->child_count = 0;
    n->child_cap = 0;
    n->is_file = is_file;
    return n;
}

static TreeNode *find_child(TreeNode *parent, const char *name){
    for(size_t i = 0;i < parent->child_count; i++){
        if(strcmp(parent->children[i]->name, name)==0)
            return parent->children[i];
    }
    return NULL;
}

static TreeNode *add_child(TreeNode *parent, const char *name, bool is_file){
    TreeNode *child = find_child(parent, name);
    if(child) return child;
    if(parent->child_count==parent->child_cap){
        size_t newcap = parent->child_cap?parent->child_cap*2:4;
        TreeNode **tmp = realloc(parent->children, newcap*sizeof(TreeNode*));
        if(!tmp) return NULL;
        parent->children = tmp;
        parent->child_cap = newcap;
    }
    child = create_node(name, is_file);
    parent->children[parent->child_count++] = child;
    return child;
}

static void add_path(TreeNode *root, const char *path){
    char *buf = strdup(path);
    char *saveptr=NULL;
    char *token = strtok_r(buf, "/", &saveptr);
    TreeNode *cur = root;
    while(token){
        char *next = strtok_r(NULL, "/", &saveptr);
        cur = add_child(cur, token, next==NULL);
        token = next;
    }
    free(buf);
}

static void print_tree_rec(const TreeNode *node, const char *prefix, bool last){
    if(node->name){
        printf("%s%s%s\n", prefix, last?"└── ":"├── ", node->name);
        char new_prefix[1024];
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, last?"    ":"│   ");
        for(size_t i=0;i<node->child_count;i++)
            print_tree_rec(node->children[i], new_prefix, i==node->child_count-1);
    } else {
        for(size_t i=0;i<node->child_count;i++)
            print_tree_rec(node->children[i], prefix, i==node->child_count-1);
    }
}

static void free_tree(TreeNode *node){
    for(size_t i=0;i<node->child_count;i++)
        free_tree(node->children[i]);
    free(node->children);
    free(node->name);
    free(node);
}

void print_matches_tree(void){
    if(global_maches_len==0){
        printf("No matches found.\n");
        return;
    }
    TreeNode *root = create_node(NULL,false);
    if(!root) return;
    for(size_t i=0;i<global_maches_len;i++)
        add_path(root, global_maches[i]);
    print_tree_rec(root, "", true);
    free_tree(root);
}