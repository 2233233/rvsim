#include "header/plic.h"
extern uint8_t* uart0_reg;
extern uint8_t* plic_reg;
extern HANDLE hStdin;
extern int running;

#define LSR  (uart0_reg+5)
#define THR  (uart0_reg)
#define RHR  (uart0_reg)
#define UART0_IRQ 10

void uart0_print_func(void){
    *LSR |= 0x20;
    if (*THR != 0){
        //uart0有数据传来
        *LSR &= 0xDF;
        putchar(*(char*)THR);
        *THR = 0;
        *LSR |= 0x20;
    }
}

void keyboard_thread_run(void){
    int c = 0;
    disable_input_buffering();
    setbuf(stdout, 0);

    while(running){
        while(getchar() != '\n');
        c = getchar();
        *LSR &= 0xFE;
        *RHR = (char)c;
        *LSR |= 0x1;
        SET_PENDING(UART0_IRQ)
    }
}