/* Encoder.h
 * Versão 0.1
 *
 * Classe para leitura de encoders rotativos para medição de posição e velocidade
 *
 * Autor: Daniel Leite Ribeiro
 *        Autobotz UFMG
 */

#include <Arduino.h>
#include <Interrompe.h>
#include "Encoder.h"

Encoder::Encoder():
  marca_atual(0),
  vel{0., 0.},
  acel(0),
  tempo{0, 0},
  primeiro_pulso(true),
  incr(true)
  {}

Encoder::Encoder(int pino, int cpr, float freq = 20) : Encoder()
{
  config(pino,cpr,freq);
}

void Encoder::config(int pino, int cpr, float freq = 20)
{
  delta_ang = 360/cpr;
  freq_corte = freq;

  pinMode(pino, INPUT_PULLUP);

  Interrompe::habilita(pino,CHANGE,pulso,this);
}

void Encoder::freqCorte(float freq)
{
  freq_corte = freq;
}

float Encoder::freqCorte()
{
  return freq_corte;
}

float Encoder::posicao()
{
  return marca_atual;
}

float Encoder::velocidade()
{
  return vel[0];
}

float Encoder::aceleracao()
{
  return acel;
}

void Encoder::incrementando()
{
  incr = true;
}

void Encoder::decrementando()
{
  incr = false;
}

void Encoder::pulso(Encoder* e)
{
  if(e->incr)
    e->marca_atual++;
  else
    e->marca_atual--;

  e->tempo[1] = e->tempo[0];
  e->tempo[0] = micros();

  if(!e->primeiro_pulso)
  {
    // Transformada Z
    
    e->vel[1] = e->vel[0];
    e->vel[0] = (e->vel[1] + e->freq_corte * e->delta_ang) / (1 + e->freq_corte * (e->tempo[0] - e->tempo[1]));

    e->acel = (e->acel + e->freq_corte * (e->vel[0] - e->vel[1])) / (1 + e->freq_corte * (e->tempo[0] - e->tempo[1]));
  }
  else
    e->primeiro_pulso = false;
}
