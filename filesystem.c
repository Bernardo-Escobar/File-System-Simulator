#include "filesystem.h"

BTree* btree_create() {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = NULL;
    return tree;
}

TreeNode* create_txt_file(const char* name, const char* content) {
    File* file = malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = strlen(content);

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = FILE_TYPE;
    node->data.file = file;
    return node;
}

TreeNode* create_directory(const char* name) {
    Directory* dir = malloc(sizeof(Directory));
    dir->tree = btree_create();

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = DIRECTORY_TYPE;
    node->data.directory = dir;
    return node;
}

void delete_txt_file(BTree* tree, const char* name) {
    printf("Arquivo '%s' deletado (simulado)\n", name);
}

void delete_directory(BTree* tree, const char* name) {
    printf("Diretório '%s' deletado (simulado)\n", name);
}

Directory* get_root_directory() {
    Directory* root = malloc(sizeof(Directory));
    root->tree = btree_create();
    return root;
}

void change_directory(Directory** current, const char* path) {
    printf("Mudando para o diretório: %s (simulado)\n", path);
}

void list_directory_contents(Directory* dir) {
    printf("Conteúdo do diretório:\n");
    btree_traverse(dir->tree);
}

TreeNode* btree_search(BTree* tree, const char* name) {
    printf("Buscando: %s (simulado)\n", name);
    return NULL;
}

/*---------------------------------------- INSERT ----------------------------------------*/

// void btree_insert(BTree* tree, TreeNode* node) {
//     printf("Inserindo: %s (simulado)\n", node->name);
// }

void btree_split_child(BTreeNode* parent, int i, BTreeNode* full_child) {
    int t = MIN_DEGREE;

    BTreeNode* new_child = malloc(sizeof(BTreeNode));
    new_child->leaf = full_child->leaf;
    new_child->num_keys = t - 1;

    // Copia a segunda metade das chaves para o novo nó
    for (int j = 0; j < t - 1; j++) {
        new_child->keys[j] = full_child->keys[j + t];
    }

    // Copia os filhos se não for folha
    if (!full_child->leaf) {
        for (int j = 0; j < t; j++) {
            new_child->children[j] = full_child->children[j + t];
        }
    }

    full_child->num_keys = t - 1;

    // Move os filhos do pai para abrir espaço para o novo filho
    for (int j = parent->num_keys; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = new_child;

    // Move as chaves do pai para abrir espaço para a chave do meio
    for (int j = parent->num_keys - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = full_child->keys[t - 1];
    parent->num_keys++;
}

void btree_insert_nonfull(BTreeNode* r, TreeNode* node) {
    int i = r->num_keys - 1;

    if (r->leaf) {
        // Move as chaves para abrir espaço
        while (i >= 0 && strcmp(node->name, r->keys[i]->name) < 0) {
            r->keys[i + 1] = r->keys[i];
            i--;
        }

        r->keys[i + 1] = node;
        r->num_keys++;
    } else {
        // Encontra o filho que deve receber a nova chave
        while (i >= 0 && strcmp(node->name, r->keys[i]->name) < 0) {
            i--;
        }
        i++;

        // Se o filho está cheio, divida
        if (r->children[i]->num_keys == 2 * MIN_DEGREE - 1) {
            btree_split_child(r, i, r->children[i]);
            if (strcmp(node->name, r->keys[i]->name) > 0) {
                i++;
            }
        }
        btree_insert_nonfull(r->children[i], node);
    }
}

void btree_insert(BTree* tree, TreeNode* node) {
    if (tree->root == NULL) {
        // Cria raiz
        tree->root = malloc(sizeof(BTreeNode));
        tree->root->leaf = 1;
        tree->root->num_keys = 1;
        tree->root->keys[0] = node;
        return;
    }

    BTreeNode* r = tree->root;

    if (r->num_keys == 2 * MIN_DEGREE - 1) {
        // Raiz cheia, precisa dividir
        BTreeNode* s = malloc(sizeof(BTreeNode));
        tree->root = s;
        s->leaf = 0;
        s->num_keys = 0;
        s->children[0] = r;
        btree_split_child(s, 0, r);
        btree_insert_nonfull(s, node);
    } else {
        btree_insert_nonfull(r, node);
    }
}

/*----------------------------------------------------------------------------------------*/

void btree_delete(BTree* tree, const char* name) {
    printf("Removendo: %s (simulado)\n", name);
}

/*---------------------------------------- TRAVERSE ----------------------------------------*/

// void btree_traverse(BTree* tree) {
//     printf("[Exemplo] arquivo.txt\n");
// }

void btree_traverse_node(BTreeNode* node) {
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (!node->leaf) {
            btree_traverse_node(node->children[i]);
        }
        printf("- %s\n", node->keys[i]->name);
    }
    if (!node->leaf) {
        btree_traverse_node(node->children[i]);
    }
}

void btree_traverse(BTree* tree) {
    if (tree->root != NULL) {
        btree_traverse_node(tree->root);
    }
}
