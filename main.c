//adc no p6.1

#include <msp430.h>
#include <clock.h>
#include <time.h>
#include <stdlib.h>

//PINOS do SPI
#define CS BIT3     //2.3  CS
#define MOSI BIT0   //3.0  SPI MOSI (DIN)
#define SCLK BIT2   //3.2  SPI clock



void Init_MAX7219(void);                                            //Inicializa MAX7219
void SPI_Init(void);                                                //Init SPI


void SPI_Write2(unsigned char, unsigned char);                      //Transmte SPI para escrever na matriz de LED

void config_timer(void);                                            //Timer do ADC
void config_pin_adc(void);                                          //Config P6.1 para o ADC
void config_adc(void);                                              //Config ADC na porta p6.1

void escreve_matriz(char *);                                       //
void introd(void);                                                 // Introdução do Jogo (Escreve Pong na tela)

char pos_bolinha(unsigned int);                                    //

void display_clear(void);                                          // Função que limpa o display (zero em todos leds)
void display_full(void);                                           // Função que liga todos os leds
void game_over(void);                                              // Indica fim do jogo, pisca 3 vezes a tela e reinicia


int main(void){

       unsigned int p1,p2,p3;
       int dir_linha =  1;
       int dir_coluna = 1;
       int dir2_linha =  1;
       int dir2_coluna = 1;

       unsigned int bolinha_linha = 0;
       unsigned int bolinha_coluna = 0;
       unsigned int bolinha2_linha = 0;
       unsigned int bolinha2_coluna = 0;

       unsigned int init = 0;                                            //Variável para controlar início do jogo

       int vel = 25;                                                     //Indica velocidade do game

       int vel_aux = 0;

       int valor_Y = 0;                                                  //variável do adc, mede tensão analogica

       int pontos = 0;
       int level = 1;

       srand(time(NULL));                                               //aleatoriedade


       WDTCTL = WDTPW | WDTHOLD;

       clockInit();
       config_timer();
       config_adc();
       config_pin_adc();

       SPI_Init();

       _delay_cycles(100000);

       Init_MAX7219();

       _delay_cycles(1000);

       char z = 0b10000000;


       while(1){

           char p[8] = {0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000,
                          0b00000000};

           if(init == 0){
               introd();
               bolinha_linha = (rand() % 8);
               bolinha_coluna = 3 + (rand() % (8 - 3));
               p[bolinha_linha] |= pos_bolinha(bolinha_coluna);
               init++;
           }else{
              if(vel_aux == vel ){
                  vel_aux = 0;
                   if(bolinha_linha == 7){
                       dir_linha = -1;
                   }
                   if(bolinha_linha == 0){
                       dir_linha = 1;
                   }
                   if(bolinha_coluna == 7){
                       dir_coluna = -1;
                       pontos++;
                       switch(pontos){
                           case 3:
                               vel=20;
                               break;
                           case 6:
                               vel = 15;
                               break;
                           case 9:
                               vel = 10;
                               break;
                           case 12:
                               vel = 5;
                               break;
                           case 20:
                               vel=25;
                               level = 2;
                               pontos = 0;
                               bolinha_linha = (rand() % 8);
                               bolinha_coluna = 3 + (rand() % (8 - 3));
                               bolinha2_linha = (rand() % 8);
                               bolinha2_coluna = 6 + (rand() % (8 - 6));
                               break;
                       }
                   }
                   if(bolinha_coluna == 1){
                       if(bolinha_linha+1 == p1){
                          dir_coluna = 1;
                          dir_linha = -1;
                      }
                       if(bolinha_linha-1 == p2 && dir_linha == -1){
                           dir_coluna = 1;
                       }
                       if(bolinha_linha+1 == p2 && dir_linha == 1){
                           dir_coluna = 1;
                       }
                       if(bolinha_linha == p2 || bolinha_linha == p2){
                           dir_coluna = 1;
                       }
                       if(bolinha_linha-1 == p3){
                          dir_coluna = 1;
                          dir_linha = 1;
                      }
                   }

                   if(bolinha_coluna == 0){  //ver ond fica esse laço
                       game_over();
                       dir_coluna = 1;
                       init = 0;
                       pontos = 0;
                       level = 1;
                       vel = 25;
                   }

                   if(level == 2){
                    if(bolinha2_linha == 7){
                       dir2_linha = -1;
                     }
                     if(bolinha2_linha == 0){
                         dir2_linha = 1;
                     }
                     if(bolinha2_coluna == 7){
                         dir2_coluna = -1;
                     }
                     if(bolinha2_coluna == 1){
                         if(bolinha2_linha+1 == p1){
                            dir2_coluna = 1;
                            dir2_linha = -1;
                        }
                         if(bolinha2_linha-1 == p2 && dir2_linha == -1){
                             dir2_coluna = 1;
                         }
                         if(bolinha2_linha+1 == p2 && dir2_linha == 1){
                             dir2_coluna = 1;
                         }
                         if(bolinha2_linha == p2 || bolinha2_linha == p2){
                             dir2_coluna = 1;
                         }
                         if(bolinha2_linha-1 == p3){
                            dir2_coluna = 1;
                            dir2_linha = 1;
                        }
                     }

                     if(bolinha2_coluna == 0){  //ver ond fica esse laço
                         game_over();
                         dir2_coluna = 1;
                         init = 0;
                         pontos = 0;
                         level = 1;
                         vel = 25;
                     }

                     bolinha2_linha += dir2_linha;
                     bolinha2_coluna += dir2_coluna;
                   }
                   bolinha_linha += dir_linha;
                   bolinha_coluna += dir_coluna;
              }

              p[bolinha_linha] |= pos_bolinha(bolinha_coluna);
              if(level == 2){
                  p[bolinha2_linha] |= pos_bolinha(bolinha2_coluna);
              }
              vel_aux ++;
           }



           while((ADC12IFG&ADC12IFG0) == 0);
           valor_Y = ADC12MEM0/2;

           if( valor_Y <= 220){
               p1 = 0;
               p2 = 1;
               p3 = 2;
           } else if( valor_Y > 220 && valor_Y <= 441){
               p1 = 1;
               p2 = 2;
               p3 = 3;
           } else if( valor_Y > 441 && valor_Y <= 520){
               p1 = 2;
               p2 = 3;
               p3 = 4;
           } else if( valor_Y > 520 && valor_Y <= 1029){
               p1 = 3;
               p2 = 4;
               p3 = 5;
           }else if( valor_Y > 1029 && valor_Y <= 1600){
               p1 = 4;
               p2 = 5;
               p3 = 6;
           } else if( valor_Y > 1600 && valor_Y <= 2048){
               p1 = 5;
               p2 = 6;
               p3 = 7;
           }

           p[p1] |= z;
           p[p2] |= z;
           p[p3] |= z;


           escreve_matriz(p);
       }



}


void config_adc(void){
    ADC12CTL0 &= ~ADC12ENC;         // Desabilitar para configurar

    ADC12CTL0 = ADC12SHT0_3 |       // Tsample > 2,33us
                ADC12ON;            // Ligar ADC

    ADC12CTL1 = ADC12CSTARTADD_0 |  // Guardar resultado em ADC12MEM0
                ADC12SHS_1       |  // Selecionar timer TA0.1 **mudei
                ADC12SHP         |  // Usar Timer
                ADC12DIV_0       |  // Dividir clock por 1
                ADC12SSEL_3      |  // Clock = SMCLK
                ADC12CONSEQ_3;      // Modo repeat-single-channel - Um canal convertendo repetidamente

    ADC12CTL2 = ADC12TCOFF       |  // Desligar sensor de temperatura
                ADC12RES_2;         // 12 bits

    ADC12MCTL0 = ADC12SREF_0     |  // VR+ = AVCC e VR- = AVSS
                 ADC12INCH_1;       // (Pino 6.1) A0 CANAL 1

    ADC12CTL0 |= ADC12ENC;          // Habilitar ADC12
}

void config_pin_adc(void){
    P6DIR &= BIT1;   // CANAL A0
    P6OUT &= ~BIT1;
    P6REN |= BIT1;
    P6SEL |= BIT1;

}

void config_timer(void){
    TA0CTL = (TASSEL_2 | ID_0 | MC_1 | TACLR);
    TA0CCR0 = 1000;
    TA0CCR1 = 999;
    TA0CCTL1 = OUTMOD_6;
}

void SPI_Init(void) //SPI initialization

{

    P2DIR |= CS;  //cs is output

    P3SEL |= MOSI + SCLK; //spi init

    //P3SEL2 |= MOSI + SCLK; //spi init

    UCB0CTL1 = UCSWRST;

     UCB0CTL0 |= UCMSB + UCMST + UCSYNC + UCCKPH; // 3-pin, 8-bit SPI master

     UCB0CTL1 |= UCSSEL_2;                     // SMCLK

     UCB0BR0 = 10;                          // spi speed is smclk/10

     UCB0BR1 = 0;                              //

     UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**



     __enable_interrupt(); // enable all interrupts

}



void SPI_Write2(unsigned char MSB, unsigned char LSB) //SPI write one byte

{



       P2OUT &= ~CS;

       _delay_cycles(50);

       UCB0TXBUF = MSB ;

       while (UCB0STAT & UCBUSY);

       UCB0TXBUF = LSB ;

       while (UCB0STAT & UCBUSY);

       P2OUT |= CS;

}

void Init_MAX7219(void)

{

       SPI_Write2(0x09, 0x00);       //

       SPI_Write2(0x0A, 0x0F);       //

       SPI_Write2(0x0B, 0x0F);       //

       SPI_Write2(0x0C, 0x01);       //

       SPI_Write2(0x0F, 0x0F);       //

       SPI_Write2(0x0F, 0x00);       //

}

void display_clear(void){

    int i;
    char z=0x00;
    for(i=1;i<9;i++){
    SPI_Write2(i,z);
    }
}

void display_full(void){

    int i;
    char z=0xFF;
    for(i=1;i<9;i++){
    SPI_Write2(i,z);
    }
}


void escreve_matriz(char *p){

    int i = 0;

    for(i=1;i<9;i++){

        SPI_Write2(i,p[i-1]);


    }

}

char pos_bolinha(unsigned int pos){
    switch (pos){
    case 0:
        return 0b10000000;
    case 1:
        return 0b01000000;
    case 2:
        return 0b00100000;
    case 3:
        return 0b00010000;
    case 4:
        return 0b00001000;
    case 5:
        return 0b00000100;
    case 6:
        return 0b00000010;
    case 7:
        return 0b00000001;
    default:
        return 0b00000000;
    }
}

void game_over(void){
    int pisca = 3;
    int i = 0;

    for(i = 0; i < pisca; i++){
        display_full();
        _delay_cycles(5000000);
        display_clear();
        _delay_cycles(5000000);

    }
}

void introd(void){

    char p[8] = {0x3C,0x22,0x22,0x22,0x3C,0x20,0x20,0x20}; //P
    char o[8] = {0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C}; //O
    char n[8] = {0x00,0x42,0x62,0x52,0x4A,0x46,0x42,0x00};  //N
    char g[8] = {0x3C,0x40,0x40,0x40,0x40,0x44,0x44,0x3C}; //G

    escreve_matriz(p);
    _delay_cycles(10000000);
    escreve_matriz(o);
    _delay_cycles(10000000);
    escreve_matriz(n);
    _delay_cycles(10000000);
    escreve_matriz(g);
    _delay_cycles(10000000);

}
