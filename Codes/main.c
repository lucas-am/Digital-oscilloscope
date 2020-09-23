/*!
 * \file main.c
 * \brief Osciloscopio Digital
 * 
 * \details Este c&oacute;digo ilustra o funcionamento de um Osciloscopio Digital implementado usando o ADC do microcontrolador.
 * \mainpage Projeto de Osciloscopio Digital.
 * \author Lucas Alves Martins e Bruno Edson Limeira
 * \date 03/11/2016
 * \note Documenta&ccedil;&atilde;o do c&oacute;digo no formato de Doxygen:
 *       http://mcuoneclipse.com/2014/09/01/automatic-documentation-generation-doxygen-with-processor-expert/
 */

#include "derivative.h"
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "global.h"
#include "medidas.h"

char titulo[6][150] = {
"																				                                                                 ",
"         dPfYb  .dPfY8  dPffb8 88 88      dPfYb  .dPfY8  dPffb8  dPfYb  88ffYb 88  dPfYb           8888bb  88  dPffb8 88 888888    db    88     ",   
"        dP   Yb `Ybo.  dP      88 88     dP   Yb `Ybo.  dP   `  dP   Yb 88__dP 88 dP   Yb           8I  Yb 88 dP   `f 88   88     dPYb   88     ",
"        Yb   dP o.`Y8b Yb      88 88   o Yb   dP o.`Y8b Yb      Yb   dP 88     88 Yb   dP           8I  dY 88 Yb   88 88   88    dP__Yb  88  .o ",
"         YbodP  8bodP'  YboodP 88 88ood8  YbodP  8bodP'  YboodP  YbodP  88     88  YbodP            888Y   88  YboodP 88   88   dP    Yb 88ood8 ",
"																				                                                                 "
};

/*
 * Frases usadas no programa
*/
char frases[11][40] = { 
	"OSCILOSCӐIO DIGITAL",						//frase 0
	"DIGITE UM COMANDO",						//frase 1
	"ADQUIRINDO DADOS, AGUARDE...",				//frase 2
	"AMOSTRAGEM CONCLǗA",						//frase 3
	"COMANDO INVALIDO",							//frase 4
	"NڍERO FORA DOS LIMITES",					//frase 5
	"DIGITE UM NڍERO ENTRE 0 E 2500 (mV)",		//frase 6
	"DIGITE UM NڍERO ENTRE 200 E 60000 (us)",	//frase 7
	"MAXIMO CONFIGURADO MENOR QUE MINIMO",		//frase 8
	"MINIMO CONFIGURADO MAIOR QUE MAXIMO"		//frase 9
};

/*
 * Comandos usados no programa
*/
char comandos[15][33] = {
	"T - Trigger",						//comando 0
	"C - Configurar",					//comando 1
	"M - Medir",						//comando 2
	"L - Lista Din⭩ca",				//comando 3
	"I - Imprime Onda",					//comando 4
	"S - Voltar ao menu inicial",		//comando 5
	"M - Ajusta valor mḩmo",			//comando 6
	"B - Ajusta valor m�mo",			//comando 7
	"T - Ajusta per�o de amostragem",	//comando 8
	"1 - Valor Mḩmo",					//comando 9
	"2 - Valor M�mo",					//comando 10
	"3 - Pico a pico",					//comando 11
	"4 - Valor M餩o",					//comando 12
	"5 - Valor RMS"						//comando 13
};

/*
 * Medidas usadas no programa
*/
char med_string[5][15] = {
	"Mḩmo = ",                        //medida 0
	"M�mo = ",                        //medida 0
	"Pico a Pico = ",                   //medida 0
	"M餩a = ",                         //medida 0
	"RMS = "                            //medida 0
};

/*
 * Enumera&ccedil&atilde;o do tipo de string
*/
typedef enum _tipo_string {
	FRASE,
	COMANDO
} tipo_string;

/*!
 * \fn ConvUIpraDec(unsigned int j, char * s)
 * \brief Converte em ASCII o inteiro sem sinal j.
 * \param[in]  j valor inteiro sem sinal
 * \param[out] s representa&ccedil;&atilde;o em ASCII
 */
void ConvUIpraDec (unsigned int j, char * s) {
	short k = 0;
	char aux[11];
	/*!
	 * Passo 1: extrai os algarismos decimais
	 */
	if (j == 0) { ///< Caso especial
		aux[k] = 0;
		k++;
	}
	while (j) {
		aux[k] = j % 10;
		j = j / 10;
		k++;
	}
	/*!
	 * Passo 2: representa em ASCII os d&iacute;gitos decimais
	 */
	while (k > 0) {
		*s = (aux[k-1] + 0x30);
		s++;                    ///< incrementa o endere&ccedil;o por unidade do tipo de dados
		k--;
	}
	*s = 0;
	
}

/*!
 * \fn ConvFpraString 
 * \brief converte Float para String
 */
void ConvFpraString (float j, char * s) {

	if(j<1.0){
		*s = '0';
		s++;
	}
	int num = (int) (j*10);
	
	ConvUIpraDec(num,s);
	
	while(*s) s++;
	
	*s = *(s-1);
	*(s-1) = '.';
	*(s+1) = '\0';
}

/*!
 * \fn imprime (uint8_t j, tipo_string s)
 * \brief imprime frases e comandos.
 */
void imprime(uint8_t j, tipo_string s){
	int i=0;
	
	switch(s){
	
	case FRASE:
		while(frases[j][i]){
			while(!insere_item(TRANSMISSOR,frases[j][i]));
			i++;
		}
		while(!insere_item(TRANSMISSOR,'\r'));
		while(!insere_item(TRANSMISSOR,'\n'));
		break;
	
	case COMANDO:
		while(comandos[j][i]){
			while(!insere_item(TRANSMISSOR,comandos[j][i]));
			i++;
		}
		while(!insere_item(TRANSMISSOR,'\r'));
		while(!insere_item(TRANSMISSOR,'\n'));
		break;
	}
}

/*!
 * \fn onda_estatica ()
 * \brief imprime forma de onda do sinal.
 */
void onda_estatica(){
	
	int i=0, j=0;
	float step = (config[2] - config[1])/LINHAS;
	
	for(i=LINHAS-1;i>=0;i--){
					
		while(!insere_item(TRANSMISSOR,'|'));
		
		for(j=0;j<COLUNAS;j++){
			if((data[j*SIZEVEC/COLUNAS]>=step*i+config[1])&&(data[j*SIZEVEC/COLUNAS]<=step*(i+1)+config[1])){
				while(!insere_item(TRANSMISSOR,'*'));
			} else{
				while(!insere_item(TRANSMISSOR,' '));
			}
		}
		
		while(!insere_item(TRANSMISSOR,'\r'));
		while(!insere_item(TRANSMISSOR,'\n'));
	}
	
	while(!insere_item(TRANSMISSOR,'|'));
	
	for(j=0;j<COLUNAS;j++){
		while(!insere_item(TRANSMISSOR,'-'));
	}
	
	while(!insere_item(TRANSMISSOR,'\r'));
	while(!insere_item(TRANSMISSOR,'\n'));
	
}

/*!
 * \fn print_medida(float medida, uint8_t j)
 * \brief imprime as frases da rotina medida
 */
void print_medida(float medida, uint8_t j){
	int i = 0;	
	
	while(med_string[j][i]){
		while(!insere_item(TRANSMISSOR,med_string[j][i]));
		i++;
	}

	ConvFpraString(medida,strg);
	
	for(i=0;strg[i]!='\0';i++){
		while(!insere_item(TRANSMISSOR, strg[i]));
	}
	
	while(!insere_item(TRANSMISSOR, 'm'));
	while(!insere_item(TRANSMISSOR, 'V'));
	while(!insere_item(TRANSMISSOR, '\r'));
	while(!insere_item(TRANSMISSOR, '\n'));
}

/*!
 * \fn medidas()
 * \brief seleciona qual medida serᠲealizada
 */
void medidas(){
    char tipo = 'H';
    
    while(tipo != 'S'){  	
		tipo = 'H';
		imprime(1,FRASE);
		imprime(9,COMANDO);
		imprime(10,COMANDO);
		imprime(11,COMANDO);
		imprime(12,COMANDO);
		imprime(13,COMANDO);
		imprime(5,COMANDO);
		
		while(buffer_receptor.head==buffer_receptor.tail);   //buffer estiver vazio
		tipo = remove_item(RECEPTOR);

		switch (tipo){
		
			case 'H': 
				break;
				
			case 'S': 			
				break;
				
			case '1':       // maximo
				
				print_medida(maximo (), 0);
				
				break;
			case '2':       // minimo   
				
				print_medida(minimo (), 1);
				break;
				
			case '3':       // pico a pico
				
				print_medida(pico_pico (), 2);
				
				break;
			case '4':       // medio
			
				print_medida(medio(), 3);
				
				break;
			case '5':       // eficaz     
				
				print_medida(eficaz(), 4);
			
				break;
			 default:        //Comando invalido!
				imprime(4,FRASE);
		}
    }
}

/*!
 * \fn lista(void) 
 * \brief imprime valores em tempo real do sinal medido
 */
void lista(void){
	char tipo='H';
	float conv=0;
	int i=0;
	
	while(tipo!='S'){
	
		ADC0_SC1A	&= (uint32_t)~(uint32_t)(ADC_SC1_ADCH(0x1f)); ///< zera o campo ADCH
		ADC0_SC1A	|= ADC_SC1_ADCH(0x08);            ///< AD8 selecionado (PTB0)
		
		conv = ((float)(value)*(VREFH-VREFL))/MAX;
		ConvFpraString(conv,strg);
		
		for(i=0;strg[i]!='\0';i++){
			while(!insere_item(TRANSMISSOR, strg[i]));
		}
		
		while(!insere_item(TRANSMISSOR, '\r'));
		while(!insere_item(TRANSMISSOR, '\n'));
		
		if(buffer_receptor.head!=buffer_receptor.tail){ //buffer n㯠estiver vazio
			tipo = remove_item(RECEPTOR);
		}
	}
	
}

/*!
 * \fn le_numero(int min, int max)
 * \brief verifica se valores de amplitude est㯠dentro da faixa aceita
 */
int le_numero(int min, int max){

	int num = 0;
	char caractere = '\0';
	
	while(caractere != '\r'){
		if(buffer_receptor.head!=buffer_receptor.tail){	//Se o buffer do receptor n㯠estiver vazio
			caractere = remove_item(RECEPTOR);
			if((caractere >= '0') && (caractere <='9')){
				while(!insere_item(TRANSMISSOR, caractere)); //escreve o ultimo caractere digitado
				num = num*10;
				num = num + (caractere-'0');
			}
			else{
				remove_item(RECEPTOR); //Se n㯠for um numero remove do buffer
			}
		}
	}
	
	while(!insere_item(TRANSMISSOR, caractere));
	while(!insere_item(TRANSMISSOR, '\n'));
	
	if((num<min) || (num>max)){
		return 0xffff;
	}
	else return num;
}

/*!
 * \fn configura(void)
 * \brief configura escala de amplitude e escala de tempo
 */
void configura(void){
	char tipo = 'H';
	int num = 0;
	
	while(tipo != 'S'){
		tipo = 'H';
		
		imprime(1,FRASE);
		imprime(6,COMANDO);
		imprime(7,COMANDO);
		imprime(8,COMANDO);
		imprime(5,COMANDO);
	
		while(buffer_receptor.head==buffer_receptor.tail);   //buffer estiver vazio
		tipo = remove_item(RECEPTOR);
		
		switch(tipo){
		case 'H':
			break;
		case 'S':
			break;
		case 'M':
			imprime(6,FRASE);
			
			num = le_numero(0,2500);
			if(num == 0xffff){
				imprime(5,FRASE);
			}
			else if(num <= config[1]){
				imprime(8,FRASE);
			}
			else{
				config[2] = num;
			}
			break;
		
		case 'B':
			imprime(6,FRASE);
			
			num = le_numero(0,2500);
			if(num == 0xffff){
				imprime(5,FRASE);
			}
			else if(num >= config[2]){
				imprime(9,FRASE);
			}
			else{
				config[1] = num;
			}			
			break;
			
		case 'T':
			imprime(7,FRASE);
			
			num = le_numero(200,60000);
			if(num == 0xffff){
				imprime(5,FRASE);
			}
			else{
				config[0] = num;
			}			
			break;
			
		default: 
			imprime(4,FRASE);
		}
	}
}

/*!
 * \fn main 
 * \brief executa o fluxo do programa: Lista de comandos do oscilosc󰩯 digital
 */
int main(void){
	
	read = 0;
	config[0] = 200; // [0]= intervalo de tempo entre amostragens, [1]= amplitude minima, [2]= amplitude maxima
	config[1] = 0;
	config[2] = 2000;
	value = 0;
	int i = 0;
	int j;
	
    char command='H';

	//Inicializa磯 da UART, ADC, NVIC, SysTick
	initUART();
	calibraADC();   //calibra磯 do ADC
	configuraADC();
	enableNVIC();
	InitSysTick();
	
	//inicializa磯 buffer circular
	buffer_receptor.head=0;
	buffer_receptor.tail=0;
    buffer_transmissor.head=0;
    buffer_transmissor.tail=0;
    
    //Inicia o vetor de dados como 0
    for(i=0;i<SIZEVEC;i++){
    	data[i] = 0.0;
    }
    
    //Mensagens Iniciais 
	for (j=0; j<6; j++){
		for (i=0; i<150; i++){
			while(!insere_item(TRANSMISSOR, titulo[j][i]));
		}
		while(!insere_item(TRANSMISSOR,'\r'));
		while(!insere_item(TRANSMISSOR,'\n'));	
	}  
	while(!insere_item(TRANSMISSOR,'\r'));
	while(!insere_item(TRANSMISSOR,'\n'));

    
    //Loop do programa
	for(;;){
		imprime(1,FRASE);	    //"DIGITE UM COMANDO"
		for(i=0;i<6;i++){
			imprime(i,COMANDO); //Lista de comandos
		}
		
		command = 'H';
		
	    while(buffer_receptor.head==buffer_receptor.tail);   //buffer estiver vazio
		command = remove_item(RECEPTOR);
			
		switch(command){
		case 'H':
			break;
		case 'S':
			//command = 'H';
			break;
			
		case 'T':                                        //Salva 1000 amostras do sinal
			imprime(2,FRASE);
			
			SYST_RVR = SysTick_RVR_RELOAD(21*config[0]);
			SYST_CVR = SysTick_CVR_CURRENT(1);	        //Zera o Contador
			SYST_CSR |= SysTick_CSR_ENABLE_MASK;	    //Habilita o Systick
			
			for(i=0;i<SIZEVEC;i++){
				while(!(SYST_CSR&SysTick_CSR_COUNTFLAG_MASK));
				ADC0_SC1A	&= (uint32_t)~(uint32_t)(ADC_SC1_ADCH(0x1f)); ///< zera o campo ADCH
				ADC0_SC1A	|= ADC_SC1_ADCH(0x08);                          ///< AD8 selecionado (PTB0)
				while(!read);
				read = 0;
				data[i]=((float)(value)*(VREFH-VREFL))/MAX;
			}
			
			SYST_CSR &= ~SysTick_CSR_ENABLE_MASK;	//Desabilita o Systick
			imprime(3,FRASE);		
			
			break;
		
		case 'C':                                          //Configura磯 das escalas
			configura();
			break;
			
		case 'I':                                         //Imprime forma de onda 
			onda_estatica();
			break;
			
		case 'L':                                        //Imprime lista de valores 
			lista();
			break;
		
		case 'M':                                        //Imprime medidas
		    medidas(); 
		    break;        
		default:                                         //Comando invalido!
			imprime(4,FRASE);
		}
	}
	return 1;
}
