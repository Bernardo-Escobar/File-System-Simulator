# 📂 File System Simulator — Sistema de Arquivos com Árvore B

Repositório desenvolvido para a Avaliação M3 da disciplina de **Sistemas Operacionais (2025/1)** da **Universidade do Vale do Itajaí (UNIVALI)**.  
O projeto consiste na criação de um **sistema de arquivos virtual** baseado na estrutura de dados **Árvore B**, com funcionalidades básicas de manipulação de arquivos e diretórios via terminal.

---

## 🎯 Objetivo

Desenvolver um simulador de sistema de arquivos utilizando uma **Árvore B** para representar hierarquicamente:

- Arquivos `.txt` com conteúdo textual  
- Pastas e subpastas organizadas recursivamente  
- Navegação entre diretórios com comandos estilo Unix (`cd`, `ls`)  
- Operações de inserção, exclusão e listagem

---

## 🧱 Estrutura de Arquivos

```bash
File-System-Simulator/
├── main_fs.c         # Arquivo principal para interação com o sistema
├── filesystem.c      # Implementação das funções do sistema de arquivos
├── filesystem.h      # Definições de structs e funções
├── fs.img            # Arquivo gerado representando a imagem do sistema de arquivos
├── programa          # Binário compilado (opcional)
└── README.md         # Este arquivo
```

## ⚙️ Como Compilar e Executar
🔧 Compilar
```bash
gcc main_fs.c filesystem.c -o programa
```
▶️ Executar
```bash
./programa
```

## ✅ Funcionalidades
🗃️ Arquivos `.txt`
Inserção: Cria arquivos `.txt` com conteúdo (até 1MB)

Remoção: Exclui arquivos por nome

Conteúdo: Associado a cada arquivo por ponteiro (`char* content`)

📁 Diretórios
Criação: Diretórios podem conter subpastas e arquivos

Remoção: Somente diretórios vazios podem ser removidos

Navegação: Uso de comandos como `cd`, `ls`

🌳 Estrutura com Árvore B
Cada diretório contém uma árvore B para armazenar seus filhos

Nó da árvore contém:

Nome do item

Tipo (`arquivo` ou `diretório`)

Ponteiro para o conteúdo (se arquivo) ou sub-árvore (se pasta)

## 💻 Interface de Comando Simulada
Comandos disponíveis:

`touch <arquivo>` — cria arquivo .txt

`rm <arquivo>` — remove arquivo

`mkdir <dir>` — cria diretório

`rmdir <dir>` — remove diretório vazio

`cd <dir>` — acessa diretório

`ls` — lista conteúdos do diretório atual

`exit` — encerra o programa e salva estrutura em `fs.img`

## 📚 Conceitos Praticados
Simulação de sistema de arquivos

Árvores B como estrutura de diretório

Manipulação de strings e memória dinâmica em C

Entrada e saída via terminal

Modularização com múltiplos arquivos fonte

## 🧪 Exemplo de Execução
```shell
> mkdir documentos
> cd documentos
> create file trabalho.txt "Conteúdo do trabalho"
> ls
📄 trabalho.txt
> cd ..
> ls
📁 documentos
```

## 👨‍💻 Autores
Projeto desenvolvido por alunos da Universidade do Vale do Itajaí (UNIVALI) — Campus Itajaí Disciplina: Sistemas Operacionais

## 📝 Licença
Uso acadêmico e educacional. Livre para consulta e estudo.
