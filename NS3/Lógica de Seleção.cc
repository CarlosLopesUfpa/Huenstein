#include <stdio.h>
#include <iostream>

using namespace std;

void avalParam();
int nAp = 2;


int main()
{

avalParam();

	return 0;
}

			  void avalParam()
			  {
//Determinar quantidade de parâmetros
				int nPar = 5;

//Determinar os Parametros utilizados
				int LostPackets [nAp][0];
				int Throughput [nAp][0];
				int Energy [nAp][0];
				int Delay [nAp][0];
				int Alcance [nAp][0];

// Determinar fluxo
				int flow[nAp][0];
				for(int l=0; l<nAp; ++l){
					
				}

//Atribuir valores dos Parâmetros
				for (int l = 0; l<nAp; ++l){

					for (int p = 0; p<nAp; ++p){
					LostPackets[l][0]=flow[p][0];
					}
					
					Throughput [l][0]=2;

					Energy [l][0]=3;

					Delay [l][0]=4;

					Alcance [l][0]=5;
					}
				
				
							
				

//Criar Matriz dos nós de retransmissão
				int mMR [nAp][nPar+1];

				for (int l = 0; l < nAp; ++l)
				{
					for (int c = 0; c < nPar; ++c)
					{
						switch(c){
							case 0:
							mMR [l][c] = l;
							break;
							case 1:
							mMR [l][c] = LostPackets [l][0];	
							break;				
							case 2:
							mMR [l][c] = Throughput [l][0];
							break;
							case 3:
							mMR [l][c] = Energy [l][0];
							break;
							case 4:
							mMR [l][c] = Delay [l][0];
							break;
							case 5:
							mMR [l][c] = Alcance [l][0];
							break;
							default:
							break;
						}
					}
				}
//Comparar Parâmetros
				int low_LostPckt = 2147483647;
				int high_Thoughput = 0;
				int high_Energy = 0;
				int low_Delay = 2147483647;
				int high_Alcance = 0;

				for (int l = 0; l < nAp; ++l){
					for (int c = 0; c < nPar; ++c){
						
						if(LostPackets[l][0] < low_LostPckt){
							low_LostPckt = LostPackets[l][0];
						}

						if(Throughput[l][0] > high_Thoughput){
							high_Thoughput = Throughput[l][0];
						}

						if(Energy[l][0]>high_Energy){
							high_Energy = Energy[l][0];
						}

						if(Delay[l][0] < low_Delay){
							low_Delay = Delay[l][0];
						}

						if(Alcance[l][0]< high_Alcance){
							high_Alcance = Alcance[l][0];
						}

					}
				}

//Atribuir Pontuação aos MRs
				int sum [nAp][0];

				for (int l = 0; l < nAp; ++l){
					
				sum[l][0] = 0;
					
				}

				for (int l = 0; l < nAp; ++l){
					for (int c = 1; c < nPar; ++c){
						switch(c){
							case 1:
								if(mMR[l][c]==low_LostPckt){
									sum[l][0] = sum[l][0] + 30;
								}
							break;				
							case 2:
								if(mMR[l][c]==high_Thoughput){
									sum[l][0] = sum[l][0] + 25;
								}
							break;
							case 3:
								if(mMR[l][c]==high_Energy){
									sum[l][0] = sum[l][0] + 20;
								}
							break;
							case 4:
								if(mMR[l][c]==low_Delay){
									sum[l][0] = sum[l][0] + 15;
								}
							break;
							case 5:
								if(mMR[l][c]==high_Alcance){
									sum[l][0] = sum[l][0] + 10;
								}
							break;
							default:
							break;
						}


					}
				}

//Imprimir Resultados
				for (int l=0; l<nAp; ++l){
					std::cout << "Nó de Retransmissão " << mMR[l][0] << " Pontuação: " << sum[l][0] <<std::endl;
					}

				}
