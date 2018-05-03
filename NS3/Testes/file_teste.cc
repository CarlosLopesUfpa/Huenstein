#include <stdio.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
int main()
{
  FILE *arq;
  char Linha[10];
  char *result;
  double vet[10][1];
  char *aux;
  // clrscr();
  // Abre um arquivo TEXTO para LEITURA
  arq = fopen("50_pos_x.txt", "rt");
  if (arq == NULL)  // Se houve erro na abertura
  {
     printf("Problemas na abertura do arquivo\n");
     return(0);
  }
  for(int i = 0; i<10; i++){

  // Lê uma linha (inclusive com o '\n')
      result = fgets(Linha, 10, arq);  // o 'fgets' lê até 99 caracteres ou até o '\n'
      if (result)  // Se foi possível ler
      cout<<"Linha: "<<i<<"|"<<Linha<<"\n";
      aux = Linha;
      vet[i][0] = atof (aux);
      cout<<"aux: "<<aux<<"\n";
  }
  for(int y = 0; y<10; y++){
      cout<<"End Fora Vetor: "<<vet[y]<<"\n";
      cout<<"Fora Vetor: "<<*vet[y]<<"\n";
  }

  fclose(arq);

  return(0);
}