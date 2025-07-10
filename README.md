# 📦 Sistemas Embarcados – Projetos Acadêmicos e Práticos

Repositório contendo projetos desenvolvidos na disciplina de **Sistemas Embarcados**, utilizando microcontroladores **AVR (ATmega328P)** e simulados no **SimulIDE**. Os projetos demonstram controle básico de hardware, interrupções externas, displays de 7 segmentos e gerenciamento de entradas (botões).

## 🔧 Tecnologias e Ferramentas
- Linguagem **C**
- Microcontroladores **AVR (ATmega328P)**
- **AVR-GCC**
- **SimulIDE**
- Displays de 7 segmentos
- Botões com interrupção (**PCINT**)
- Arquivos **.hex** para simulação

## 📂 Projetos incluídos

### ✅ Display de 7 segmentos controlado por botões
- Incremento individual de cada dígito usando botões conectados aos pinos PC0–PC3.
- Multiplexação para exibição dos dígitos.
- Uso de interrupções para detectar as pressões dos botões.

### ➕ Outros projetos (futuros)
- PWM com LED
- Leitura de sensores analógicos
- Comunicação serial

## ▶️ Como rodar no SimulIDE
1. Abra o arquivo `.simu` no **SimulIDE**.
2. Dê dois cliques no microcontrolador (**ATmega328P**).
3. Em "Firmware", selecione o arquivo `.hex`.
4. Pressione **Run** para iniciar a simulação.
5. Pressione os botões simulados (C0 a C3) e observe a contagem no display.

## 👤 Autor
Raphael Di Pace
