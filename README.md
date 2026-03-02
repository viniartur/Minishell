# 🐚 Minishell

> A minimal UNIX shell implementation in C — 42 School project.

## A project made by vvieira and tmorais-

---

## 📋 About

Minishell is a simplified shell interpreter built from scratch in C, following the POSIX standard as a reference. The goal is to understand how a shell works internally — from reading user input to tokenizing, parsing, expanding variables, and executing commands.

---

## 📁 Project Structure

```
minishell/
├── includes/
│   └── minishell.h               # All structs, enums, and function prototypes
├── srcs/
│   ├── main.c                    # Entry point
│   ├── parser/
│   │   ├── lexer.c               # Tokenizer: converts input string into token list
│   │   ├── lexer_utils.c         # Lexer helpers: init, advance, peek, create_token, skip_whitespace
│   │   ├── parser.c              # Parser: converts token list into AST + variable expansion
│   │   ├── parser_utils.c        # Parser helpers: create_command, add_argument, redirections
│   │   └── ast.c                 # AST node creation and memory management
│   ├── shell/
│   │   ├── shell.c               # Main loop, input handling, shell lifecycle, builtin dispatch
│   │   └── executor_utils.c      # Command path resolution (get_command_path)
│   ├── builtins/
│   │   ├── builtins.c            # is_builtin, exec_builtin dispatch
│   │   ├── builtins_nav.c        # echo, pwd, cd, exit
│   │   └── builtins_env.c        # env, export, unset
│   ├── var_expansion/
│   │   ├── expansion.c           # expand_all_variables, expand_variable, expand_exit_status
│   │   └── expansion_utils.c     # extract_var_name, join_strings, is_valid_var_char
│   └── utils/
│       ├── utils.c               # ft_ string functions, memory utils
│       └── prompt.c              # Readline prompt and history
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
  [ EXPANDER ]     → Expands variables during parsing ($VAR, $?, $?$?)
      ↓
  [ BUILTINS ]     → Executes built-in commands in the parent process
      ↓
  [ EXECUTOR ]     → Walks the AST and runs external commands (not yet implemented)
```

### Key Data Structures

**`t_token`** — A node in the token linked list:
```c
typedef struct s_token {
    t_token_type    type;               // WORD, PIPE, REDIR_IN, DOLLAR, QUOTE, DQUOTE, etc.
    char            *value;             // Token value (for WORD, QUOTE, DQUOTE)
    int             len;
    int             preceded_by_space;  // 1 if whitespace appeared before this token
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
    int         had_space;  // 1 if skip_whitespace advanced before current token
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
- Handles all token types: `WORD`, `PIPE`, `REDIR_IN`, `REDIR_OUT`, `HEREDOC`, `APPEND`, `DOLLAR`, `QUOTE`, `DQUOTE`, `SEMICOLON`, `AMPERSAND`, `ASSIGN`, `EOF`
- Quote handling:
  - Single quotes `'...'` → `TOKEN_QUOTE` — content treated as literal, no expansion
  - Double quotes `"..."` → `TOKEN_DQUOTE` — content preserved, `$VAR` will expand
  - Empty quotes `''` and `""` produce a valid empty-string argument
  - Quotes of one type inside the other are treated as literal characters
  - Unclosed quotes produce a syntax error
- `TOKEN_DOLLAR` handler reads only valid variable name characters `[a-zA-Z0-9_]`, stopping at `/`, spaces, and other delimiters — so `$HOME/file` correctly tokenizes as `DOLLAR + WORD:'HOME' + WORD:'/file'`
- `preceded_by_space` flag on each token tracks whether whitespace appeared before it, enabling correct argument boundary detection in the parser
- Token list built with O(1) insertion using a `last` pointer

### Parser (`parser.c`, `parser_utils.c`, `ast.c`)
- Builds an AST from the token list using an internal `t_parser` struct (token pointer + shell reference)
- Grammar supported:
  - `command_list = pipeline ( ';' pipeline )*`
  - `pipeline     = command ( '|' command )*`
  - `command      = simple_command redirection*`
- Redirections: `<`, `>`, `>>`, `<<` — can be interleaved with words; file name supports variable expansion (e.g. `> $HOME/file.txt`)
- `collect_argument` concatenates adjacent tokens with no space between them into a single argument:
  - `$?$?` → `"00"` (two DOLLARs with no space = one argument)
  - `"$USER"_suffix` → `"tmfanfa_suffix"` (DQUOTE + WORD glued together)
- Memory managed correctly: `free_ast` recursively frees the whole tree
- `print_ast` for debug visualization (indented tree output)

### Variable Expansion (`expansion.c`, `expansion_utils.c`)
- Expansion happens **during parsing**, integrated into `collect_argument`
- Quote type determines expansion behavior:
  - `TOKEN_QUOTE` (single quotes) → **no expansion**, value returned as-is
  - `TOKEN_DQUOTE` (double quotes) → **expands** `$VAR` inside
  - `TOKEN_WORD` (no quotes) → **expands** `$VAR`
- `$VAR` — looks up variable in `shell->env`
- `$?` — expands to the last exit status as a string
- Undefined variables expand to empty string `""`
- `$` alone (not followed by a valid name or `?`) expands to the literal `"$"`

### Builtins (`builtins.c`, `builtins_nav.c`, `builtins_env.c`)
- Built-in commands run in the **parent process** (no fork) when the AST root is a simple command
- Implemented:

| Builtin | Status | Notes |
|---------|--------|-------|
| `echo` | ✅ Full | Supports `-n` flag |
| `pwd`  | ✅ Full | Uses `getcwd` |
| `env`  | ✅ Full | Prints all environment variables |
| `exit` | ✅ Full | Sets `should_exit`, shell exits cleanly |
| `cd`   | 🔧 Stub | Accepts args, no-op for now |
| `export` | 🔧 Stub | Accepts args, no-op for now |
| `unset`  | 🔧 Stub | Accepts args, no-op for now |

### Shell lifecycle (`shell.c`)
- `init_shell` — initializes all fields including `should_exit = 0`
- `main_loop` — readline loop, exits on `Ctrl+D` (EOF) or `exit` command
- `process_command` — tokenizes → parses (with expansion) → dispatches to builtins or future executor
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
|---------|-------|
| Executor | `fork` + `execve` for external commands |
| `cd` (full) | `chdir`, update `PWD`/`OLDPWD` in env |
| `export` (full) | Add/update variables in `shell->env` |
| `unset` (full) | Remove variables from `shell->env` |
| Signal handling | `Ctrl+C` (SIGINT), `Ctrl+\` (SIGQUIT) |
| Heredoc execution | Token and AST recognized; `<<` input reading not implemented |
| Pipe execution | AST built correctly; `fork`/`pipe`/`execve` not yet |
| Redirection execution | AST built correctly; `open`/`dup2` not yet |
| `${VAR}` syntax | Brace-delimited variable names not yet supported |

---

## 🔧 Compilation

```bash
make        # builds ./minishell
make clean  # removes object files
make fclean # removes objects and binary
make re     # fclean + make
```

> Requires `libreadline`. On Ubuntu/Debian: `sudo apt install libreadline-dev`

> If you see `make: warning: Clock skew detected` after copying files, run:
> ```bash
> touch $(find . -name "*.c" -o -name "*.h") && make re
> ```

---

## 🧪 Current Debug Output

While the executor is not yet fully implemented, running a command shows the token list and AST:

```
minishell$ echo "$USER"_suffix
=== TOKENS ===
[WORD: 'echo']
[DQUOTE: '$USER']
[WORD: '_suffix']
[EOF]
==============
PARSER: Initializing parsing...
PARSER: successfully concluded
DEBUG: AST after var expansion:
COMMAND: ['echo', 'tmfanfa_suffix']
tmfanfa_suffix
```

```
minishell$ echo '$HOME'
=== TOKENS ===
[WORD: 'echo']
[QUOTE: '$HOME']
[EOF]
==============
PARSER: Initializing parsing...
PARSER: successfully concluded
DEBUG: AST after var expansion:
COMMAND: ['echo', '$HOME']
$HOME
```

> Debug prints (`print_tokens`, `print_ast`) will be removed once the executor is complete.

---

## 📌 Notes

- The project follows the **42 School norm** (Norminette)
- No use of `strtok`, `system`, or `execvp` with path resolution bypassed
- All memory allocations are checked and freed
- Global variable `g_signal` is the only global, used for signal handling

---

*README reflects the current state of the project — will be updated as features are added.*
