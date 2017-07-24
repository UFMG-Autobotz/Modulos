Datasheet:
* [nRF24L01]
* [nRF24L01+]

Problemas
---
Conferir se o nosso chip é o nRF24L01 ou o nRF24L01+.

---
No SPI do rádio, o LSByte é enviado primeiro. Verificar se esse envio está ocorrendo de forma correta, pois no código todos os arrays de bytes estão com o MSByte primeiro. Isso afeta a

---
As flags de interrupção no registrador `STATUS` estão sendo limpadas após sua leitura?

---
Logo no início do construtor de `Radio`:
```c++
  if(talker_cnt >= 5)
    return;
```
Isso faz com que não seja adicionado mais nenhum listener puro se a quantidade máxima de talkers tiver sido atingida. Adicionar um `!listener &&`.

---
Problema do ponto de criação do objeto:
- Se for criado no `setup`, não pode ser usado no `loop`
- Não faz sentido criar o objeto no `loop` pois o construtor seria chamado múltiplas vezes
- Se for criado no namespace global, o construtor será chamado antes da função `Radio::config`

Os construtores de todas as classes do rádio chamam a função `Radio::requestAdress` para saber a qual pipe o interlocutor (PRX) deseja ser endreçado. Para que o PRX responda, a mensagem enviada a ele pedindo essa informação contém o endereço do presente rádio (PTX), que só é definido na função config. Portanto, há duas soluções:
- Definir uma função à parte para atuar como construtor, em vez de configurar o rádio assim que o objeto é declarado. Dessa forma, a declaração não conterá argumentos e construirá um objeto vazio, e tal função será chamada no `setup` após `config`.
- Não incluir o endereço na mensagem enviada por `requestAdress`. Ao invés disso, sinzalizar para que o PRX envie a menságem de resposta para o seu próprio endereço, e preencher o endereço do pipe 0 de PTX com esse endereço, de forma semelhante ao _auto acknowledgement_.

Sugestões
---
Adicionar recursos:
* Função para informar o valor de `PLOS_CNT` (contagem de falhas de envio de pacote).
* Função para definir o valor da frequência (tomar cuidado com a separação de 2 MHz necessária para a taxa de 2 Mbps)

---
O código apresenta várias violações da regra do strict aliasing, ex. coisas do tipo `*(uint32_t*) a = n`, onde `a` é um vetor de 4 `uint8_t`. Elas devem ser substituídas por `union`s.

---
Atribuir valores diferentes para os bytes de controle, os escolhidos possuem codificações binárias muito semelhantes para significados completamente opostos (como > e <);


[nRF24L01]: https://www.nordicsemi.com/eng/content/download/2730/34105/file/nRF24L01_Product_Specification_v2_0.pdf
[nRF24L01+]: https://www.nordicsemi.com/eng/content/download/2726/34069/file/nRF24L01P_Product_Specification_1_0.pdf