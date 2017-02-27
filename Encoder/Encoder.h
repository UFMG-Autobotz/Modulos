/* Encoder.h
 * Versão 0.1
 *
 * Classe para leitura de encoders rotativos para medição de posição e velocidade
 * A versão atual (0.1) vale apenas para encoders de um canal
 *
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

// NOTA: NÃO FUNCIONA BEM PARA BAIXAS VELOCIDADES! É NECESSÁRIO TESTAR O COMPORTAMENTO E MELHORAR O ALGORITMO
// MOTIVO: a disciplina de controle não cobre tudo que eu precisava pra essa aplicação :(
// CONSOLO: estou pesquisando soluções

#ifndef Encoder_h
#define Encoder_h

class Encoder
{
public:

  Encoder();
  Encoder(int pino, int cpr, float freq = 20);
  void config(int pino, int cpr, float freq = 20);
  
  void freqCorte(float freq);
  float freqCorte();

  float posicao();
  float velocidade();
  float aceleracao();

  void incrementando();
  void decrementando();

private:

  float delta_ang;

  bool incr;        // True se incrementando, false se decrementando
  bool primeiro_pulso;
  float freq_corte;

  volatile int marca_atual;
  volatile float vel[2], acel;
  volatile unsigned long tempo[2];

  static void pulso(Encoder*);
};

#endif
