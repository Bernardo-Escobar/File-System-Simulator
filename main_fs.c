#include "filesystem.h"

int main() {
    // Criação do sistema de arquivos
    Directory* root = get_root_directory();

    // Diretórios de segundo nível
    TreeNode* dirSO = create_directory("SO");
    TreeNode* dirTEST = create_directory("TEST");
    btree_insert(root->tree, dirSO);
    btree_insert(root->tree, dirTEST);


    // Arquivo em SO
    TreeNode* file1 = create_txt_file("arquivo1.txt", "Arquivo de teste de SO.");
    btree_insert(dirSO->data.directory->tree, file1);

    TreeNode* dir1 = create_directory("dir1");
    TreeNode* dir2 = create_directory("dir2");
    btree_insert(root->tree, dir1);
    btree_insert(root->tree, dir2);

    TreeNode* file2 = create_txt_file("arquivo2.txt", "Arquivo de teste de SO.");
    btree_insert(dir1->data.directory->tree, file2);
    TreeNode* file3 = create_txt_file("arquivo2.txt", "Arquivo de teste de SO.");
    btree_insert(dir1->data.directory->tree, file3);
    

    // Listagem
    printf("--- Conteúdo do diretório ROOT ---\n");
    list_directory_contents(root);

    printf("\n--- Conteúdo do diretório SO ---\n");
    list_directory_contents(dirSO->data.directory);

    printf("\n--- Conteúdo do diretório 1 ---\n");
    list_directory_contents(dir1->data.directory);

    printf("\n--- Conteúdo do diretório 2 ---\n");
    list_directory_contents(dir2->data.directory);

    // Simulação de persistência
    FILE* img = fopen("fs.img", "w");
    if (img) {
        fprintf(img, "ROOT\n");
        fprintf(img, "├── SO\n");
        fprintf(img, "│   └── arquivo1.txt: Arquivo de teste de SO.\n");
        fprintf(img, "└── TEST\n");
        fclose(img);
        printf("\nSistema de arquivos salvo em fs.img\n");
    } else {
        perror("Erro ao criar fs.img");
    }

    return 0;
}
