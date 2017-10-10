%{
Algoritmo de seleção de nó de retransmissão mestre Huenstein.

Parametros Avaliados:

Perda de pacotes (Prioridade - 01) Pontos: 50
Vazão (Prioridade - 02) Pontos: 40
Energia (Prioridade - 03) Pontos: 30
Alcance (Prioridade - 04) Pontos: 20
Distância (Prioridade - 05) Pontos: 10 
%}

%Criação dos nós Multi Relay
%OBS: Uma Matriz sendo as linhas os nós Multi Relay e as Colunas os parametros).
n=3;
p=5;
% 
MR(1:n, 1:p)= 9;





%Importação do arquivo trace


%Analise da Perda de Pacotes
%Analise da Vazão
%Analise de Energia
%Analise do Alcance
%Analise da Distância

%Ätribuição da pontuação dos nós Multi Relay

