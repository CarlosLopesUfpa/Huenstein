#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;

void avalParam();
int nAp = 3;


int main()
{

avalParam();

	return 0;
}


void avalParam(){
//Determinar quantidade de parâmetros
				int nPar = 5;

//Determinar os Parametros utilizados
				float LostPackets [nAp][1];
				float Throughput [nAp][1];
				float Energy [nAp][1];
				float Delay [nAp][1];
				float Range [nAp][1];

//Atribuir 0 a todas as posições da matriz (limpar)
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

					Range [l][0]= rand()%(100);
				}
				
				std::cout << "Valor de parametros: " <<std::endl;
				for(int l=0;l<nAp; l++){
				std::cout << " " <<std::endl;
				std::cout << "Cont: " << l <<std::endl;
				std::cout << " " << " " <<std::endl;
				std::cout << "LostPackets " << LostPackets [l][0] << " " <<std::endl;
				std::cout << "Throughput " << Throughput[l][0] << " " <<std::endl;
				std::cout << "Energy " << Energy[l][0] << " " <<std::endl;
				std::cout << "Delay " << Delay[l][0]<< " " <<std::endl;
				std::cout << "Range " << Range[l][0] << " " <<std::endl;
				}
				
							
				

//Criar Matriz dos nós de retransmissão
				float mMR [nAp][nPar+1];

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


				//int low_LostPckt = 2147483647;
				//int high_Thoughput = 0;
				//int high_Energy = 0;
				//int low_Delay = 2147483647;
				//int high_Range = 0;

				// for (int l = 0; l < nAp; ++l){
				// 	for (int c = 0; c <= nPar; ++c){
						
				// 		if(LostPackets[l][0] < low_LostPckt){
				// 			low_LostPckt = LostPackets[l][0];
				// 		}

				// 		if(Throughput[l][0] > high_Thoughput){
				// 			high_Thoughput = Throughput[l][0];
				// 		}

				// 		if(Energy[l][0]>high_Energy){
				// 			high_Energy = Energy[l][0];
				// 		}

				// 		if(Delay[l][0] < low_Delay){
				// 			low_Delay = Delay[l][0];
				// 		}

				// 		if(Range[l][0]> high_Range){
				// 			high_Range = Range[l][0];
				// 		}

				// 	}
				
				//std::cout << " " << " " <<std::endl;
				//std::cout << " " << " " <<std::endl;
				//std::cout << "LOW_LostPackets " << low_LostPckt << " " <<std::endl;
				//std::cout << "HIGH_Throughput " << high_Thoughput << " " <<std::endl;
				//std::cout << "HIGH_Energy " << high_Energy << " " <<std::endl;
				//std::cout << "LOW_Delay " << low_Delay<< " " <<std::endl;
				//std::cout << "HIGH_Range " << high_Range << " " <<std::endl;
				//std::cout << " " << " " <<std::endl;
				//std::cout << " " << " " <<std::endl;


//mMR[0][1]= perda de pacotes      |   	
//mMR[0][2]= vazão                 |
//mMR[0][3]= energia               |   Médias/Valores "brutos", ainda nao normalizadas
//mMR[0][4]= delay                 |
//mMR[0][5]= alcance               |

float somaPerdaPct = 0;
float somaVazao = 0;
float somaEnergia = 0;
float somaDelay = 0;
float somaAlcance = 0; 

						for (int l = 0; l < nAp; ++l){

							somaPerdaPct =  somaPerdaPct +  mMR[l][1];  
							somaVazao    =  somaVazao    +  mMR[l][2];
							somaEnergia  =  somaEnergia  +  mMR[l][3];
							somaDelay    =  somaDelay    +  mMR[l][4];
							somaAlcance	 =  somaAlcance  +  mMR[l][5];

						}
						for(int l = 0; l<1; l++){
							std::cout << " " <<std::endl;
							std::cout << "Somatória: " << l <<std::endl;
							std::cout << " " << " " <<std::endl;
							std::cout << "LostPackets " << somaPerdaPct  << " " <<std::endl;
							std::cout << "Throughput " << somaVazao  << " " <<std::endl;
							std::cout << "Energy " << somaEnergia  << " " <<std::endl;
							std::cout << "Delay " << somaDelay  << " " <<std::endl;
							std::cout << "Range " << somaAlcance  << " " <<std::endl;
						}


//somaPerdaPct   | 
//somaVazão      |
//somaEnergia    |  Essas variáveis recebem a soma dos valores dos parametros de todos os nós, 
//somaDelay      |  vamo usar pra normalizar os valores antes de usar na fórmula
//somaAlcance    |



//Fórmula, calculo valor paramentros * peso = valor normalizado para 100%							
/*
						for (int l = 0; l < nAp; ++l){
							
							mMR[l][1] = mMR[l][1];
								fórmula_do_poder = (perdaPtcNormalizada*0,3);
						}

						




						std::cout << "	Id do Nó escolhido: "<< mMR[Lost_esc][1] <<std::endl;
						sum[Lost_esc][0] = sum[Lost_esc][0]+30;
							

								
						std::cout << " " << " " <<std::endl;
						std::cout << " " << " " <<std::endl;
					
				

//Imprimir Resultados
						for (int l=0; l<nAp; ++l){
							std::cout << "Nó de Retransmissão " << mMR[l][0] << " Pontuação: " << sum[l][0] <<std::endl;
						}
*/
}
		
 
