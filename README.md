*This project was created as part of the 42 curriculum by vvieira, tmorais-.*

---

## Description

Minishell is a simplified shell interpreter built from scratch in C, using POSIX as a reference. The goal is to understand how a shell works internally — from reading user input to tokenizing, parsing, expanding variables, and executing commands with full support for pipes, redirections, heredocs, and built-in commands.

The implementation covers:

- **Lexer** — tokenizes input into a linked list of tokens, handling single and double quotes, mixed tokens, and empty quotes
- **Parser** — builds an Abstract Syntax Tree (AST) from the token list
- **Variable expansion** — `$VAR`, `$?`, `$$` with suppression inside single quotes
- **Executor** — walks the AST using fork/execve for external commands, runs builtins in the parent process
- **Pipelines** — support for `|` with multiple parallel processes
- **Heredoc** — collected in the parent process before any fork, with variable expansion support
- **Signals** — `Ctrl+C`, `Ctrl+\`, `Ctrl+Z` and `Ctrl+D` handled correctly in parent and child processes
- **Builtins** — `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`

---

## Instructions

### Dependencies

Requires `libreadline`. To install on Ubuntu/Debian:

```bash
sudo apt install libreadline-dev
```

### Compilation

```bash
make        # builds ./minishell
make clean  # removes object files
make fclean # removes objects and binary
make re     # fclean + make
make val    # builds and runs under valgrind with readline suppressions
```

### Execution

```bash
./minishell
```

The minishell displays an interactive prompt. Usage examples:

```bash
minishell$ echo "Hello, $USER"
minishell$ ls -la | grep src
minishell$ export MY_VAR=42
minishell$ cat << EOF
heredoc> $MY_VAR
heredoc> EOF
minishell$ exit
```

---

## Resources

### References

- [Bash Manual](https://www.gnu.org/software/bash/manual/bash.html) — main reference for expected behavior of builtins, expansion, and signals
- [POSIX Shell Grammar](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html) — formal shell grammar specification
- [Valgrind User Manual](https://valgrind.org/docs/manual/manual.html) — used for memory leak detection and elimination
- [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html) — interactive input library with history support

### AI Usage

AI tools were used as support during the development of this project to assist with research, conceptual clarification, and general review. All code was written and understood by the authors.