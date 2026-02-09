#!/bin/bash

echo "🔧 Instalando dependências para Minishell no WSL Ubuntu"
echo ""

# Atualiza lista de pacotes
echo "📦 Atualizando lista de pacotes..."
sudo apt-get update

# Instala readline development
echo "📚 Instalando libreadline-dev..."
sudo apt-get install -y libreadline-dev

# Verifica instalação
echo ""
echo "✅ Verificando instalação..."

if [ -f /usr/include/readline/readline.h ]; then
    echo "✅ readline.h encontrado em /usr/include/readline/readline.h"
else
    echo "❌ readline.h NÃO encontrado!"
    exit 1
fi

if [ -f /usr/lib/x86_64-linux-gnu/libreadline.so ]; then
    echo "✅ libreadline.so encontrado"
else
    echo "❌ libreadline.so NÃO encontrado!"
    exit 1
fi

echo ""
echo "🎉 Instalação concluída com sucesso!"
echo ""
echo "Agora você pode compilar com:"
echo "  make"
