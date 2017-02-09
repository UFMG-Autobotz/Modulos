#Interrompe.h

Interface para utilização de interrupções no Arduino Uno.
Funcionalidades:
* Interrupções por mudança de nível em qualuqer pino digital (personalizáveis)
* Agendar funções para serem executadas após o esgotamento de um timer

Documentação [aqui](https://drive.google.com/file/d/0B69EjPAzN1_cbTVIUTJTZDY5Zk0) (atualmente está desatualizada).

Para instalar, mova esta pasta para o diretório de bibliotecas do seu computador.
- `~/Arduino/libraries` (Linux)
- `(...)\Documentos\Arduino\Libraries` (Windows)
É necessário também baixar a biblioteca [MsTimer2](https://github.com/PaulStoffregen/MsTimer2) e adicioná-la ao diretório de bibliotecas.

Por fim, basta incluir `#include <Interrompe.h>` em seu sketch.
