#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;

void avalParam();
int nAp = 5;


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
				int LostPackets [nAp][1];
				int Throughput [nAp][1];
				int Energy [nAp][1];
				int Delay [nAp][1];
				int Range [nAp][1];

// Determinar fluxo
				// int flow[nAp+1][0];
				for(int l=0; l<nAp; ++l){
					LostPackets [nAp][0] = 0;
			 		Throughput [nAp][0] = 0;
					Energy [nAp][0] = 0;
					Delay [nAp][0] = 0;
					Range [nAp][0] = 0;
				}

//Atribuir valores dos Parâmetros
				srand((unsigned)time(0));
				for (int l = 0; l<nAp; ++l){

					LostPackets [l][0]= rand()%(1000);
														
					Throughput [l][0]= rand()%(5);

					Energy [l][0]= rand()%(100);

					Delay [l][0]= rand()%(1000);

					Range [l][0]= rand()%(1000);
				}
				
				std::cout << "Valor de parametros: " <<std::endl;
				for(int l=0;l<nAp; l++){
				std::cout << " " <<std::endl;
				std::cout << "Cont " << l <<std::endl;
				std::cout << " " << " " <<std::endl;
				std::cout << "LostPackets " << LostPackets [l][0] << " " <<std::endl;
				std::cout << "Throughput " << Throughput[l][0] << " " <<std::endl;
				std::cout << "Energy " << Energy[l][0] << " " <<std::endl;
				std::cout << "Delay " << Delay[l][0]<< " " <<std::endl;
				std::cout << "Range " << Range[l][0] << " " <<std::endl;
				}
				
							
				

//Criar Matriz dos nós de retransmissão
				int mMR [nAp][nPar+1];

				for (int l = 0; l < nAp; ++l)
				{
					for (int c = 0; c <= nPar; ++c)
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
							mMR [l][c] = Range [l][0];
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
				int high_Range = 0;

				for (int l = 0; l < nAp; ++l){
					for (int c = 0; c <= nPar; ++c){
						
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

						if(Range[l][0]> high_Range){
							high_Range = Range[l][0];
						}

					}
				}
				std::cout << " " << " " <<std::endl;
				std::cout << " " << " " <<std::endl;
				std::cout << "LOW_LostPackets " << low_LostPckt << " " <<std::endl;
				std::cout << "HIGH_Throughput " << high_Thoughput << " " <<std::endl;
				std::cout << "HIGH_Energy " << high_Energy << " " <<std::endl;
				std::cout << "LOW_Delay " << low_Delay<< " " <<std::endl;
				std::cout << "HIGH_Range " << high_Range << " " <<std::endl;
				std::cout << " " << " " <<std::endl;
				std::cout << " " << " " <<std::endl;

//Atribuir Pontuação aos MRs
				float sum [nAp][1];

				float auxLostpackt = 0;
				float auxThroughput = 0;
				float auxEnergy = 0;
				float auxDelay = 0;
				float auxRange = 0;

				for (int l = 0; l < nAp; ++l){
					
				sum[l][0] = 0;
					
				}
				
				// for(int l=0; l<nAp; ++l){
				// 	for(int c=0; c<nPar; ++c){
				// 		if(mMR[l][c] == mMR[l+1][c]){
				
				// 			mMR[l][c] = sum[l][0]/2;
				// 			mMR[l+1][c] = sum[l][0]/2;

				// 		}
				// 	}
				// }

				for (int l = 0; l < nAp; ++l){


								if(mMR[l][1]==low_LostPckt){
									sum[l][0] = sum[l][0] + 30;

									auxLostpackt = auxLostpackt + sum[l][0];
									if(auxLostpackt == 60){
										sum[l][0] = sum[l][0] - 15;
										std::cout << "                     "<< sum[l][0] <<std::endl;
										sum[l-1][0] = sum[l-1][0] - 15;
										std::cout << "                     "<< sum[l-1][0] <<std::endl;
										auxLostpackt = 0;

									}
								}
								std::cout << "Cont: "<< l << " Sum: "<< sum[l][0] <<std::endl;

								if(mMR[l][2]==high_Thoughput){
									sum[l][0] = sum[l][0] + 25;

									auxThroughput = auxThroughput + sum[l][0];
									if(auxThroughput >= 80){
										sum[l][0] = sum[l][0] - 12.5;
										std::cout << "                     "<< sum[l][0] <<std::endl;
										sum[l-1][0] = sum[l-1][0] - 12.5;
										std::cout << "                     "<< sum[l-1][0] <<std::endl;
										auxThroughput = 0;

									}
								}
								std::cout << "Cont: "<< l << " Sum: "<< sum[l][0] <<std::endl;

								if(mMR[l][3]==high_Energy){
									sum[l][0] = sum[l][0] + 20;

									auxEnergy = auxEnergy + sum[l][0];
									if(auxEnergy >= 95){
										sum[l][0] = sum[l][0] - 10;
										std::cout << "                     "<< sum[l][0] <<std::endl;
										sum[l-1][0] = sum[l-1][0] - 10;
										std::cout << "                     "<< sum[l-1][0] <<std::endl;
										auxEnergy = 0;

									}
								}
								std::cout << "Cont: "<< l << " Sum: "<< sum[l][0] <<std::endl;

								if(mMR[l][4]==low_Delay){
									sum[l][0] = sum[l][0] + 15;

									auxDelay = auxDelay + sum[l][0];
									if(auxDelay == 105){
										sum[l][0] = sum[l][0] - 7.5;
										std::cout << "                     "<< sum[l][0] <<std::endl;
										sum[l-1][0] = sum[l-1][0] - 7.5;
										std::cout << "                     "<< sum[l-1][0] <<std::endl;
										auxDelay = 0;

									}
								}
								std::cout << "Cont: "<< l << " Sum: "<< sum[l][0] <<std::endl;

								if(mMR[l][5]==high_Range){
									sum[l][0] = sum[l][0] + 10;

									auxRange = auxRange + sum[l][0];
									if(auxRange == 110){
										sum[l][0] = sum[l][0] - 5;
										std::cout << "                     "<< sum[l][0] <<std::endl;
										sum[l-1][0] = sum[l-1][0] - 5;
										std::cout << "                     "<< sum[l-1][0] <<std::endl;
										auxRange = 0;

									}
								}
								std::cout << "Cont: "<< l << " Sum: "<< sum[l][0] <<std::endl;
								


								std::cout << " " << " " <<std::endl;
								std::cout << " " << " " <<std::endl;
					}
				

//Imprimir Resultados
				for (int l=0; l<nAp; ++l){
					std::cout << "Nó de Retransmissão " << mMR[l][0] << " Pontuação: " << sum[l][0] <<std::endl;
					}

				}
