# Minishell - Projeto 42

## 📋 Descrição

Este é o projeto Minishell da Escola 42, reescrito completamente seguindo a **Norma 42**.

O projeto implementa um interpretador de linha de comando (shell) minimalista, similar ao Bash, com funcionalidades de:
- ✅ Análise léxica (tokenização)
- 🔄 Loop principal com readline
- 📝 Histórico de comandos
- 🎯 Tratamento de aspas
- 💾 Estruturas de dados modulares

## 🏗️ Estrutura do Projeto

```
minishell_norma/
├── Makefile
├── includes/
│   └── minishell.h          # Header principal
└── src/
    ├── main.c               # Ponto de entrada
    ├── parser/
    │   ├── lexer.c         # Analisador léxico
    │   └── lexer_utils.c   # Utilitários do lexer
    ├── shell/
    │   └── shell.c         # Loop principal do shell
    └── utils/
        ├── prompt.c        # Gerenciamento de prompt
        └── utils.c         # Funções utilitárias
```

## 🔧 Compilação

### Pré-requisitos

Instale a biblioteca readline:

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install libreadline-dev
```

**macOS:**
```bash
brew install readline
```

### Compilar o projeto

```bash
make
```

### Executar

```bash
./minishell
```

### Limpar arquivos de compilação

```bash
make clean    # Remove objetos
make fclean   # Remove objetos e executável
make re       # Recompila tudo
```

## 🎮 Como Usar

Ao executar o minishell, você verá o prompt:

```
========================================
          Minishell com Lexer
========================================
Comandos disponíveis:
  - Qualquer comando: mostra tokens gerados
  - test: ativa modo debug do lexer
  - exit: sai do shell
========================================

minishell$
```

### Exemplos de comandos:

```bash
minishell$ ls -la
minishell$ cat file.txt | grep hello
minishell$ echo "hello world" > output.txt
minishell$ cat << EOF
minishell$ exit
```

O lexer irá mostrar os tokens identificados em cada comando.

## 📚 Documentação

Consulte o arquivo **`minishell_explicacao.pdf`** para uma explicação completa de:
- Conceitos fundamentais
- Estruturas de dados
- Algoritmo de tokenização
- Tratamento de aspas
- Próximos passos do desenvolvimento

## ✅ Conformidade com a Norma

Todo o código está em conformidade com a Norma 42:
- Headers padrão em todos os arquivos
- Máximo de 25 linhas por função
- Máximo de 80 colunas por linha
- Funções static para auxiliares
- Nomenclatura snake_case
- Indentação com tabs

Para verificar:
```bash
norminette src includes
```

## 🚀 Estado Atual do Projeto

### ✅ Implementado:
- [x] Lexer completo (tokenização)
- [x] Reconhecimento de pipes (|)
- [x] Reconhecimento de redirecionamentos (<, >, <<, >>)
- [x] Tratamento de aspas (' e ")
- [x] Identificação de variáveis ($)
- [x] Interface readline com histórico
- [x] Estruturas de dados base

### 🔄 Próximos Passos:
- [ ] Parser (análise sintática)
- [ ] Expansão de variáveis
- [ ] Executor (fork, execve, pipes)
- [ ] Comandos built-in (echo, cd, pwd, export, unset, env, exit)
- [ ] Gerenciamento de sinais (Ctrl-C, Ctrl-D, Ctrl-\\)
- [ ] Here document (<<)

## 📝 Notas de Desenvolvimento

### Estruturas Principais:

**t_token** - Representa um token léxico
```c
typedef struct s_token {
    t_token_type  type;   // WORD, PIPE, REDIR_IN, etc
    char         *value;  // Conteúdo do token
    int           len;    // Comprimento
    struct s_token *next; // Lista encadeada
} t_token;
```

**t_lexer** - Estado do analisador léxico
```c
typedef struct s_lexer {
    const char *input;    // String de entrada
    size_t      input_len;
    int         pos;      // Posição atual
    char        current;  // Caractere atual
    int         in_quote; // Estado de aspas (0/1/2)
} t_lexer;
```

**t_shell** - Estado global do shell
```c
typedef struct s_shell {
    char   **env;         // Variáveis de ambiente
    int      exit_status; // Código de saída
    t_token *tokens;      // Lista de tokens
} t_shell;
```

## 🐛 Debug

Para debugar com gdb:
```bash
make
gdb ./minishell
```

O projeto já é compilado com flag `-g` para debug.

## 📖 Referências

- [Readline Manual](https://tiswww.case.edu/php/chet/readline/rltop.html)
- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
- [Advanced Programming in the UNIX Environment](https://stevens.netmeister.org/631/)

## 👥 Autores

Projeto desenvolvido para a Escola 42.

---

**Versão:** 1.0 (Lexer Completo)
**Data:** Fevereiro 2026
**Status:** 🟡 Em Desenvolvimento
