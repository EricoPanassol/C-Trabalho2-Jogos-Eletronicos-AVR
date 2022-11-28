#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

void nokia_lcd_init(void);
void nokia_lcd_clear(void);
void nokia_lcd_set_cursor(uint8_t x, uint8_t y);
void nokia_lcd_write_string(const char *str, uint8_t scale);
void nokia_lcd_render(void);
void nokia_lcd_drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void nokia_lcd_drawrect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void nokia_lcd_drawcircle(uint8_t x1, uint8_t y1, uint8_t r);
void won();
void lost();
void drawEnemy();
void drawElements();

// flor
uint8_t floor_x1 = 0;
uint8_t floor_y1 = 43;
uint8_t floor_x2 = 83;
uint8_t floor_y2 = 43;

// player
uint8_t player_lb = 7;  
uint8_t player_tb = 33; // cresce pra 0
uint8_t player_rb = 17;
uint8_t player_bb = 43; // chão do eixo y

// enemy
uint8_t enemy_lb = 76;
uint8_t enemy_tb = 33; // cresce pra 0
uint8_t enemy_rb = 80; 
uint8_t enemy_bb = 43; // chão do eixo y

uint8_t points = 0;
uint8_t jogando = 1;
uint8_t pressed = 0;
uint8_t pulo = 0;

ISR(INT0_vect)
{
    if (!(PIND & (1 << PD2)))
    {   
        pressed = 1;
    }
}

int main()
{
    nokia_lcd_init();
    nokia_lcd_clear();
    
    // Definindo as saídas
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);

    // Definindo as entradas
    DDRD &= (0 << PD2);

    // Definindo as interrupções
    PCICR |= (1 << PCIE2); // habilita vetor de interrupção de todos os PDs
    PCMSK2 |= (1 << PCINT16); // habilita interrupção de PD0

    // Habilitando o pull-up (sempre passa 1)
    PORTD |= (1 << PD2);

    EICRA = 0b00000010; // interrupcao externa INT0 na borda de descida
    EIMSK = 0b00000001; // habilita a interrupcao externa INT0

    sei();
    

    // game loop
    while(jogando == 1){
        
        // winning condition
        if(points == 10){
            won();
        }
        else{
            if(pressed == 1){
                pulo = 1;

                if(jogando == 0){
                    return -1;
                }
                pressed = 0;
            }

            // up
            if(pulo == 1){
                if(player_tb > 13){
                    player_tb -= 1;
                    player_bb -= 1;
                }
                else{
                    pulo = 0;
                }
            // down
            }else{
                if(player_bb < 43){
                    player_tb += 1;
                    player_bb += 1;
                }
            }
            
            // draw elements on screen
            _delay_ms(1);
            drawEnemy();
            drawElements();
            
            // verify colision and lose condition
            if(enemy_lb <= player_rb && enemy_tb < player_bb && enemy_bb > player_tb){
                lost();
            }
        }
    }

}

void won(){
    nokia_lcd_clear();
    nokia_lcd_set_cursor(18, 10);
    nokia_lcd_write_string("You Won", 1);
    nokia_lcd_set_cursor(15, 27);
    char str[13];
    sprintf(str, "Points: %d", points);
    nokia_lcd_write_string(str, 1);
    nokia_lcd_render();
}

void lost(){
    nokia_lcd_clear();
    nokia_lcd_set_cursor(18, 10);
    nokia_lcd_write_string("You Lost", 1);
    nokia_lcd_set_cursor(15, 27);
    char str[13];
    sprintf(str, "Points: %d", points);
    nokia_lcd_write_string(str, 1);
    nokia_lcd_render();
    jogando = 0;
}

void drawElements(){
    nokia_lcd_clear();
    nokia_lcd_drawline(floor_x1, floor_y1, floor_x2, floor_y2);
    nokia_lcd_drawrect(player_lb, player_tb, player_rb, player_bb);
    nokia_lcd_drawrect(enemy_lb, enemy_tb, enemy_rb, enemy_bb);
    char str[13];
    sprintf(str, "points: %d", points);
    nokia_lcd_write_string(str, 1);
    nokia_lcd_render();
}

void drawEnemy(){
    enemy_lb -= 1;
    enemy_rb -= 1;
    if(enemy_lb <= 0 || enemy_rb <= 0){
        enemy_lb = 80;
        enemy_rb = 76;
        points++;
    }
}

    

   
