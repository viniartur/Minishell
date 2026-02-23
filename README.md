# 🐚 Minishell

> A minimal UNIX shell implementation in C — 42 School project.

## A project made by vvieira and tmorais-
---

## 📋 About

Minishell is a simplified shell interpreter built from scratch in C, following the POSIX standard as a reference. The goal is to understand how a shell works internally — from reading user input to tokenizing, parsing, and eventually executing commands.

---

## 📁 Project Structure

```
minishell/
├── includes/
│   └── minishell.h          # All structs, enums, and function prototypes
├── srcs/
│   ├── main.c               # Entry point
│   ├── parser/
│   │   ├── lexer.c          # Tokenizer: converts input string into token list
│   │   ├── lexer_utils.c    # Lexer helpers: init, advance, peek, create_token, skip_whitespace
│   │   ├── parser.c         # Parser: converts token list into AST
│   │   ├── parser_utils.c   # Parser helpers: create_command, add_argument, redirections
│   │   └── ast.c            # AST node creation and memory management
│   ├── shell/
│   │   ├── shell.c          # Main loop, input handling, shell lifecycle
│   │   └── executor_utils.c # Command path resolution (get_command_path)
│   └── utils/
│       ├── utils.c          # ft_ string functions, memory utils
│       └── prompt.c         # Readline prompt and history
└── Makefile
```

---

## 🏗️ Architecture

The project is organized in a pipeline of distinct stages:

```
Input (readline)
      ↓
  [ LEXER ]        → Produces a linked list of tokens
      ↓
  [ PARSER ]       → Produces an Abstract Syntax Tree (AST)
      ↓
  [ EXECUTOR ]     → Walks the AST and runs commands  (not yet implemented)
```

### Key Data Structures

**`t_token`** — A node in the token linked list:
```c
typedef struct s_token {
    t_token_type    type;   // WORD, PIPE, REDIR_IN, DOLLAR, etc.
    char            *value; // Token value (only for TOKEN_WORD)
    int             len;
    struct s_token  *next;
} t_token;
```

**`t_lexer`** — Internal lexer state:
```c
typedef struct s_lexer {
    const char  *input;
    size_t      input_len;
    int         pos;
    int         start;
    char        current;
    int         in_quote;   // 0=none, 1=single, 2=double
    int         error;
} t_lexer;
```

**`t_ast_node`** — A node in the AST:
```c
typedef struct s_ast_node {
    t_node_type type;       // NODE_PIPELINE or NODE_COMMAND
    union {
        struct { t_ast_node *left; t_ast_node *right; } pipeline;
        t_command   *cmd;
        t_redir     *redir;
    } data;
} t_ast_node;
```

**`t_shell`** — Global shell state:
```c
typedef struct s_shell {
    char        **env;
    int         exit_status;
    int         should_exit;
    t_token     *tokens;
    t_ast_node  *ast;
} t_shell;
```

---

## ✅ What's Implemented

### Lexer (`lexer.c`, `lexer_utils.c`)
- Tokenizes the full input string into a linked list of `t_token`
- Handles all token types: `WORD`, `PIPE`, `REDIR_IN`, `REDIR_OUT`, `HEREDOC`, `APPEND`, `DOLLAR`, `SEMICOLON`, `AMPERSAND`, `ASSIGN`, `EOF`
- Quote handling:
  - Single quotes `'...'` — content treated as literal (no expansion)
  - Double quotes `"..."` — content preserved, `$` will expand (future)
  - Quotes of one type inside the other are treated as literal characters
  - Unclosed quotes produce a syntax error
- Words built into a dynamic buffer (allocated per input size) — no fixed-size overflow risk
- Token list built with O(1) insertion using a `last` pointer

### Parser (`parser.c`, `parser_utils.c`, `ast.c`)
- Builds an AST from the token list
- Supports pipelines: `cmd1 | cmd2 | cmd3` → left-associative pipeline tree
- Supports redirections: `<`, `>`, `>>`, `<<` — can be interleaved with words
- `parse_command` handles: `[WORD]* [redir | WORD]*`
- Memory managed correctly: `free_ast` recursively frees the whole tree
- `print_ast` for debug visualization (indented tree output)

### Shell lifecycle (`shell.c`)
- `init_shell` — initializes all fields including `should_exit = 0`
- `main_loop` — readline loop, exits only on `Ctrl+D` (EOF) or `exit` command
- `handle_input` / `process_command` — tokenizes, parses, and dispatches
- `cleanup_shell` — frees env, tokens, AST, and readline history

### Utilities (`utils.c`)
- `ft_strlen`, `ft_strdup`, `ft_substr`, `ft_strchr`
- `ft_strcmp`, `ft_strncmp`, `ft_strcspn`
- `ft_memcpy`, `ft_strjoin`, `ft_split`
- `ft_isspace`, `ft_copy_env`
- `free_shell`

---

## 🚧 Not Yet Implemented

| Feature | Notes |
|---|---|
| Executor | Walk the AST and actually run commands |
| Builtins | `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit` |
| Variable expansion | `$VAR`, `$?`, `${VAR}` — lexer produces `DOLLAR + WORD`, expansion pending |
| Signal handling | `Ctrl+C` (SIGINT), `Ctrl+\` (SIGQUIT) |
| Heredoc (`<<`) | Token recognized, execution not implemented |
| Pipes execution | AST built correctly, `fork`/`pipe`/`execve` not yet |
| Redirections execution | AST built correctly, `open`/`dup2` not yet |

---

## 🔧 Compilation

```bash
make        # builds ./minishell
make clean  # removes object files
make fclean # removes objects and binary
make re     # fclean + make
```

> Requires `libreadline`. On Ubuntu/Debian: `sudo apt install libreadline-dev`

---

## 🧪 Current Debug Output

While the executor is not yet implemented, running a command shows the token list and AST:

```
minishell$ ls -la | grep .c
=== TOKENS ===
[WORD: 'ls']
[WORD: '-la']
[PIPE]
[WORD: 'grep']
[WORD: '.c']
[EOF]
==============
PIPELINE:
  COMMAND: ['ls', '-la']
  COMMAND: ['grep', '.c']
```

> Debug prints (`print_tokens`, `print_ast`) will be removed once the executor is in place.

---

## 📌 Notes

- The project follows the **42 School norm** (Norminette)
- No use of `strtok`, `system`, or `execvp` with path resolution bypassed
- All memory allocations are checked and freed
- Global variable `g_signal` is the only global, used for signal handling

---

*README reflects the current state of the project — will be updated as features are added.*
