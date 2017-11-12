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
				double LostPackets [nAp][1];
				double Throughput [nAp][1];
				double Energy [nAp][1];
				double Delay [nAp][1];
				double Range [nAp][1];

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
				std::cout << "Nó: " << l <<std::endl;
				std::cout << " " <<std::endl;
				std::cout << "LostPackets " << LostPackets [l][0] << " " <<std::endl;
				std::cout << "Throughput " << Throughput[l][0] << " " <<std::endl;
				std::cout << "Energy " << Energy[l][0] << " " <<std::endl;
				std::cout << "Delay " << Delay[l][0]<< " " <<std::endl;
				std::cout << "Range " << Range[l][0] << " " <<std::endl;
				}
				
							
				

//Criar Matriz dos nós de retransmissão
				double mMR [nAp][nPar+1];

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

//mMR[0][1]= perda de pacotes      |   	
//mMR[0][2]= vazão                 |
//mMR[0][3]= energia               |   Médias/Valores "brutos", ainda nao normalizadas
//mMR[0][4]= delay                 |
//mMR[0][5]= alcance               |

double somaPerdaPct = 0;
double somaVazao = 0;
double somaEnergia = 0;
double somaDelay = 0;
double somaAlcance = 0; 

						for (int l = 0; l < nAp; ++l){

							somaPerdaPct =  somaPerdaPct +  mMR[l][1];  
							somaVazao    =  somaVazao    +  mMR[l][2];
							somaEnergia  =  somaEnergia  +  mMR[l][3];
							somaDelay    =  somaDelay    +  mMR[l][4];
							somaAlcance	 =  somaAlcance  +  mMR[l][5];

						}
						for(int l = 0; l<1; l++){
							std::cout << " " <<std::endl;
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



//Normalização Inversa e Atribuição da pontuação de perda de pacotes ao Ap (Valor Normalizado * Pontuação do parametro).
				double normalmMR[nAp][nPar];
							
						for (int l = 0; l < nAp; ++l){
							normalmMR[l][1] = (mMR[l][1]/somaPerdaPct)*100;
							mMR[l][1] = normalmMR[l][1]*0.30;
		
							std::cout << "Nó " << l << " Pontuação perda de pacotes " << mMR[l][1] <<std::endl;
							std::cout << " " <<std::endl;
						}
//Normalização e Atribuição da pontuação de Vazão ao Ap.
						for (int l = 0; l < nAp; ++l){
							normalmMR[l][2] = (mMR[l][2]/somaVazao)*100;
							mMR[l][2] = normalmMR[l][2]*0.25;
				
							std::cout << "Nó " << l << " Pontuação vazão " << mMR[l][2] <<std::endl;
							std::cout << " " <<std::endl;
						}
//Normalização e Atribuição da pontuação de Energia ao Ap.
						for (int l = 0; l < nAp; ++l){
							normalmMR[l][3] = (mMR[l][3]/somaEnergia)*100;
							mMR[l][3] = normalmMR[l][3]*0.20;
			
							std::cout << "Nó " << l << " Pontuação energia " << mMR[l][3] <<std::endl;
							std::cout << " " <<std::endl;
						}
//Normalização Inversa e Atribuição da pontuação de Delay ao Ap.
						
						for (int l = 0; l < nAp; ++l){
							normalmMR[l][4] = (mMR[l][4]/somaDelay)*100;
							mMR[l][4] = normalmMR[l][4]*0.15;
									
							std::cout << "Nó " << l << " Pontuação Delay " << mMR[l][4] <<std::endl;
							std::cout << " " <<std::endl;
						}
//Normalização e Atribuição da pontuação de Alcance ao Ap.
						for (int l = 0; l < nAp; ++l){
							normalmMR[l][5] = (mMR[l][5]/somaAlcance)*100;
							mMR[l][5] = normalmMR[l][5]*0.10;
				
							std::cout << "Nó " << l << " Pontuação alcance " << mMR[l][5] <<std::endl;
							std::cout << " " <<std::endl;
					
						}
//Somatória da pontuação dos Aps.
				double ic[nAp][2];
						for(int l=0; l<nAp; ++l){
							for(int c=0; c<=nPar; ++c){
								ic[l][0] = l;
								ic[l][1] = ic[l][1]+mMR[l][c];
							}
						
						std::cout << "Nó " << ic[l][0] << " Pontuação Geral " << ic[l][1] <<std::endl;
						std::cout << "////////////////////////////" <<std::endl;
						}
								

//Imprimir Resultados
						double maior_ic = 0;
						double id = 0;
						for (int l=0; l<nAp; ++l){
							if(ic[l][1] > maior_ic){
								maior_ic = ic[l][1];
								id = l;
							}
						}
						std::cout << " " <<std::endl;
						std::cout << "Nó de Retransmissão Mestre " << id << " Pontuação: " << maior_ic <<std::endl;

}
		
 
