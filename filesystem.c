#include "filesystem.h"
#include <stdbool.h>

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


Directory* get_root_directory() {
    Directory* root = malloc(sizeof(Directory));
    root->tree = btree_create();
    return root;
}


void change_directory(Directory** current, const char* path) {
    TreeNode* target_node = btree_search((*current)->tree, path);
    if (target_node && target_node->type == DIRECTORY_TYPE) {
        *current = target_node->data.directory;
    } else {
        printf("cd: Diretorio nao encontrado: %s\n", path);
    }
}

void list_directory_contents(Directory* dir) {
    printf("Conteúdo do diretório:\n");
    btree_traverse(dir->tree);
}

TreeNode* btree_search_recursive(BTreeNode* node, const char* name) {
    if (!node) return NULL;
    int i = 0;
    while (i < node->num_keys && strcmp(name, node->keys[i]->name) > 0) i++;
    if (i < node->num_keys && strcmp(name, node->keys[i]->name) == 0) return node->keys[i];
    if (node->leaf) return NULL;
    return btree_search_recursive(node->children[i], name);
}

TreeNode* btree_search(BTree* tree, const char* name) {
    if (!tree || !tree->root) {
        return NULL;
    }
    return btree_search_recursive(tree->root, name);
}

/*---------------------------------------- INSERT ----------------------------------------*/


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
        while (i >= 0 && strcmp(node->name, r->keys[i]->name) < 0) { //dif
            r->keys[i + 1] = r->keys[i];
            i--;
        }

        r->keys[i + 1] = node;
        r->num_keys++;
    } else {
        // Encontra o filho que deve receber a nova chave
        while (i >= 0 && strcmp(node->name, r->keys[i]->name) < 0) { //dif
            i--;
        }
        i++;

        // Se o filho está cheio, divida
        if (r->children[i]->num_keys == 2 * MIN_DEGREE - 1) {
            btree_split_child(r, i, r->children[i]);
            if (strcmp(node->name, r->keys[i]->name) > 0) { //dif
                i++;
            }
        }
        btree_insert_nonfull(r->children[i], node);
    }
}

void btree_insert(BTree* tree, TreeNode* node) {
    // Cria raiz se não existe
    if (tree->root == NULL) {
        tree->root = malloc(sizeof(BTreeNode));
        tree->root->leaf = 1;
        tree->root->num_keys = 1;
        tree->root->keys[0] = node;
        return;
    }

    BTreeNode* r = tree->root;

    if (r->num_keys == 2 * MIN_DEGREE - 1) {
        // Raiz cheia, precisa dividir
        // Cria novo nó
        BTreeNode* s = malloc(sizeof(BTreeNode));
        tree->root = s; // raiz = s
        s->leaf = 0;
        s->num_keys = 0; //dif
        s->children[0] = r;
        btree_split_child(s, 0, r);
        btree_insert_nonfull(s, node);
    } else {
        btree_insert_nonfull(r, node);
    }
}

/*---------------------------------------- DELETE ----------------------------------------*/

int findKey(BTreeNode* node, const char* name) {
    int idx = 0;
    while (idx < node->num_keys && strcmp(node->keys[idx]->name, name) < 0) ++idx;
    return idx;
}

void removeFromLeaf(BTreeNode* node, int idx) {
    for (int i = idx + 1; i < node->num_keys; ++i) node->keys[i - 1] = node->keys[i];
    node->num_keys--;
}

TreeNode* getPred(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx];
    while (!cur->leaf) cur = cur->children[cur->num_keys];
    return cur->keys[cur->num_keys - 1];
}

TreeNode* getSucc(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx + 1];
    while (!cur->leaf) cur = cur->children[0];
    return cur->keys[0];
}

// Declaração antecipada
void btree_delete_internal(BTreeNode* node, const char* name);

void fill(BTreeNode* node, int idx, const char* name) {
    if (idx != 0 && node->children[idx - 1]->num_keys >= MIN_DEGREE) {
        BTreeNode* child = node->children[idx];
        BTreeNode* sibling = node->children[idx - 1];
        for (int i = child->num_keys - 1; i >= 0; --i) child->keys[i + 1] = child->keys[i];
        if (!child->leaf) {
            for (int i = child->num_keys; i >= 0; --i) child->children[i + 1] = child->children[i];
        }
        child->keys[0] = node->keys[idx - 1];
        if (!child->leaf) child->children[0] = sibling->children[sibling->num_keys];
        node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];
        child->num_keys += 1;
        sibling->num_keys -= 1;
    } else if (idx != node->num_keys && node->children[idx + 1]->num_keys >= MIN_DEGREE) {
        BTreeNode* child = node->children[idx];
        BTreeNode* sibling = node->children[idx + 1];
        child->keys[child->num_keys] = node->keys[idx];
        if (!child->leaf) child->children[child->num_keys + 1] = sibling->children[0];
        node->keys[idx] = sibling->keys[0];
        for (int i = 1; i < sibling->num_keys; ++i) sibling->keys[i - 1] = sibling->keys[i];
        if (!sibling->leaf) {
            for (int i = 1; i <= sibling->num_keys; ++i) sibling->children[i - 1] = sibling->children[i];
        }
        child->num_keys += 1;
        sibling->num_keys -= 1;
    } else {
        int merge_idx = (idx == node->num_keys) ? idx - 1 : idx;
        BTreeNode* child = node->children[merge_idx];
        BTreeNode* sibling = node->children[merge_idx + 1];
        child->keys[MIN_DEGREE - 1] = node->keys[merge_idx];
        for (int i = 0; i < sibling->num_keys; ++i) child->keys[i + MIN_DEGREE] = sibling->keys[i];
        if (!child->leaf) {
            for (int i = 0; i <= sibling->num_keys; ++i) child->children[i + MIN_DEGREE] = sibling->children[i];
        }
        for (int i = merge_idx + 1; i < node->num_keys; ++i) node->keys[i - 1] = node->keys[i];
        for (int i = merge_idx + 2; i <= node->num_keys; ++i) node->children[i - 1] = node->children[i];
        child->num_keys += sibling->num_keys + 1;
        node->num_keys--;
        free(sibling);
        btree_delete_internal(child, name);
        return;
    }
}

//Declaração antecipada
void removeFromNonLeaf(BTreeNode* node, int idx);

void btree_delete_internal(BTreeNode* node, const char* name) {
    int idx = findKey(node, name);
    if (idx < node->num_keys && strcmp(node->keys[idx]->name, name) == 0) {
        if (node->leaf) removeFromLeaf(node, idx);
        else removeFromNonLeaf(node, idx);
    } else {
        if (node->leaf) { return; }
        bool flag = (idx == node->num_keys);
        if (node->children[idx]->num_keys < MIN_DEGREE) {
            fill(node, idx, name);
        }
        if (flag && idx > node->num_keys) btree_delete_internal(node->children[idx - 1], name);
        else btree_delete_internal(node->children[idx], name);
    }
}

void removeFromNonLeaf(BTreeNode* node, int idx) {
    TreeNode* k = node->keys[idx];
    if (node->children[idx]->num_keys >= MIN_DEGREE) {
        TreeNode* pred = getPred(node, idx);
        node->keys[idx] = pred;
        btree_delete_internal(node->children[idx], pred->name);
    } else if (node->children[idx + 1]->num_keys >= MIN_DEGREE) {
        TreeNode* succ = getSucc(node, idx);
        node->keys[idx] = succ;
        btree_delete_internal(node->children[idx + 1], succ->name);
    } else {
        BTreeNode* child = node->children[idx];
        BTreeNode* sibling = node->children[idx + 1];
        child->keys[MIN_DEGREE - 1] = k;
        for (int i = 0; i < sibling->num_keys; ++i) child->keys[i + MIN_DEGREE] = sibling->keys[i];
        if (!child->leaf) {
            for (int i = 0; i <= sibling->num_keys; ++i) child->children[i + MIN_DEGREE] = sibling->children[i];
        }
        for (int i = idx + 1; i < node->num_keys; ++i) node->keys[i - 1] = node->keys[i];
        for (int i = idx + 2; i <= node->num_keys; ++i) node->children[i - 1] = node->children[i];
        node->num_keys--;
        child->num_keys += sibling->num_keys + 1;
        free(sibling);
        btree_delete_internal(child, k->name);
    }
}



void btree_delete(BTree* tree, const char* name) {
    if (!tree->root) return;
    btree_delete_internal(tree->root, name);
    if (tree->root->num_keys == 0) {
        BTreeNode* tmp = tree->root;
        if (tree->root->leaf) { 
            // Ela é o único nó da árvore
            tree->root = NULL;
        }
        else {
            tree->root = tree->root->children[0];
            free(tmp);
        }
    }
}

void delete_txt_file(BTree* tree, const char* name) {
    btree_delete(tree, name);
}

void delete_directory(BTree* tree, const char* name) {
    btree_delete(tree, name);
}


/*---------------------------------------- TRAVERSE ----------------------------------------*/

void btree_traverse_node(BTreeNode* node) {
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (!node->leaf) {
            btree_traverse_node(node->children[i]);
        }
        printf("%s%s  \n", node->keys[i]->name, node->keys[i]->type == DIRECTORY_TYPE ? "/" : "");
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

void persist_node(FILE* img, BTreeNode* node, const char* prefix, int is_last) {
    if (!node) return;

    for (int i = 0; i < node->num_keys; i++) {
        TreeNode* t = node->keys[i];

        // Define prefixo para a linha atual
        fprintf(img, "%s", prefix);
        if (i == node->num_keys - 1 && node->leaf)
            fprintf(img, "└── ");
        else
            fprintf(img, "├── ");

        // Escreve nome e conteúdo
        if (t->type == FILE_TYPE) {
            fprintf(img, "%s: %s\n", t->name, t->data.file->content);
        } else if (t->type == DIRECTORY_TYPE) {
            fprintf(img, "%s\n", t->name);

            // Prefixo para os filhos
            char new_prefix[512];
            snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, (i == node->num_keys - 1 && node->leaf) ? "    " : "│   ");

            if (t->data.directory->tree && t->data.directory->tree->root) {
                persist_node(img, t->data.directory->tree->root, new_prefix, 1);
            }
        }

        // Se não é folha, percorre os filhos
        if (!node->leaf && node->children[i]) {
            persist_node(img, node->children[i], prefix, 0);
        }
    }

    // Último filho, se houver
    if (!node->leaf && node->children[node->num_keys]) {
        persist_node(img, node->children[node->num_keys], prefix, 1);
    }
}
void save_filesystem(Directory* root) {
    FILE* img = fopen("fs.img", "w");
    if (!img) {
        perror("Erro ao criar fs.img");
        return;
    }

    fprintf(img, "ROOT\n");
    if (root->tree && root->tree->root) {
        persist_node(img, root->tree->root, "", 1);
    }

    fclose(img);
    printf("\nSistema de arquivos salvo em fs.img\n");
}
