Datasheet:
* [nRF24L01]
* [nRF24L01+]

Problemas
---
Conferir se o nosso chip � o nRF24L01 ou o nRF24L01+.

---
No SPI do r�dio, o LSByte � enviado primeiro. Verificar se esse envio est� ocorrendo de forma correta, pois no c�digo todos os arrays de bytes est�o com o MSByte primeiro. Isso afeta a

---
As flags de interrup��o no registrador `STATUS` est�o sendo limpadas ap�s sua leitura?

---
Logo no in�cio do construtor de `Radio`:
```c++
  if(talker_cnt >= 5)
    return;
```
Isso faz com que n�o seja adicionado mais nenhum listener puro se a quantidade m�xima de talkers tiver sido atingida. Adicionar um `!listener &&`.

---
Problema do ponto de cria��o do objeto:
- Se for criado no `setup`, n�o pode ser usado no `loop`
- N�o faz sentido criar o objeto no `loop` pois o construtor seria chamado m�ltiplas vezes
- Se for criado no namespace global, o construtor ser� chamado antes da fun��o `Radio::config`

Os construtores de todas as classes do r�dio chamam a fun��o `Radio::requestAdress` para saber a qual pipe o interlocutor (PRX) deseja ser endre�ado. Para que o PRX responda, a mensagem enviada a ele pedindo essa informa��o cont�m o endere�o do presente r�dio (PTX), que s� � definido na fun��o config. Portanto, h� duas solu��es:
- Definir uma fun��o � parte para atuar como construtor, em vez de configurar o r�dio assim que o objeto � declarado. Dessa forma, a declara��o n�o conter� argumentos e construir� um objeto vazio, e tal fun��o ser� chamada no `setup` ap�s `config`.
- N�o incluir o endere�o na mensagem enviada por `requestAdress`. Ao inv�s disso, sinzalizar para que o PRX envie a mens�gem de resposta para o seu pr�prio endere�o, e preencher o endere�o do pipe 0 de PTX com esse endere�o, de forma semelhante ao _auto acknowledgement_.

Sugest�es
---
Adicionar recursos:
* Fun��o para informar o valor de `PLOS_CNT` (contagem de falhas de envio de pacote).
* Fun��o para definir o valor da frequ�ncia (tomar cuidado com a separa��o de 2 MHz necess�ria para a taxa de 2 Mbps)

---
O c�digo apresenta v�rias viola��es da regra do strict aliasing, ex. coisas do tipo `*(uint32_t*) a = n`, onde `a` � um vetor de 4 `uint8_t`. Elas devem ser substitu�das por `union`s.

---
Atribuir valores diferentes para os bytes de controle, os escolhidos possuem codifica��es bin�rias muito semelhantes para significados completamente opostos (como > e <);


[nRF24L01]: https://www.nordicsemi.com/eng/content/download/2730/34105/file/nRF24L01_Product_Specification_v2_0.pdf
[nRF24L01+]: https://www.nordicsemi.com/eng/content/download/2726/34069/file/nRF24L01P_Product_Specification_1_0.pdf