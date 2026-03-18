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
│   ├── main.c                    # Entry point + g_signal global declaration
│   ├── parser/
│   │   ├── lexer.c               # Tokenizer: converts input string into token list
│   │   ├── lexer_utils.c         # Lexer helpers: init, advance, peek, create_token, skip_whitespace
│   │   ├── parser.c              # Parser: converts token list into AST + variable expansion
│   │   ├── parser_utils.c        # Parser helpers: create_command, add_argument, redirections
│   │   └── ast.c                 # AST node creation and memory management
│   ├── shell/
│   │   ├── shell.c               # Main loop, input handling, shell lifecycle
│   │   └── shell_free.c          # free_shell, child_exit
│   ├── executor/
│   │   ├── executor.c            # execute_ast, execute_command, execute_pipeline, handle_redirections
│   │   ├── executor_heredoc.c    # collect_heredoc, prepare_heredocs, prepare_ast_heredocs
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
│       ├── utils2.c              # Additional ft_ helpers
│       ├── utils3.c              # Additional ft_ helpers
│       ├── utils4.c              # join_strings, free_split_result
│       ├── prompt.c              # Readline prompt and history
│       └── signals.c             # Signal handlers: SIGINT, SIGQUIT, SIGTSTP
└── Makefile
```

---

## 🏗️ Architecture

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

---

## ✅ What's Implemented

### Lexer
- Tokenizes input into a linked list of `t_token`
- Single quotes `'...'` → literal, no expansion
- Double quotes `"..."` → expands `$VAR`
- Mixed tokens like `"$USER"'$USER'` split and concatenated by the parser
- Empty quotes `''` / `""` produce valid empty-string arguments
- Unclosed quotes produce a syntax error

### Parser
- Builds an AST from the token list
- Grammar: `command_list → pipeline ( ';' pipeline )* `, `pipeline → command ( '|' command )*`
- Adjacent tokens (no space) concatenated into a single argument
- Heredoc delimiter collected literally — `$HOME` as delimiter means the literal string `$HOME`

### Variable Expansion
- `$VAR` — environment lookup
- `$?` — last exit status
- `$$` — PID of the shell
- Undefined variables → empty string
- Single quotes suppress all expansion

### Executor
- External commands: `fork` → `handle_redirections` → `execve`
- Builtins run in the parent with saved/restored file descriptors
- Pipelines: recursive `NODE_PIPELINE` via two forks per `|`
- Heredocs collected in the **parent process** before any fork via `prepare_ast_heredocs`, so `readline("heredoc> ")` always writes to the real terminal
- All forked children call `child_exit` to free inherited memory before exiting

### Signals
- `Ctrl+C` on empty prompt: new line + new prompt
- `Ctrl+C` on running command: delivered to child (`SIG_DFL`), parent ignores during wait
- `Ctrl+\` : ignored in parent, default in child
- `Ctrl+Z` : ignored
- `Ctrl+D` : exits the shell
- Single global `g_signal` stores only the received signal number

### Builtins

| Builtin  | Notes |
|----------|-------|
| `echo`   | Supports `-n` flag |
| `pwd`    | Uses `getcwd` |
| `env`    | Prints current environment |
| `cd`     | Handles no args, `~`, `-`, `.`, `..`; updates `PWD`/`OLDPWD` |
| `export` | Adds/updates vars; no args prints `declare -x` list |
| `unset`  | Removes variables from environment |
| `exit`   | Numeric validation, `too many arguments` check, code mod 256 |

- All builtins support redirections (e.g. `pwd > file.txt`)
- `exit` inside a pipeline runs in a subshell and does not close the parent

---

## 🔧 Compilation

```bash
make        # builds ./minishell
make clean  # removes object files
make fclean # removes objects and binary
make re     # fclean + make
make val    # builds and runs under valgrind with readline suppressions
```

> Requires `libreadline`. On Ubuntu/Debian: `sudo apt install libreadline-dev`

---

## 🧠 Design Decisions

- **Single global variable** `g_signal` — used only to store the signal number received asynchronously. No other data passes through globals.
- **Heredoc collected before fork** — `prepare_ast_heredocs` traverses the full AST in the parent before any `fork`, ensuring `readline("heredoc> ")` always writes to the real terminal and never into a pipe.
- **`child_exit` instead of `exit`** — all forked children call `child_exit(shell, code)` which frees inherited tokens, AST, and env before exiting, keeping valgrind clean.
- **Heredoc delimiter never expanded** — `$HOME` as a delimiter is treated as the literal string `$HOME` (bash-compatible). Any quoted part of the delimiter disables variable expansion inside the heredoc body.

---

## 📌 Notes

- Follows the **42 School norm** (Norminette)
- No use of `strtok`, `system`, or `execvp` with bypassed path resolution
- All heap memory freed before exit — valgrind reports `definitely lost: 0`, `indirectly lost: 0`, `possibly lost: 0`
