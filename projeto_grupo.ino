#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// ========== DEFINIÇÃO DAS VARIÁVEIS ==========
#define potenciometro PF0 // PC0 // PF0 (U:A0, M:A0)
#define motorCC PB5 // PB1 // PB5		(U:9, M:11)

#define botaoR PD0 // PD2 // PD0		(U:2, M:21)
#define buzzerP PB6 // PB2 // PB6		(U:10, M:12)

#define chaveFC PD1 // PD3 // PD1		(U:3, M:20)

#define sensorForca PF1 // PC1 // PF1	(U:A1, M:A1)

#define led_power PG5 // PD4 // PG5		(U:4, M:4)
#define led_leitura PE3 // PD5 // PE3	(U:5, M:5)

#define sensor_IR PB4 // PB4 // PB4		(U:12, M:10) (PCINT[4])

volatile uint8_t reset = 0;
volatile uint8_t pesar = 0;
volatile uint8_t motor_vel = 0;
volatile float peso = 0.0;
volatile uint16_t quant_produto = 0;

char buffer[10]; // buffer para conversão do valor ADC
// =============================================

// ========== DECLARAÇÃO DAS FUNÇÕES ==========
uint8_t canal_adc(uint8_t canal);
void inicializa();
void transporte();
void pesagem();
// ============================================

// ===== SERIAL APENAS PARA VISUALIZACAO DOS DADOS =====
// Inicializa o USART com 9600 bps (para 16 MHz clock)
void USART_Init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1<<TXEN0);                // Habilita transmissão
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 bits de dados, 1 stop bit
}

// Envia um caractere via USART
void USART_Transmit(unsigned char data) {
    while (!(UCSR0A & (1<<UDRE0)));  // Espera buffer estar vazio
    UDR0 = data;
}

// Envia uma string via USART
void USART_SendString(const char* str) {
    while (*str) {
        USART_Transmit(*str++);
    }
}
// ============================================

// ===== Função para trocar entre os canais de ADC =====
uint8_t canal_adc(uint8_t canal) {
  // Mantem a configuração do ADMUX e seleciona o novo canal
  ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
  // tempo para o canal estabilizar
  _delay_us(10);
  // Inicia conversão ADC
  ADCSRA |= (1 << ADSC);
  // Espera a conversão terminar
  while (ADCSRA & (1 << ADSC));
  
  // Retorna o valor de 8 bits
  return ADCH;
}
// ============================================

// ===== Função para inicilização ou reset =====
void inicializa() {
  uint16_t freq[] = {440, 260, 330, 440, 260, 330};
  uint16_t top;
  
  for(uint8_t i=0; i<6; i++) {
    // Ativa saída PWM no buzzer
    TCCR1A |= (1 << COM1B1);

    top = (F_CPU / (2UL * 64UL * freq[i])) - 1; // usa prescaler 64
    ICR1 = top;
    OCR1B = top / 2; // buzzer no canal B
    _delay_ms(100);
    
    // Desliga PWM e zera pino para evitar ruído
    TCCR1A &= ~(1 << COM1B1);
    PORTB &= ~(1 << buzzerP);

    _delay_ms(50);
    USART_SendString("\nINICIALIZA/RESET\n");
  }
}
// ============================================

// Função para tranporte
void transporte() {
  // POTENCIOMETRO -------------------------------

  // Recebe o valor do ADCH convertido do potenciômetro
  motor_vel = canal_adc(0x00);
  // Ajusta para porcentagem
  motor_vel /= 2.55;

  // Atualiza PWM com o valor lido do potenciômetro
  ICR1 = 250;
  OCR1A = motor_vel;

  // Converte para string
  itoa(motor_vel, buffer, 10);

  // Envia via serial
  USART_SendString("Velocidade: ");
  USART_SendString(buffer);
  USART_SendString("%\r\n"); // Adiciona símbolo de porcentagem e nova linha
}

// Função para pesagem
void pesagem() {
  while(peso <= 10) {
    // Recebe o valor do ADCH convertido do sensor de força
    peso = canal_adc(0x01);
    // Limite da leitura no simulador foi 116
    // Calibração para limitar em 10 Kg
    peso /= 10.0;
    
    // Converte para string
    dtostrf(peso, 5, 2, buffer); // 5 caracteres, 2 casas decimais
    
    // Envia via serial
    USART_SendString("PESO: ");
    USART_SendString(buffer);
    USART_SendString(" Kg\r\n");
  }
  // redefinir variaveis
  
}
//--------------------------------------------


int main(void) {
  // Configuração UART: 9600 bps (para 16MHz)
  USART_Init(103);  // UBRR = 103 para 9600 bps com F_CPU = 16MHz
  
  // POTENCIOMETRO
  DDRF &= ~(1<<potenciometro); // PC0 ou PF0 como entrada (ADC0) potenciometro
  
  // MOTOR CC
  DDRB |= (1 << motorCC); // PB1 ou PB5 como saída (OCR1A) motor
  
  // Botão para reset
  DDRD &= ~(1 << botaoR); // PD2 ou PD0 como entrada para botão
  PORTD |= (1 << botaoR); // ativa pull-up interno
  
  // Buzzer passivo
  DDRB |= (1 << buzzerP); // PB2 ou PB6 como saída (OCR1B) buzzer passivo
  
  // Chave fim de curso (simulado por botão)
  DDRD &= ~(1 << chaveFC); // PD3 ou PD1 como entrada para botão
  PORTD |= (1 << chaveFC); // ativa pull-up interno
  
  //LARISSA ADICIONOU POTENCIOMETRO E SENSOR DE FORCA MAS N SEI SE TA CERTO
  DDRF &= ~ (1<<sensorForca);
  
  // Led de funcionamento e de leitura do sensor IR
  DDRG &= (1 << led_power);
  DDRE &= (1 << led_leitura);
  
  DDRB &= ~(1 << sensor_IR); // PB4 como entrada (PCINT[4]) sensor_IR
  
  // Configuração ADC
  ADMUX = (1<<REFS0) | (1<<ADLAR); // Referência AVCC
  ADCSRA = (1<<ADEN) | 0x07; // Habilita ADC, prescaler de 128
  
  // === CONFIGURAÇÃO DO TIMER1 PARA Fast PWM, Modo 14 ===
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // OC1A não invertido
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Prescaler 64
  OCR1A = 0; // Ciclo inicial do PWM (motor desligado)
  OCR1B = 0; // Ciclo inicial do PWM (buzzer passivo desligado)

  // Configuração da Interrupção Externa
  EIMSK |= (1 << INT0) | (1 << INT1);  // Habilita interrupção externa INT0 e INT1
  EICRA |= (1 << ISC01); // Dispara na borda de descida
  
  // Configuração da Interrupção por Mudança de Pino
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4);
  // ----------
  
  sei();				 //Habilita interrupções globalmente
  
  PORTG |= (1 << led_power);
  inicializa();

  while(1) {
    // 
    PORTE &= ~(1 << led_leitura);
    
    transporte();
    
    // Salta para a função de inicialização/reset
    if(reset == 1) {
      reset = 0;
      inicializa();
    }
    
    // Salta para a função de pesagem
    if(pesar == 1) {
      pesar = 0;
      pesagem();
    }
    
  }
}

ISR(INT0_vect) {
  _delay_ms(50);  // aguarda 50 ms para estabilizar o botão
  if (!(PIND & (1 << botaoR))) {  // verifica se o botão ainda está pressionado
    //reset = 1;
    reset = 1;
  }

  // Envia via serial
  USART_SendString("\nRESET\n");
}

ISR(INT1_vect) {
  _delay_ms(50);  // aguarda 50 ms para estabilizar o botão
  if (!(PIND & (1 << chaveFC))) {  // verifica se o botão ainda está pressionado
    pesar = 1;
  }
  
  // Envia via serial
  USART_SendString("\nPESAGEM\n");
}

ISR(PCINT0_vect) {
  // 
  _delay_ms(20); // debounce simples
  char num[10];
  if ( !(PINF & (1 << sensorForca)) ) {
    quant_produto++;
    PORTE |= (1 << led_leitura); // acende led
    itoa(quant_produto, num, 10);
    USART_SendString(num);
    USART_SendString("\nLEITURA\n");
  }
}
