#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

void	test_case(const char *description, const char *input)
{
	t_token	*tokens;

	printf("\n");
	printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
	printf("🧪 TESTE: %s\n", description);
	printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
	printf("INPUT: \"%s\"\n", input);
	printf("\n");

	tokens = tokenize(input);

	if (tokens)
	{
		print_tokens(tokens);
		free_tokens(tokens);
		printf("✅ Tokenização bem-sucedida\n");
	}
	else
	{
		printf("❌ ERRO: tokenize() retornou NULL\n");
	}
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	(void)envp;

	printf("\n");
	printf("╔════════════════════════════════════════════════╗\n");
	printf("║        SUITE DE TESTES - LEXER MINISHELL      ║\n");
	printf("╔════════════════════════════════════════════════╗\n");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 1: Comandos Simples\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Comando único", "ls");
	test_case("Comando com argumento", "ls -la");
	test_case("Comando com múltiplos argumentos", "echo hello world");
	test_case("Comando com path absoluto", "/bin/ls");
	test_case("Comando com path relativo", "./minishell");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 2: Pipes\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Pipe simples", "ls | grep test");
	test_case("Pipe com argumentos", "cat file.txt | grep hello");
	test_case("Múltiplos pipes", "cat file | grep test | wc -l");
	test_case("Pipe sem espaços", "ls|grep");
	test_case("Pipe com espaços extras", "ls  |  grep  test");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 3: Redirecionamentos\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Redirecionamento saída >", "echo hello > output.txt");
	test_case("Redirecionamento entrada <", "cat < input.txt");
	test_case("Append >>", "echo test >> file.txt");
	test_case("Heredoc <<", "cat << EOF");
	test_case("Múltiplos redirecionamentos", "cat < in.txt > out.txt");
	test_case("Redirecionamento sem espaços", "cat<file>out");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 4: Aspas Simples\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Aspas simples básicas", "echo 'hello world'");
	test_case("Aspas simples vazias", "echo ''");
	test_case("Aspas com espaços internos", "echo '  hello  world  '");
	test_case("Múltiplas palavras com aspas", "echo 'hello' 'world'");
	test_case("Aspas no meio da palavra", "echo a'b'c");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 5: Aspas Duplas\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Aspas duplas básicas", "echo \"hello world\"");
	test_case("Aspas duplas vazias", "echo \"\"");
	test_case("Aspas duplas com espaços", "echo \"  hello  \"");
	test_case("Aspas simples dentro de duplas", "echo \"hello 'world'\"");
	test_case("Concatenação de aspas", "echo \"a\"b\"c\"");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 6: Variáveis ($)\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Variável simples", "echo $USER");
	test_case("Exit status", "echo $?");
	test_case("Variável no início", "$HOME/docs");
	test_case("Múltiplas variáveis", "echo $USER $HOME");
	test_case("Variável dentro de aspas duplas", "echo \"Hello $USER\"");
	test_case("Variável dentro de aspas simples", "echo 'Hello $USER'");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 7: Comandos Complexos\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Pipe + Redirecionamento", "cat file.txt | grep test > output.txt");
	test_case("Aspas + Pipe", "echo 'hello world' | grep hello");
	test_case("Tudo junto", "cat < in.txt | grep 'test' | wc -l > out.txt");
	test_case("Comando longo", "ls -la /home/user | grep txt | wc -l > count.txt");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 8: Whitespace\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Múltiplos espaços", "ls    -la");
	test_case("Espaços no início", "   ls");
	test_case("Espaços no fim", "ls   ");
	test_case("Apenas espaços", "   ");
	test_case("String vazia", "");
	test_case("Tab entre palavras", "echo\thello");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 9: Edge Cases\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Operadores colados", "cat<file>out|grep");
	test_case("Números", "echo 123 456");
	test_case("Caracteres especiais em palavra", "echo test_file-v2.txt");
	test_case("Path longo", "/usr/local/bin/ls");
	test_case("Palavra + aspas + palavra", "pre'mid'post");

	printf("\n\n");
	printf("═══════════════════════════════════════════════\n");
	printf("📂 CATEGORIA 10: Detecção de Erros\n");
	printf("═══════════════════════════════════════════════\n");

	test_case("Aspas simples não fechadas", "echo 'hello");
	test_case("Aspas duplas não fechadas", "echo \"world");
	test_case("Aspas mistas não fechadas", "echo 'hello\"");

	printf("\n\n");
	printf("╔════════════════════════════════════════════════╗\n");
	printf("║            TESTES CONCLUÍDOS                   ║\n");
	printf("╔════════════════════════════════════════════════╗\n");
	printf("\n");
	printf("✅ Se todos os tokens foram mostrados corretamente,\n");
	printf("   seu lexer está funcionando!\n");
	printf("\n");
	printf("⚠️  Verifique se:\n");
	printf("   1. Aspas preservam espaços\n");
	printf("   2. Operadores são reconhecidos (|, <, >, <<, >>)\n");
	printf("   3. $ é marcado como DOLLAR\n");
	printf("   4. Aspas não fechadas dão erro\n");
	printf("   5. EOF aparece no final de cada teste\n");
	printf("\n");

	return (0);
}
