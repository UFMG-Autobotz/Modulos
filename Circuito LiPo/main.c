/* 
 * File:    main.c
 *
 * Authors: Bianca Martins e Daniel Leite
 *          Autobotz UFMG
 */

#include <htc.h>        // Include do compilador
#include <pic16f676.h>  // Include do PIC

// Palavra de configuração
__CONFIG(FOSC_INTRCIO &  PWRTE_OFF & BOREN_ON & MCLRE_OFF & WDTE_OFF);

#define _XTAL_FREQ 4000000 // Frequência do cristal

/* Valores de referência para o limite mínimo por célula.
 * Dois limiares são usados, um conservador (3,75 V) e um "relaxado" (3,5 V).
 * A saída do conversor A/D é um inteiro de 0 a 1023 correspondendo a uma
 * tensão de 0 a 5 V. Logo, as saídas correspondene às tensões limiares são:
 * Saída A/D mínima = Tensão limiar * Divisor de tensão resistivo * Bits por volt
 */
#define REF1 313  // 3,75 * 0,407 * 1024 / 5
#define REF2 292  // 3,5  * 0,407 * 1024 / 5

/* NOTA PARA VERSÃO 3: Esses valores estão muito conservadores. A célula aguenta
 * cair até 3,0 V, embora não seja prudente deixar chegar a esse valor. Bons
 * limites seriam de 3,5 V para o conservador e 3,2 V para o relaxado.
 * OBS: O uso de dois limiares diferentes foi ideia do Blanc. A ideia era usar
 * conector de dois pinos e, caso um jumper fosse conectado entre eles, o código
 * selecionaria o limiar "relaxado", enquanto se não houvesse jumper seriam
 * usados os valores conservadores. Assim era possível continuar a operar open
 * robô por mais um tempo após um desligamento por subtensão, bastando para isso
 * simplesmente conectar o jumper e ele religaria (com o novo limiar relaxado).
 * Esse esquema não foi incluído no esquemático e layout da placa versão 3, mas
 * pode ser facilmente incluído de volta caso se julgue interessante/necessário.
 */
 
#include <stdlib.h>

int main(int argc, char** argv)
{
   int celula1, celula2, celula3;      // Tensão em cada célula
   int leitura1, leitura2, leitura3;   // Leitura de cada canal do ADC
   int cont = 0, seguro;
    
   /* Registradores TRIS: Configuram os pinos como saída (0) ou entrada (1).
    * Registradores PORT: Define a tensão de saída do pino (caso ele seja saída)
    * ou armazena a leitura da tensão no pino (caso seja entrada digital).
    * Registradores ANSEL: Define pinos como digital (0) ou analógico (1).
    */
   
   TRISC = 0;  // Define todos os pinos do PORTC como saída
   PORTC = 0;  // Coloca todas as saídas do PORTC em  0 V
   
   TRISAbits.TRISA2 = 1;   // Célula superior (RA2/AN2) - Definir como entrada
   TRISAbits.TRISA4 = 1;   // Célula intermediária (RA4/AN3) - Definir como entrada
   TRISCbits.TRISC0 = 1;   // Célula inferior (RC0/AN4) - Definir como entrada
   
   TRISAbits.TRISA5 = 1;   // Jumper de seleção de referência (RA5) - Definir como entrada
   
   ANSELbits.ANS2 = 1;  // Célula superior (RA2/AN2) - Desabilita entrada digital 
   ANSELbits.ANS3 = 1;  // Célula intermediária (RA4/AN3) - Desabilita entrada diigtal 
   ANSELbits.ANS4 = 1;  // Célula inferior (RC0/AN4) - Desabilita entrada diigtal
   
   /* CMCON: Comparator module configuration register */
   
   CMCONbits.CM = 0b111;   // Desliga módulo de comparador (desconecta pino RA2 do módulo)
                           // Necessário ao se utilizar RA2 como entrada analógica
   
   /* ADCON0 e ADCON1: ADC configuration registers 0 e 1, respectivamente */
   
   ADCON1bits.ADCS = 0b100;   // ADC clock select: Define o clock do ADC como Fosc/4
   
   ADCON0bits.ADFM = 1; // ADC format: Alinhar resultado da conversão à direita
   ADCON0bits.VCFG = 0; // Voltage config: Seleciona a tensão máx. de referência como sendo a tensão de alimentação
   ADCON0bits.ADON = 1; // ADC on: Liga o periférico de conversão
    
   ADCON0bits.CHS = 0b010; // Channel select: Seleciona o canal 2 para conversão (RA2/AN2 - célula superior)
    
    while(1)
   {    
      ADCON0bits.GO = 1;         // Inicia a conversão
         while(ADCON0bits.GO);   // Espera o fim da conversão (o bit GO fica em 1 até a conversão ser concluída, depois vai para 0)
        
      /* A variável "cont" representa o canal da última conversão realizada
       * Canal 2: cont = 0
       * Canal 3: cont = 1
       * Canal 4: cont = 2
       */     
      
      switch(cont)   // Após o fim da conversão, verifica em qual canal ela foi realizada
      {
      /* O resultado da conversão AD fica armazenada nos registradores ADRES (ADC result):
       * ADRESH armazena o byte superior e ADRESL, o byte inferior.
       */

      case 0:                 // Fim da conversão no canal 2
         leitura1 = (ADRESH << 8) | ADRESL;
         ADCON0bits.CHS = 0b011; // Seleciona o canal 3 para próx. conversão (RA4/AN3 - célula intermediária)
         cont = 1;
         break;

      case 1:                 // Fim da conversão no canal 3
         leitura2 = (ADRESH << 8) | ADRESL;
         ADCON0bits.CHS = 0b100; // Seleciona o canal 4 para próx. conversão (RC0/AN4 - célula inferior)
         cont = 2;
         break;
          
      case 2:                 // Fim da conversão no canal 4
         leitura3 = (ADRESH << 8) | ADRESL;
         ADCON0bits.CHS = 0b010; // Seleciona o canal 2 para próx. conversão (RA2/AN2 - célula superior)
         cont = 0;
         break;
      }

      // Isola a tensão individual de cada célula
      celula1 = leitura1 - leitura2;
      celula2 = leitura2 - leitura3;
      celula3 = leitura3;
        
      if(PORTAbits.RA5 == 1)  // Jumper de seleção de referência desconectado
         seguro = (celula1 > REF1) && (celula2 > REF1) && (celula3 > REF1);
      else
         seguro = (celula1 > REF2) && (celula2 > REF2) && (celula3 > REF2);
      
      /* O transitor conectado a RC1 corta a saída se a base estiver em nível alto, e a mantém ativa se em 
       * nível baixo. O LED acende se a saída estiver ativa e apaga se for cortada.
       */

      PORTCbits.RC1 = !seguro;   // RC1 (transistor de saída) em nível baixo se for seguro, alto se não for.
      PortCbits.RC2 = seguro;    // RC2 (LED) aceso se for seguro;
      
      /* NOTA PARA A VERSÃO 3:
       * - Adicionar mais um canal do ADC para a leitura da corrente;
       * - Adicionar condição de sobrecorrente para corte da saída;
       * - Adicionar condição de desbalanceamento entre as células para corte da saída;
       * - Ao cortar a saída, mantê-la cortada até que se pressione o botão de reset;
       * - Mudar comportamento do LED
       * - Inverter comportamento do transistor de saída (ver arquivo "LiPo - Comentários", seção Ideias)
       */
   }
    
   return (EXIT_SUCCESS);
}