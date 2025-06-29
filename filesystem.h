#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN_DEGREE 2 // Grau mínimo (pode ser ajustado)
// ordem = quantidade máxima de ponteiros/filhos por nó
// ordem - 1 = quantidade de chaves
// ordem/2 = quantidade mínima de filhos
// grau mínimo = mínimo de chaves
// grau máximo = máximo de chaves (gera split)
// ex.: min_degree = 2 --> max_degree = 3

//Enum para distinguir entre arquivos e diretórios
typedef enum { FILE_TYPE, DIRECTORY_TYPE } NodeType;

typedef struct File {
    char* name;
    char* content;
    size_t size;
} File;

struct Directory;

// Nó da árvore que pode ser um arquivo ou diretório
typedef struct TreeNode {
    char* name;
    NodeType type;
    union {
        File* file;
        struct Directory* directory;
    } data;
} TreeNode;

// Nó da árvore B
typedef struct BTreeNode {
    int num_keys;
    TreeNode* keys[2 * MIN_DEGREE - 1];
    struct BTreeNode* children[2 * MIN_DEGREE];
    int leaf;
} BTreeNode;

// Árvore B
typedef struct BTree {
    BTreeNode* root;
} BTree;

typedef struct Directory {
    BTree* tree;
} Directory;

// BTree Functions
BTree* btree_create();
void btree_insert(BTree* tree, TreeNode* node);
void btree_delete(BTree* tree, const char* name); //Implementar
TreeNode* btree_search(BTree* tree, const char* name); //Implementar
void btree_traverse(BTree* tree);

// File/Directory creation
TreeNode* create_txt_file(const char* name, const char* content);
TreeNode* create_directory(const char* name);
void delete_txt_file(BTree* tree, const char* name); //Implementar
void delete_directory(BTree* tree, const char* name); //Implementar

// Navigation
Directory* get_root_directory();
void change_directory(Directory** current, const char* path); //Implementar
void list_directory_contents(Directory* dir);

// Img
void save_filesystem(Directory* root);
void persist_node(FILE* img, BTreeNode* node, const char* prefix, int is_last);

#endif
