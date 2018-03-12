#include <stdio.h>

int main ()
{
  int tempo;
  double dado;
  int tempo2;
  double dado2;

  FILE * pArquivo1;
  FILE * pArquivo2;
  FILE * pDestino;

  pArquivo1 = fopen ("Simulation_1_Graficos/Jitter/lte_Flow_vs_Jitter_Group_1.txt", "r");
  pArquivo2 = fopen ("Simulation_2_Graficos/Jitter/Simulation_2_Flow_vs_Jitter_Group_1.txt", "r");
  pDestino = fopen ("twolines/Jitter_1.txt", "w+");


	if(pArquivo1,pArquivo2 == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");
	else
		while( (fscanf(pArquivo1,"%i %lf\n", &tempo, &dado))!=EOF && (fscanf(pArquivo2,"%i %lf\n", &tempo2, &dado2))!=EOF)
			fprintf(pDestino,"%i %lf %lf\n", tempo, dado, dado2);

  printf ("Finished!\n");
  return 0;
}