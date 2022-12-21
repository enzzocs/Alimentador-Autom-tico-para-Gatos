# Alimentador Automático de Gatos
Este repositório tem como objetivo relatar as técnicas utilizadas para o desenvolvimentos do projeto de um alimentador automático de gatos. Cada felino é reconhecido por uma tag específica e possui uma porção pré-determinada de ração, baseada na idade e peso do animal. O projeto foi desenvolvido para a displina de Projeto Integrador III, do curso de Engenharia Eletrônica do IFSC Campus Florianópolis, pelos estudantes Enzzo Comassetto dos Santos e Jade Dutra Lopes.

Como referências para a concepção do projeto, foram utilizadas alguns produtos já existentes no mercado, observados nos links a seguir:

[Link 1](https://www.amazon.com.br/PETODAY-Alimentador-Autom%C3%A1tico-Inteligente-Dispensador/dp/B08THNQ36R/ref=sr_1_8?crid=27I0CH6PFHF4P&keywords=automatic+feeder+cat&qid=1671628260&sprefix=cat+feeder%2Caps%2C192&sr=8-8)

[Link 2](https://www.amazon.com.br/Prote%C3%A7%C3%A3o-Alimentos-Alimentador-Autom%C3%A1tico-C%C3%A2mera/dp/B0BC9765NC/ref=sr_1_9?crid=27I0CH6PFHF4P&keywords=automatic+feeder+cat&qid=1671628260&sprefix=cat+feeder%2Caps%2C192&sr=8-9&ufe=app_do%3Aamzn1.fos.25548f35-0de7-44b3-b28e-0f56f3f96147)

[Link 3](https://www.amazon.com.br/Alimentadores-autom%C3%A1ticos-cronometrados-Dispensador-quantitativo/dp/B09TKS6W6D/ref=sr_1_6?crid=27I0CH6PFHF4P&keywords=automatic+feeder+cat&qid=1671628260&sprefix=cat+feeder%2Caps%2C192&sr=8-6&ufe=app_do%3Aamzn1.fos.6121c6c4-c969-43ae-92f7-cc248fc6181d)

## 1. A estrutura

   Para o alimentador, foi necessário desenvolver duas seções da estrutura: uma para o armazenamento da ração, e outra para suportar o sistema eletrônico. Utilizou-se um cano para a interligação das seções com o pote de ração fixado na estrutura.
   De material, foi utilizado MDF para a construção do protótipo (figura 1) juntamente de um cano PVC tipo T de 50mm (figura 2) utilizado como o dispenser da ração para o pote. O conjunto foi montado conforme a figura 3.

[![Figura 1](https://www.imagemhost.com.br/images/2022/07/26/estrutura-1.th.jpg)](https://www.imagemhost.com.br/image/rTlkOC)
[![Figura 2](https://www.imagemhost.com.br/images/2022/07/26/estrutura-2.th.jpg)](https://www.imagemhost.com.br/image/rTlwlL)
[![Figura 3](https://www.imagemhost.com.br/images/2022/07/26/estrutura-3.th.jpg)](https://www.imagemhost.com.br/image/rTlzaB)

  De acordo com a progressão das atividades, foi preciso fazer algumas alterações físicas na estrutura. O sensor de peso (que será detalhado mais para frente) foi um grande obstáculo para o andamento do projeto. Esse requeriu mudanças no alimentador para que pudesse operar coerentemente. 
  
## 1.1 Hélice

   Para empurrar a ração ao pote, foi feita em uma impressora 3D uma hélice helicoidal, fixada no servo motor. A ração cai sobre ela, e com o auxílio do motor, a quantidade especificada de ração cai sobre o pote. O design do helicóide foi retirado de um [site](https://www.thingiverse.com/thing:27854) com projeto similar, e ajustado para o tamanho do cano PVC usado. A figura 4 mostra as duas hélices que foram impressas, de 13 e 9cm, respectivamente. A de 13cm foi a utilizada no projeto, enquanto a de 9, como foi resultado de uma impressão falha, foi usada como teste para furos para o encaixe do motor.
  
[![Figura 4](https://www.imagemhost.com.br/images/2022/07/26/helicoide-1.md.jpg)](https://www.imagemhost.com.br/image/rTlaZK)

   Durante o andamento do projeto, a hélice quebrou no encontro do helicóide com a base. Para consertar, foi colocado um parafuso, que passava pela base e ia por dentro do helicóiode, prendendo a estrutura. Porém isto fez com que a hélice não ficasse perpendicular à base, dificultando a rotação do motor, uma vez que dependendo do ângulo entre o cano e a hélice, o atrito gerado era maior. Isto ocasionava um maior esforço do motor. Dessa forma, o encaixe do mesmo com o cano teve que ser feita de maneira mais cuidadosa.

### 1.2 Motor

  A fim de empurrar a ração para o pote, foi utilizado um [servo motor SM-S4306R 360º](https://www.filipeflop.com/produto/servo-motor-sm-s4306r-360-graus-acessorios/), sem ser necessário o uso de redução para tal. Ele é alimentado em 5V, e funciona por PWM.
  
### 
  
## 2. O projeto

  O alimentador automático de gatos identifica o gato que se aproxima do leitor RFID por uma determinada tag, onde cada gato possui uma porção de ração específica por dia. O motor libera ração o suficiente para completar a porção do gato da vez (monitorado pelo sensor de peso abaixo do pote) para aquele momento do dia.
  
 ## 2.1 Microcontolador
 
  O microcontrolador utilizado foi o **Atmega328P**, inicialmente acoplado em um Arduino Uno. A plataforma do Arduino foi muito útil para os testes iniciais dos sensores e do motor, possibilitando calibrações e detecções de possíveis erros de leitura. Ao fim dos testes individuais, todos sensores foram conectados juntos ao microcontrolador para a integração deles no código final.
  
   Após todos os sensores serem testados, o código foi adaptado para linguagem C no software Atmel Studio.

 
 ## 2.2 Sensores
 
 ### 2.2.1 Sensor RFID
 
  O sensor **RFID-RC522 13,56MHz** foi utilizado para a identificação de cada felino. Foram utilziadas duas tags diferentes, cada qual com um _id_ diferente para cada gato. Quando a tag é identificada, um sinal é enviado ao motor e este gira a espira até completar a porção necessária.
  
  A ideia original seria o sensor ficar numa posição em frente ao alimentador, sendo capaz de captar a tag do gato. Porém, por ser um protótipo, o sensor ficou em posição de difícil acesso à um gato real, sendo apenas utilizado afim de testes do projeto.
  
 ### 2.2.2 Sensor RTC
 
  O módulo **RTC DS1307** tem a função de controlar o tempo do dia para que o gato não coma mais de 1 porção em determinado período.
  
 ### 2.2.3 Sensor de peso
 
  O sensor de peso com certeza foi o mais complexo para se trabalhar de todo o projeto. Inicialmente, trabalhou-se com uma célula de carga de 50kg juntamente de um **amplificador conversor A/D HX711**. Essa célula de carga funciona de acordo com a deformação que o centro dela sofre de um peso. Para o funcionamento correto da célula, é preciso criar uma base para a mesma, de forma que o centro não fique em contato com a superfície. 
  
  Foi impresso em 3D uma base para a célula de peso, seguindo um [modelo](https://www.thingiverse.com/thing:2624188) achado para o mesmo tipo de célula, de acordo com um [tutorial](https://circuitjournal.com/50kg-load-cells-with-HX711#1x50) para esta aplicação. A figura 5 mostra como ficou a base para o nosso sensor.
  
[![Figura 5](https://www.imagemhost.com.br/images/2022/07/27/base-cel.md.jpg)](https://www.imagemhost.com.br/image/rTBF9Y)

   Entretando, apesar de ter a estrutura para a célula, o sensor continuava a apresentar resultados extremamentes imprecisos, e não conseguia fixar em uma faixa específica de valores. Pesquisando mais a fundo, foram encontrados vários exemplos utilizando não apenas 1, mas 4 dessas células para formar uma balança, o que dificultaria mais ainda criar uma estrutura estável para medir poucas gramas. Dado este enorme problema, a equipe optou por comprar uma balança de peso de alimentos, desmontar a mesma e utilizar a célula de carga + a base desta, como mostra a figura 6. A célula em questão é apresentada na figura 7, é de formato diferente da original e suporta até 10kg. 
   
   Foi então calibrado o sensor, utilizando pesos já conhecidos até obtermos um indice de calibração aceitável.

[![Figura 6](https://www.imagemhost.com.br/images/2022/07/27/sensor-peso-2.md.jpg)](https://www.imagemhost.com.br/image/rTB4zI)
[![Figura 7](https://www.imagemhost.com.br/images/2022/07/27/celula-carga.md.jpg)](https://www.imagemhost.com.br/image/rTBMxS)

   Esta troca de célula e base foi essencial para o andamento do projeto, pois os resultados foram muito mais precisos e a faixa de valores não alterava conforme o tempo.
  
## 2.3 A placa

   Foi feito o esquemático da placa no software Altium, utilizando o Atmega328P como microcontrolador, juntamente dos sensores listados acima. O design de PCB pode ser visto na figura 8 abaixo.

![Figura 8.png](https://www.imagemhost.com.br/images/2022/12/21/layout.png)

A placa tem dimensão de aproximadamente 9,2x8cm, para caber dentro da estrutura do alimentador. O modelo 3D dela é observado na figura 9.

![Figura 9](https://www.imagemhost.com.br/images/2022/12/21/3D_Superior.png)

## 2.4 O código

   Como comentado anteriormente, primeiramente os códigos foram testados na IDE do Arduino, afim de realizar testes e calibrações. Após esta etapa, eles foram integrados e refeitos no Atmel Studio. Foram usadas as seguintes bibliotecas:

```
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "utils.h"
#include "spi.h"
#include "mfrc522.h"
#include "twi.h"
```

   Para a leitura do RFID estar sempre disponível e reconhecer uma tag cadastrada a qualquer momento, desligou-se as interrupções do motor até o momento que a tag seja lida pelo sensor.

O código inteiro está disponível neste repositório.

## 3. Custos

   Apesar de alguns sensores e materiais obtidos a partir da impressora 3D terem sido cedidos pelo departamento, o projeto deste protótipo de um alimentador automático de gatos gerou alguns custos para a equipe, como é observado na tabela abaixo.

| Material | Custo | 
| :------------------ | :----------: | 
| MDF               | R$ 80,00        | 
| Servo motor               | R$ 120,00        | 
| Balança de alimentos               | R$ 40,00        | 
| Total  | R$ 240,00 |

   Outras alternativas de materiais podem ser obtidas para a construção deste projeto.


## 4. Conclusões

   O alimentador de gatos, após tudo ser montado, ficou como mostra as figuras 10 e 11 abaixo.
   
   ![Figura 10](https://www.imagemhost.com.br/images/2022/12/21/IMG_79988478689c1169e444.jpg)
   
   ![Figura 11](https://www.imagemhost.com.br/images/2022/12/21/IMG_8003.jpg)
   
   A figura 11 mostra o alimentador com a hélice já acoplada, e a figura 10 mostra como a placa foi fixada dentro da estrutura.
   
   [![Vídeo do funcionamento do alimentador]](https://youtube.com/shorts/Q7Cq7R1CHkE?feature=share)

   O projeto apresenteu resultados satisfatórios quando em ambiente controlado. Para funcionamento completo, algumas modificações seriam necessárias, como:
   
   - O RFID utilizado deve possuir um alcance maior (como 10 cm) para a melhor detecção dos gatos;
   - Possuir uma hélice mais resistente, para aguentar o peso da ração sobre ela, sem atrapalhar seu funcionamento;
   - Ser adicionado alguma proteção caso o motor trave;
   - Para um produto final, alguns custos poderiam ser reduzidos, como o material utilizado para a estrutura e o motor da hélice.
