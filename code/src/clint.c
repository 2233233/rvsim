#include "header/riscv.h"
extern uint32_t* clint_reg;
extern uint32_t* csr;
#define TIMER_INC_INSTR_NUM 5
#define MTIME ((uint64_t*)(clint_reg + clint_mtime1))
#define MTIMECMP ((uint64_t*)(clint_reg + clint_mtimecmp1))
void clint_func(void){

    (*MTIME) += 1;
    if(*MTIME == *MTIMECMP){
        csr[csr_mip] |= 0x80;
        *MTIME = 0;
    }
    if(clint_reg[clint_MSIP] & 0x1){
        csr[csr_mip] |= 0x8;

    }else{
        csr[csr_mip] &= 0xFFFFFFF7;
    }

}

