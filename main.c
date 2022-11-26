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
void startMenu();
void resetEnemy();
void drawElements();
char scan_keypad();

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

// global variables
uint8_t points = 0;
uint8_t jogando = 0;
uint8_t pressed = 0;
uint8_t pulo = 0;
uint8_t start = 0;
uint16_t key;

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
    // configura PD6 e PD7 como saídas
    DDRD |= (1 << PD6) | (1 << PD7);

    // Definindo as entradas
    DDRD &= (0 << PD2);
    // configura PD5 como entrada (com pull-down externo)
    DDRD &= ~(0 << PD5);

    // Habilitando o pull-up (sempre passa 1)
    PORTD |= (1 << PD2);

    // Definindo as interrupções
    PCICR |= (1 << PCIE2); // habilita vetor de interrupção de todos os PDs
    EICRA = 0b00000010;    // interrupcao externa INT0 na borda de descida
    EIMSK = 0b00000001;    // habilita a interrupcao externa INT0

    sei();

    // menu
    while (1)
    {
        startMenu();
        key = scan_keypad();
        if (key)
        {
            jogando = 1;
            break;
        }
        break;
    }

    nokia_lcd_clear();

    // game loop
    while (jogando == 1)
    {

        if (pressed == 1)
        {
            pulo = 1;

            if (jogando == 0)
            {
                return -1;
            }
            pressed = 0;
        }

        // subida
        if (pulo == 1)
        {
            if (player_tb > 13)
            {
                player_tb -= 1;
                player_bb -= 1;
            }
            else
            {
                pulo = 0;
            }
            // descida
        }
        else
        {
            if (player_bb < 43)
            {
                player_tb += 1;
                player_bb += 1;
            }
        }

        _delay_ms(1);
        resetEnemy();
        drawElements();

        if (enemy_lb <= player_rb && enemy_tb < player_bb && enemy_bb > player_tb)
        {
            nokia_lcd_clear();
            nokia_lcd_write_string("Voce perdeu\001", 1);
            nokia_lcd_render();
            jogando = 0;
        }
    }
}

void startMenu()
{
    nokia_lcd_set_cursor(29, 0);
    nokia_lcd_write_string("Game", 1);
    nokia_lcd_set_cursor(10, 17);
    nokia_lcd_write_string("(1) - Start", 1);
    nokia_lcd_set_cursor(10, 34);
    nokia_lcd_write_string("(2) - Exit", 1);
    nokia_lcd_render();
}

void drawElements()
{
    nokia_lcd_clear();
    nokia_lcd_drawline(floor_x1, floor_y1, floor_x2, floor_y2);
    nokia_lcd_drawrect(player_lb, player_tb, player_rb, player_bb);
    nokia_lcd_drawrect(enemy_lb, enemy_tb, enemy_rb, enemy_bb);
    char str[13];
    sprintf(str, "points: %d", points);
    nokia_lcd_write_string(str, 1);
    nokia_lcd_render();
}

void resetEnemy()
{
    enemy_lb -= 1;
    enemy_rb -= 1;
    if (enemy_lb <= 0 || enemy_rb <= 0)
    {
        enemy_lb = 80;
        enemy_rb = 76;
        points++;
    }
}

// entrada - PD5
// saida - PD6, PD7
char scan_keypad()
{
    char keys[1][2] = {{'1', '2'}};
    uint8_t i, j;

    // seleciona a coluna
    for (j = 0; j < 2; j++)
    {
        if (j == 0)
        {
            PORTD |= (1 << PD6);
        }
        if (j == 1)
        {
            PORTD |= (1 << PD7);
        }
        // seleciona a linha PD5 = 5
        for (i = 5; i < 6; i++)
        {
            if (PIND & (1 << i))
            {
                // Esperamos a tecla ser solta, para evitar repetição de entrada
                _delay_ms(5);
            }
        }
        PORTD &= ~((1 << PD6) | (1 << PD7));
        return keys[i][j];
    }

    return 0;
}
