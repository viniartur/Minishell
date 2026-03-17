# 🐚 Minishell

> A minimal UNIX shell implementation in C — 42 School project.

## A project made by vvieira and tmorais-

---

## 📋 About

Minishell is a simplified shell interpreter built from scratch in C, following the POSIX standard as a reference. The goal is to understand how a shell works internally — from reading user input to tokenizing, parsing, expanding variables, and executing commands with full support for pipes, redirections, heredocs, and built-in commands.

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
│   │   └── shell.c               # Main loop, input handling, shell lifecycle
│   ├── executor/
│   │   ├── executor.c            # execute_ast, execute_command, execute_pipeline, handle_redirections
│   │   └── executor_utils.c      # Command path resolution (get_command_path)
│   ├── builtins/
│   │   ├── builtins.c            # is_builtin, exec_builtin dispatch
│   │   ├── builtins_nav.c        # echo, pwd, cd, exit
│   │   └── builtins_env.c        # env, export, unset
│   ├── var_expansion/
│   │   ├── expansion.c           # expand_all_variables, expand_variable, expand_exit_status, pid_to_str
│   │   └── expansion_utils.c     # extract_var_name, join_strings, is_valid_var_char
│   └── utils/
│       ├── utils.c               # ft_ string functions, memory utils, ft_putstr_fd
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
  [ EXPANDER ]     → Expands variables during parsing ($VAR, $?, $$)
      ↓
  [ EXECUTOR ]     → Walks the AST: forks external commands, runs builtins in parent
      ↓
  [ BUILTINS ]     → echo, cd, pwd, export, unset, env, exit
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

**`t_redir`** — A redirection node:
```c
typedef struct s_redir {
    int             type;       // REDIR_IN, REDIR_OUT, APPEND, HEREDOC
    char            *file;      // File name or heredoc delimiter
    char            *content;   // Heredoc body (collected lines), NULL for other redirs
    int             expand;     // 1 = expand $VAR inside heredoc, 0 = literal (quoted delim)
    int             fd;
    struct s_redir  *next;
} t_redir;
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
- Handles all token types: `WORD`, `PIPE`, `REDIR_IN`, `REDIR_OUT`, `HEREDOC`, `APPEND`, `DOLLAR`, `QUOTE`, `DQUOTE`, `EOF`
- Quote handling:
  - Single quotes `'...'` → `TOKEN_QUOTE` — literal, no expansion
  - Double quotes `"..."` → `TOKEN_DQUOTE` — expands `$VAR`
  - Mixed words like `oi"$USER"'$USER'` are split into multiple adjacent tokens and concatenated by the parser
  - Empty quotes `''` and `""` produce a valid empty-string argument
  - Unclosed quotes produce a syntax error
- `=` is not treated as a special delimiter — `VAR=value` is tokenized as a single `TOKEN_WORD`, enabling `export VAR=value` to work correctly
- `preceded_by_space` flag on each token enables correct argument boundary detection in the parser

### Parser (`parser.c`, `parser_utils.c`, `ast.c`)
- Builds an AST from the token list
- Grammar supported:
  - `command_list = pipeline ( ';' pipeline )*`
  - `pipeline     = command ( '|' command )*`
  - `command      = simple_command redirection*`
- Redirections `<`, `>`, `>>`, `<<` fully parsed; heredoc delimiter stores `expand` flag
- `collect_argument` concatenates adjacent tokens (no space between) into a single argument
- Memory managed correctly: `free_ast` recursively frees the whole tree

### Variable Expansion (`expansion.c`, `expansion_utils.c`)
- `$VAR` — looks up variable in `shell->env`
- `$?` — expands to the last exit status
- `$$` — expands to the PID of the shell process
- Undefined variables expand to empty string `""`
- `$` alone expands to literal `"$"`
- Quote type controls expansion: `TOKEN_QUOTE` → no expansion; `TOKEN_DQUOTE` / `TOKEN_WORD` → expands

### Executor (`executor.c`, `executor_utils.c`)
- `execute_ast` — dispatches `NODE_COMMAND` and `NODE_PIPELINE`
- `execute_command`:
  - Builtins run in the parent process with save/restore of file descriptors
  - External commands: `fork` → `handle_redirections` → `get_command_path` → `execve`
  - Exit status propagated via `WIFEXITED` / `WIFSIGNALED` (handles signals like CTRL+C with code 128+n)
- `execute_pipeline`:
  - Creates a `pipe(fd)`, forks left and right sides independently
  - Recursive: handles `a | b | c` via nested `PIPELINE` nodes
  - Final exit status is that of the rightmost command
- `handle_redirections`:
  - `>` — `O_WRONLY | O_CREAT | O_TRUNC`
  - `>>` — `O_WRONLY | O_CREAT | O_APPEND`
  - `<` — `O_RDONLY`
  - `<<` — creates a `pipe`, writes heredoc content, passes read-end as STDIN
- `collect_heredoc` / `prepare_heredocs`:
  - Called before `fork`, reads lines via `readline("heredoc> ")` from `/dev/tty` (not stdin, so pipes don't interfere)
  - If `expand == 1`, expands `$VAR` in each line before storing
  - If `expand == 0` (quoted delimiter), stores content literally
  - CTRL+D during heredoc prints a warning and closes gracefully

### Builtins (`builtins.c`, `builtins_nav.c`, `builtins_env.c`)

| Builtin | Status | Notes |
|---------|--------|-------|
| `echo`   | ✅ Full | Supports `-n` flag |
| `pwd`    | ✅ Full | Uses `getcwd` |
| `env`    | ✅ Full | Prints all environment variables |
| `cd`     | ✅ Full | Handles no args (`$HOME`), `~`, `-` (`$OLDPWD`), errors; updates `PWD`/`OLDPWD` |
| `export` | ✅ Full | Adds/updates env vars; no args prints `declare -x` list; validates identifiers |
| `unset`  | ✅ Full | Removes variables from `shell->env` |
| `exit`   | ✅ Full | Numeric validation, `too many arguments` check, code mod 256 |

- All builtins support redirections (e.g. `pwd > file.txt`)
- `exit` inside a pipeline runs in a subshell and does not close the parent shell

### Shell lifecycle (`shell.c`)
- `init_shell` — initializes all fields
- `main_loop` — readline loop, exits on `Ctrl+D` or `exit`
- `process_command` — tokenizes → parses → executes via `execute_ast`
- `cleanup_shell` — frees env, tokens, AST, readline history

### Utilities (`utils.c`)
- `ft_strlen`, `ft_strdup`, `ft_substr`, `ft_strchr`
- `ft_strcmp`, `ft_strncmp`, `ft_strcspn`
- `ft_memcpy`, `ft_strjoin`, `ft_split`
- `ft_isspace`, `ft_copy_env`, `ft_putstr_fd`

---

## 🚧 Pending

| Feature | Notes |
|---------|-------|
| Signal handling | `SIGINT` (Ctrl+C), `SIGQUIT` (Ctrl+\\), `SIGTSTP` (Ctrl+Z) — to be implemented by vvieira |
| Heredoc prompt cosmetic | When used in a pipeline, the `heredoc>` prompt may be visually affected by the pipe's transformation (e.g. `tr a-z A-Z`). Edge case. |

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

## 📌 Notes

- The project follows the **42 School norm** (Norminette)
- No use of `strtok`, `system`, or `execvp` with path resolution bypassed
- All memory allocations are checked and freed
- Global variable `g_signal` is the only global, used for signal handling
- `exit` inside a pipeline correctly runs in a child process and does not affect the parent shell

---

*README reflects the current state of the project — will be updated when signal handling is complete.*