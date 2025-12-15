# Acelerador de Inferência MLP em FPGA

Este repositório contém o desenvolvimento de um sistema embarcado baseado em FPGA
para a execução de inferência local de uma rede neural do tipo MLP (Multi-Layer
Perceptron), desenvolvido como projeto da disciplina **Sistemas Embarcados
(DCA3706)** da **Universidade Federal do Rio Grande do Norte (UFRN)**.

O sistema realiza a classificação binária de padrões de entrada, exibindo o
resultado final como **CLASSE 0** ou **CLASSE 1** diretamente nos displays de
7 segmentos da placa FPGA, caracterizando uma aplicação de **Inteligência
Artificial de Borda (Edge AI)**.

## 📌 Objetivo do Projeto

Implementar um **acelerador de inferência MLP em hardware**, utilizando VHDL,
capaz de:
- Realizar inferência local, sem comunicação com a nuvem;
- Processar entradas digitais fornecidas por switches;
- Classificar os dados em duas classes possíveis (Classe 0 ou Classe 1);
- Exibir o resultado da classificação nos displays de 7 segmentos da FPGA.

---

## 🧠 Arquitetura da Rede Neural

A rede neural implementada possui a seguinte topologia:

- **Entradas**: valores inteiros obtidos a partir de 4 nibbles (4 bits cada);
- **Camada 0**: 4 neurônios (blocos MAC);
- **Camada 1**: 3 neurônios (blocos MAC);
- **Camada final**: neurônio binário responsável pela decisão da classe.

Os pesos e bias da MLP foram obtidos a partir de treinamento prévio e fixados
diretamente no código VHDL.

---

### 📄 Descrição dos Arquivos do Repositório

- **acelerador_de_inferencias.vhd**  
  Módulo top-level do sistema. Integra os conversores, camadas da MLP,
  neurônio binário e decodificador de saída.

- **neuronio.txt**  
  Implementação do neurônio da MLP para as camadas intermediárias,
  contendo o bloco MAC (Multiply and Accumulate) combinacional.

- **neuronio_binario.txt**  
  Implementação do neurônio binário responsável pela decisão final da
  classificação, utilizando uma função de ativação baseada em threshold.

- **conversor.txt**  
  Módulo responsável pela conversão de valores de 4 bits para inteiros,
  permitindo a entrada correta dos dados na rede neural.

- **mlp_weights_for_fpga.txt**  
  Arquivo contendo os pesos e bias da rede neural obtidos a partir do
  treinamento prévio, utilizados como referência para a implementação
  em hardware.
---

## ⚙️ Funcionamento do Sistema

1. O usuário insere os dados por meio dos **switches de 16 bits** da placa FPGA;
2. Cada grupo de 4 bits é convertido em um valor inteiro;
3. Os valores são processados pela MLP implementada em hardware;
4. O neurônio binário realiza a decisão final da classificação;
5. O resultado é convertido em uma representação textual;
6. O display de 7 segmentos exibe **CLASSE 0** ou **CLASSE 1**.

A inferência ocorre de forma **combinacional**, sendo recalculada automaticamente
a cada alteração das entradas.

---

## 👩‍💻 Autores

Projeto desenvolvido como parte da disciplina **Sistemas Embarcados (DCA3706)**  
Universidade Federal do Rio Grande do Norte – UFRN

