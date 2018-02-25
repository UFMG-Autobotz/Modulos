/* 
 * File:    main.c
 *
 * Authors: Bianca Martins e Daniel Leite
 *          Autobotz UFMG
 */

#include <htc.h>        // Include do compilador
#include <pic16f676.h>  // Include do PIC

// Palavra de configura��o
__CONFIG(FOSC_INTRCIO &  PWRTE_OFF & BOREN_ON & MCLRE_OFF & WDTE_OFF);

#define _XTAL_FREQ 4000000 // Frequ�ncia do cristal

/* Valores de refer�ncia para o limite m�nimo por c�lula.
 * Dois limiares s�o usados, um conservador (3,75 V) e um "relaxado" (3,5 V).
 * A sa�da do conversor A/D � um inteiro de 0 a 1023 correspondendo a uma
 * tens�o de 0 a 5 V. Logo, as sa�das correspondene �s tens�es limiares s�o:
 * Sa�da A/D m�nima = Tens�o limiar * Divisor de tens�o resistivo * Bits por volt
 */
#define REF1 313  // 3,75 * 0,407 * 1024 / 5
#define REF2 292  // 3,5  * 0,407 * 1024 / 5

/* NOTA PARA VERS�O 3: Esses valores est�o muito conservadores. A c�lula aguenta
 * cair at� 3,0 V, embora n�o seja prudente deixar chegar a esse valor. Bons
 * limites seriam de 3,5 V para o conservador e 3,2 V para o relaxado.
 * OBS: O uso de dois limiares diferentes foi ideia do Blanc. A ideia era usar
 * conector de dois pinos e, caso um jumper fosse conectado entre eles, o c�digo
 * selecionaria o limiar "relaxado", enquanto se n�o houvesse jumper seriam
 * usados os valores conservadores. Assim era poss�vel continuar a operar open
 * rob� por mais um tempo ap�s um desligamento por subtens�o, bastando para isso
 * simplesmente conectar o jumper e ele religaria (com o novo limiar relaxado).
 * Esse esquema n�o foi inclu�do no esquem�tico e layout da placa vers�o 3, mas
 * pode ser facilmente inclu�do de volta caso se julgue interessante/necess�rio.
 */
 
#include <stdlib.h>

int main(int argc, char** argv)
{
   int celula1, celula2, celula3;      // Tens�o em cada c�lula
   int leitura1, leitura2, leitura3;   // Leitura de cada canal do ADC
   int cont = 0, seguro;
    
   /* Registradores TRIS: Configuram os pinos como sa�da (0) ou entrada (1).
    * Registradores PORT: Define a tens�o de sa�da do pino (caso ele seja sa�da)
    * ou armazena a leitura da tens�o no pino (caso seja entrada digital).
    * Registradores ANSEL: Define pinos como digital (0) ou anal�gico (1).
    */
   
   TRISC = 0;  // Define todos os pinos do PORTC como sa�da
   PORTC = 0;  // Coloca todas as sa�das do PORTC em  0 V
   
   TRISAbits.TRISA2 = 1;   // C�lula superior (RA2/AN2) - Definir como entrada
   TRISAbits.TRISA4 = 1;   // C�lula intermedi�ria (RA4/AN3) - Definir como entrada
   TRISCbits.TRISC0 = 1;   // C�lula inferior (RC0/AN4) - Definir como entrada
   
   TRISAbits.TRISA5 = 1;   // Jumper de sele��o de refer�ncia (RA5) - Definir como entrada
   
   ANSELbits.ANS2 = 1;  // C�lula superior (RA2/AN2) - Desabilita entrada digital 
   ANSELbits.ANS3 = 1;  // C�lula intermedi�ria (RA4/AN3) - Desabilita entrada diigtal 
   ANSELbits.ANS4 = 1;  // C�lula inferior (RC0/AN4) - Desabilita entrada diigtal
   
   /* CMCON: Comparator module configuration register */
   
   CMCONbits.CM = 0b111;   // Desliga m�dulo de comparador (desconecta pino RA2 do m�dulo)
                           // Necess�rio ao se utilizar RA2 como entrada anal�gica
   
   /* ADCON0 e ADCON1: ADC configuration registers 0 e 1, respectivamente */
   
   ADCON1bits.ADCS = 0b100;   // ADC clock select: Define o clock do ADC como Fosc/4
   
   ADCON0bits.ADFM = 1; // ADC format: Alinhar resultado da convers�o � direita
   ADCON0bits.VCFG = 0; // Voltage config: Seleciona a tens�o m�x. de refer�ncia como sendo a tens�o de alimenta��o
   ADCON0bits.ADON = 1; // ADC on: Liga o perif�rico de convers�o
    
   ADCON0bits.CHS = 0b010; // Channel select: Seleciona o canal 2 para convers�o (RA2/AN2 - c�lula superior)
    
    while(1)
   {    
      ADCON0bits.GO = 1;         // Inicia a convers�o
         while(ADCON0bits.GO);   // Espera o fim da convers�o (o bit GO fica em 1 at� a convers�o ser conclu�da, depois vai para 0)
        
      /* A vari�vel "cont" representa o canal da �ltima convers�o realizada
       * Canal 2: cont = 0
       * Canal 3: cont = 1
       * Canal 4: cont = 2
       */     
      
      switch(cont)   // Ap�s o fim da convers�o, verifica em qual canal ela foi realizada
      {
      /* O resultado da convers�o AD fica armazenada nos registradores ADRES (ADC result):
       * ADRESH armazena o byte superior e ADRESL, o byte inferior.
       */

      case 0:                 // Fim da convers�o no canal 2
         leitura1 = (ADRESH << 8) | ADRESL;
         ADCON0bits.CHS = 0b011; // Seleciona o canal 3 para pr�x. convers�o (RA4/AN3 - c�lula intermedi�ria)
         cont = 1;
         break;

      case 1:                 // Fim da convers�o no canal 3
         leitura2 = (ADRESH << 8) | ADRESL;
         ADCON0bits.CHS = 0b100; // Seleciona o canal 4 para pr�x. convers�o (RC0/AN4 - c�lula inferior)
         cont = 2;
         break;
          
      case 2:                 // Fim da convers�o no canal 4
         leitura3 = (ADRESH << 8) | ADRESL;
         ADCON0bits.CHS = 0b010; // Seleciona o canal 2 para pr�x. convers�o (RA2/AN2 - c�lula superior)
         cont = 0;
         break;
      }

      // Isola a tens�o individual de cada c�lula
      celula1 = leitura1 - leitura2;
      celula2 = leitura2 - leitura3;
      celula3 = leitura3;
        
      if(PORTAbits.RA5 == 1)  // Jumper de sele��o de refer�ncia desconectado
         seguro = (celula1 > REF1) && (celula2 > REF1) && (celula3 > REF1);
      else
         seguro = (celula1 > REF2) && (celula2 > REF2) && (celula3 > REF2);
      
      /* O transitor conectado a RC1 corta a sa�da se a base estiver em n�vel alto, e a mant�m ativa se em 
       * n�vel baixo. O LED acende se a sa�da estiver ativa e apaga se for cortada.
       */

      PORTCbits.RC1 = !seguro;   // RC1 (transistor de sa�da) em n�vel baixo se for seguro, alto se n�o for.
      PortCbits.RC2 = seguro;    // RC2 (LED) aceso se for seguro;
      
      /* NOTA PARA A VERS�O 3:
       * - Adicionar mais um canal do ADC para a leitura da corrente;
       * - Adicionar condi��o de sobrecorrente para corte da sa�da;
       * - Adicionar condi��o de desbalanceamento entre as c�lulas para corte da sa�da;
       * - Ao cortar a sa�da, mant�-la cortada at� que se pressione o bot�o de reset;
       * - Mudar comportamento do LED
       * - Inverter comportamento do transistor de sa�da (ver arquivo "LiPo - Coment�rios", se��o Ideias)
       */
   }
    
   return (EXIT_SUCCESS);
}