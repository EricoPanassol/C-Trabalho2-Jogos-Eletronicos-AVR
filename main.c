#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


void nokia_lcd_init(void);
void nokia_lcd_clear(void);
void nokia_lcd_set_cursor(uint8_t x, uint8_t y);
void nokia_lcd_write_string(const char *str, uint8_t scale);
void nokia_lcd_render(void);
void nokia_lcd_drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void nokia_lcd_drawrect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void nokia_lcd_drawcircle(uint8_t x1, uint8_t y1, uint8_t r);




ISR(INT0_vect)
{
    if (!(PIND & (1 << PD2)))
    {   
        // nokia_lcd_clear();
        // _delay_ms(2000);
        PORTC = (1 << PC1);
        // nokia_lcd_set_cursor(36, 15);
        // nokia_lcd_write_string("Oi\001", 2);
        // nokia_lcd_render();
        _delay_ms(2000);
        PORTC = 0;
    }

}


int main()
{
    nokia_lcd_init();
    nokia_lcd_clear();
    
    
    
    // Definindo as saídas
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
    DDRC |= (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4);

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

    while(1){
        uint8_t floor_x1 = 0;
        uint8_t floor_y1 = 43;
        uint8_t floor_x2 = 83;
        uint8_t floor_y2 = 43;

        uint8_t char_x1 = 0;
        uint8_t char_y1 = 34;
        uint8_t char_x2 = 9;
        uint8_t char_y2 = 43;

        nokia_lcd_clear();
        nokia_lcd_drawline(floor_x1, floor_y1, floor_x2, floor_y2);
        nokia_lcd_drawrect(char_x1, char_y1, char_x2, char_y2);
        nokia_lcd_render();


    }

}

    

   
