# Minishell - Tão belo quanto uma concha 🐚

## 📝 Introdução
O **Minishell** é um projeto de desenvolvimento de um shell simples, inspirado no Bash. O objetivo principal é proporcionar uma compreensão profunda sobre a criação de processos, manipulação de descritores de arquivos e o funcionamento interno de um interpretador de comandos.

Este projeto é uma viagem no tempo para experimentar os desafios que os desenvolvedores enfrentaram antes da existência do Windows, comunicando-se com o computador através de linhas de comando interativas.

---

## 🛠️ Especificações Técnicas

| Característica | Detalhes |
| :--- | :--- |
| **Linguagem** | C (conforme a Norma) |
| **Versão** | 9.0 |
| **Compilador** | `cc` com flags `-Wall -Wextra -Werror` |
| **Executável** | `minishell` |
| **Gerenciamento de Memória** | Zero leaks (exceto os causados pela `readline`) |

---

## 🚀 Funcionalidades (Parte Obrigatória)

O shell implementa as seguintes capacidades essenciais:

* **Prompt**: Exibição de um prompt funcional aguardando novos comandos.
* **Histórico**: Histórico de comandos funcional (`add_history`).
* **Execução**: Busca e execução de binários via variável `PATH` ou caminhos relativos/absolutos.
* **Aspas**: 
    * `' '` (Aspas simples): Impede a interpretação de metacaracteres.
    * `" "` (Aspas duplas): Impede a interpretação, exceto para `$` (expansão de variável).
* **Redirecionamentos**:
    * `<` : Redireciona a entrada.
    * `>` : Redireciona a saída.
    * `<<` : Lê a entrada até encontrar um delimitador (Here-doc).
    * `>>` : Redireciona a saída em modo de anexação (append).
* **Pipes (`|`)**: A saída de cada comando no pipeline é conectada à entrada do próximo.
* **Variáveis de Ambiente**: Expansão de `$VARIAVEL` e do status de saída `$?`.
* **Sinais**: Comportamento idêntico ao Bash:
    * `ctrl-C` : Novo prompt em uma nova linha.
    * `ctrl-D` : Sai do shell.
    * `ctrl-\` : Não faz nada.

### Comandos Built-in:
* `echo` (com opção `-n`)
* `cd` (apenas caminho relativo ou absoluto)
* `pwd`, `export`, `unset`, `env`, `exit` (sem opções adicionais)

---

## 🌟 Bônus
* Operadores lógicos `&&` e `||` com parênteses para prioridades.
