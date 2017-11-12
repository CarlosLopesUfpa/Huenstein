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


//Escreve os valores recivuto		
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



//Na verdade o importante é o que importa ... aqui acontece a porra toda...(emojin da dançarina do whatsapp) 

				float normalmMR[nAp][nPar];
				float FinalScore[nAp][2];

				for (int l = 0; l < nAp; ++l){

				normalmMR[l][1] = (mMR[l][1]/somaPerdaPct);
				normalmMR[l][2] = (mMR[l][2]/somaVazao);
				normalmMR[l][3] = (mMR[l][3]/somaEnergia);
				normalmMR[l][4] = (mMR[l][4]/somaDelay);
				normalmMR[l][5] = (mMR[l][5]/somaAlcance);

				FinalScore[l][0]=l;
				FinalScore[l][1]=((((1-normalmMR[l][1])/(nAp-1))*0.3)+(normalmMR[l][2]*0.25)+(normalmMR[l][3]*0.2)+(((1-normalmMR[l][4])/(nAp-1))*0.15)+(((1-normalmMR[l][5])/(nAp-1))*0.1))*100;
				
				std::cout << "Nó " << l << " Pontuação perda de pacotes " << ((1-normalmMR[l][1])*100)*0.3<<std::endl;
				std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25<<std::endl;
				std::cout << "Nó " << l << " Pontuação Energia " << (normalmMR[l][3]*100)*0.2<<std::endl;
				std::cout << "Nó " << l << " Pontuação Delay " << ((1-normalmMR[l][4])*100)*0.15<<std::endl;
				std::cout << "Nó " << l << " Pontuação Alcance " << ((1-normalmMR[l][5])*100)*0.1<<std::endl;
				std::cout << " " <<std::endl;
				}

//Escrever de forma agrupada os pontos de cada nó 

				for (int l = 0; l < nAp; ++l){
								
					std::cout << "Nó " << l << " Pontuação perda de pacotes " << ((1-normalmMR[l][1])*100)*0.3<<std::endl;
					
				}
				std::cout << " " <<std::endl;
				for (int l = 0; l < nAp; ++l){
						
					std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25 <<std::endl;
					
				}
				std::cout << " " <<std::endl;
				for (int l = 0; l < nAp; ++l){
										
					std::cout << "Nó " << l << " Pontuação energia " << (normalmMR[l][3]*100)*0.2 <<std::endl;
					
				}
				std::cout << " " <<std::endl;						
				for (int l = 0; l < nAp; ++l){
																
					std::cout << "Nó " << l << " Pontuação Delay " << ((1-normalmMR[l][4])*100)*0.15<<std::endl;
					
				}
				std::cout << " " <<std::endl;
				for (int l = 0; l < nAp; ++l){
										
					std::cout << "Nó " << l << " Pontuação Alcance " << ((1-normalmMR[l][5])*100)*0.1<<std::endl;
									
				}
				std::cout << " " <<std::endl;


//Escrevendo o resultado final (FinalScore) referente à cada Nó
		
				for(int l=0; l<nAp; ++l){						
				std::cout << "Nó " << FinalScore[l][0] << " Pontuação Geral " << FinalScore[l][1] <<std::endl;
				std::cout << "////////////////////////////" <<std::endl;
				}
								

//Imprimir quem foi o Nó fodão picagrossa*** das galaxias

				float maior_ic = 0;
				int id = 0;
				for (int l=0; l<nAp; ++l){
					if(FinalScore[l][1] > maior_ic){
								maior_ic = FinalScore[l][1];
								id = l;
					}
				}
				std::cout << " " <<std::endl;
				std::cout << "Nó de Retransmissão Mestre " << id << " Pontuação: " << maior_ic <<std::endl;

}
		

//***tome esse referência kkkkk