# Alimentador Automático de Gatos
Este repositório apresenta o projeto de um alimentador automático de gatos, onde cada gato é reconhecido pelo sistema e possui uma porção específica de ração. O projeto foi desenvolvido para a displina de Projeto Integrador III, do curso de Engenharia Eletrônica do IFSC, pelos estudantes Enzzo Comassetto dos Santos e Jade Dutra Lopes.

## 1. A estrutura

   Para a estrutura do projeto, foi utilizado MDF para a construção do protótipo (figura 1) juntamente de um cano PVC tipo T de 50mm (figura 2) utilizado como o dispenser da ração para o pote. O conjunto foi montado conforme a figura 3.

[![Figura 1](https://www.imagemhost.com.br/images/2022/07/26/estrutura-1.th.jpg)](https://www.imagemhost.com.br/image/rTlkOC)
[![Figura 2](https://www.imagemhost.com.br/images/2022/07/26/estrutura-2.th.jpg)](https://www.imagemhost.com.br/image/rTlwlL)
[![Figura 3](https://www.imagemhost.com.br/images/2022/07/26/estrutura-3.th.jpg)](https://www.imagemhost.com.br/image/rTlzaB)

  De acordo com a progressão das atividades, foi preciso fazer algumas alterações físicas na estrutura. O sensor de peso (que será detalhado mais para frente) foi um grande obstáculo para o andamento do projeto. Esse requeriu mudanças no alimentador para que pudesse operar coerentemente. 
  
### 1.1 Hélice

   Para empurrar a ração ao pote, foi feita em uma impressora 3D uma hélice helicoidal, fixada no servo motor. A ração cai sobre ela, e com o auxílio do motor, a quantidade especificada de ração cai sobre o pote. O design do helicóide foi retirado de um [site](https://www.thingiverse.com/thing:27854) com projeto similar, e ajustado para o tamanho do cano PVC usado. A figura 4 mostra as duas hélices que foram impressas, de 13 e 9cm, respectivamente. A de 13cm foi a utilizada no projeto, enquanto a de 9, como foi resultado de uma impressão falha, foi usada como teste para furos para o encaixe do motor.
  
[![Figura 4](https://www.imagemhost.com.br/images/2022/07/26/helicoide-1.th.jpg)](https://www.imagemhost.com.br/image/rTlaZK)


### 1.2 Motor

  A fim de empurrar a ração para o pote, foi utilizado um [servo motor SM-S4306R 360º](https://www.filipeflop.com/produto/servo-motor-sm-s4306r-360-graus-acessorios/), sem ser necessário o uso de redução para tal. Ele é alimentado em 5V, e funciona por PWM.
  
### 
  
## 2. O projeto

  O alimentador automático de gatos identifica o gato que se aproxima do leitor RFID por uma determinada tag, onde cada gato possui uma porção de ração específica por dia. O motor libera ração o suficiente para completar a porção do gato da vez (monitorado pelo sensor de peso abaixo do pote) para aquele momento do dia.
  
 ## 2.1 Microcontolador
 
  O microcontrolador utilizado foi o **Atmega328P**, inicialmente acoplado em um Arduino Uno. A plataforma do Arduino foi muito útil para os testes iniciais dos sensores e do motor, possibilitando calibrações e detecções de possíveis erros de leitura.
  
   Após todos os sensores serem testados, o código foi adaptado para linguagem C no software Atmel Studio.
 
 ## 2.2 Sensores
 
 ### 2.2.1 Sensor RFID
 
  O sensor **RFID-RC522 13,56MHz** foi utilizado para a identificação de cada felino. Foram utilziadas duas tags diferentes, cada qual com um _id_ diferente para cada gato. Quando a tag é identificada, um sinal é enviado ao motor e este gira a espira até completar a porção necessária.
  
  A ideia original seria o sensor ficar numa posição em frente ao alimentador, sendo capaz de captar a tag do gato. Porém, por ser um protótipo, o sensor ficou em posição de difícil acesso à um gato real, sendo apenas utilizado afim de testes do projeto.
  
 ### 2.2.2 Sensor RTC
 
  O módulo *RTC DS1307* tem a função de controlar o tempo do dia para que o gato não coma mais de 1 porção em determinado período.
  
 ### 2.2.3 Sensor de peso
 
  O sensor de peso com certeza foi o mais complexo para se trabalhar de todo o projeto. Inicialmente, trabalhou-se com uma célula de carga de 50kg juntamente de um **amplificador conversor A/D HX711**. Essa célula de carga funciona de acordo com a deformação que o centro dela sofre de um peso. Para o funcionamento correto da célula, é preciso criar uma base para a mesma, de forma que o centro não fique em contato com a superfície. 
  
  Foi impresso em 3D uma base para a célula de peso, seguindo um [modelo](https://www.thingiverse.com/thing:2624188) achado para o mesmo tipo de célula, de acordo com um [tutorial](https://circuitjournal.com/50kg-load-cells-with-HX711#1x50) para esta aplicação. A figura 5 mostra como ficou a base para o nosso sensor.
  
[![Figura 5](https://www.imagemhost.com.br/images/2022/07/27/base-cel.th.jpg)](https://www.imagemhost.com.br/image/rTBF9Y)

Entretando, apesar de ter a estrutura para a célula, o sensor continuava a apresentar resultos extremamentes imprecisos, e não conseguia fixar em uma faixa específica de valores. Dado este enorme problema, a equipe optou por comprar uma balança de peso de alimentos, desmontar a mesma e utilizar a célula de carga + a base desta, como mostra a figura 6. A célula em questão é apresentada na figura 7, é de formato diferente da original e suporta até 10kg.

[![Figura 6](https://www.imagemhost.com.br/images/2022/07/27/sensor-peso-2.th.jpg)](https://www.imagemhost.com.br/image/rTB4zI)
[![Figura 7](https://www.imagemhost.com.br/images/2022/07/27/celula-carga.th.jpg)](https://www.imagemhost.com.br/image/rTBMxS)

Esta troca de célula e base foi essencial para o andamento do projeto, pois os resultados foram muito mais precisos e a faixa de valores não alterava conforme o tempo.
  
