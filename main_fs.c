#include "filesystem.h"
#define MAX_COMMAND_LENGTH 1024
#define MAX_PATH_LENGTH 1024
#define MAX_STACK_SIZE 1024

// Pilhas
Directory* dir_stack[MAX_STACK_SIZE];
char path_name_stack[MAX_STACK_SIZE][MAX_PATH_LENGTH];
// stack_top: índice do topo da pilha
int stack_top = 0;

// path_stack: vetor de strings representando os nomes dos diretórios
void get_current_path_string(char path_stack[][MAX_PATH_LENGTH], int stack_size, char* path_out) {
    if (stack_size == 0 || stack_size == 1) {
        strcpy(path_out, "/");
        return;
    }

    path_out[0] = '\0'; // inicializa string
    for (int i = 1; i < stack_size; ++i) {
        strcat(path_out, "/");
        strcat(path_out, path_stack[i]);
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

int main() {
    // Criação do sistema de arquivos
    Directory* root = get_root_directory();
    Directory* current_dir = root;

    dir_stack[0] = current_dir;
    strcpy(path_name_stack[0], "");

    char command[MAX_COMMAND_LENGTH], arg1[MAX_PATH_LENGTH];

    printf("Digite 'ajuda' para ver os comandos.\n");

    while(1){

        char path_display[MAX_PATH_LENGTH];
        get_current_path_string(path_name_stack, stack_top + 1, path_display);  // +1 porque stack_top é o índice
        printf("user@machine:%s$ ", path_display);

        scanf("%s", command);

        if (strcmp(command, "exit") == 0) break;

        if (strcmp(command, "ls") == 0) {
            list_directory_contents(current_dir);
        } else if (strcmp(command, "cd") == 0) {
            scanf("%s", arg1);
            if (strcmp(arg1, "..") == 0) {
                if (stack_top > 0) {
                    stack_top--;
                    current_dir = dir_stack[stack_top];
                }
            } else {
                Directory* temp_dir = current_dir;
                change_directory(&temp_dir, arg1);
                if (temp_dir != current_dir) {
                    current_dir = temp_dir;
                    stack_top++;
                    dir_stack[stack_top] = current_dir;
                    strncpy(path_name_stack[stack_top], arg1, MAX_PATH_LENGTH);
                }
            }
        } else if (strcmp(command, "mkdir") == 0) {
            scanf("%s", arg1);
            if (btree_search(current_dir->tree, arg1)) {
                printf("mkdir: Nao foi possivel criar o diretorio '%s': Arquivo ou diretorio ja existe\n", arg1);
            } else {
                TreeNode* new_dir_node = create_directory(arg1);
                btree_insert(current_dir->tree, new_dir_node);
            }
        } else if (strcmp(command, "touch") == 0) {
            scanf("%s", arg1);
            if (btree_search(current_dir->tree, arg1)) {
                printf("touch: Nao foi possivel criar o arquivo '%s': Arquivo ou diretorio ja existe\n", arg1);
            } else {
                TreeNode* new_file_node = create_txt_file(arg1, "Conteudo de exemplo.");
                btree_insert(current_dir->tree, new_file_node);
            }
        } else if (strcmp(command, "rm") == 0) {
            scanf("%s", arg1);
            TreeNode* node_to_delete = btree_search(current_dir->tree, arg1);
            if (node_to_delete && node_to_delete->type == FILE_TYPE) {
                delete_txt_file(current_dir->tree, arg1);
            } else {
                printf("rm: Nao foi possivel remover o arquivo '%s': Arquivo nao encontrado\n", arg1);
            }
        } else if (strcmp(command, "rmdir") == 0) {
            scanf("%s", arg1);
            TreeNode* node_to_delete = btree_search(current_dir->tree, arg1);
            if (node_to_delete && node_to_delete->type == DIRECTORY_TYPE) {
                if (node_to_delete->data.directory->tree->root == NULL) {
                    delete_directory(current_dir->tree, arg1);
                } else {
                    printf("rmdir: Nao foi possivel remover o diretorio '%s': Diretorio nao vazio\n", arg1);
                }
            } else {
                printf("rmdir: Nao foi possivel remover o diretorio '%s': Diretorio nao encontrado\n", arg1);
            }
            // delete_directory(current_dir->tree, arg1);
        } else if (strcmp(command, "ajuda") == 0) {
            printf("Comandos disponiveis:\n"
                   "  ls                - Lista conteudo do diretorio atual\n"
                   "  cd <dir>          - Muda para o diretorio <dir>\n"
                   "  mkdir <dir>       - Cria um novo diretorio\n"
                   "  touch <arquivo>   - Cria um novo arquivo .txt\n"
                   "  rm <arquivo>      - Remove um arquivo\n"
                   "  rmdir <dir>       - Remove um diretorio vazio\n"
                   "  exit              - Sai do programa\n");
        } else {
            printf("Comando nao reconhecido: %s\n", command);
        }
        
        save_filesystem(root); // Salva o sistema de arquivos após cada comando

        // Limpa o buffer do scanf
        while (getchar() != '\n');
    }

    printf("Saindo...\n");

    return 0;
}
