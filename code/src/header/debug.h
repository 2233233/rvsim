#ifndef MYVM_DEBUG_H
#define MYVM_DEBUG_H
#include "riscv.h"
//默认step
#define AUTO_STATE TRUE
#define STEP_STATE FALSE
void console();
void print_disasm(uint32_t instr_addr);
void print_mem(uint32_t mem_addr);
void print_xreg();
void print_csr();
void print_plic_reg();
void print_clint_reg();

#define DEBUG_PROCESS_CODE()    if(instr_run_state) {\
                                    if(!search(bpTree, reg[rg_pc])){\
                                        break;\
                                    }\
                                    instr_run_state = STEP_STATE;   \
                                    console();                 \
                                }else{\
                                    console();\
                                    break;\
                                }

#endif //MYVM_DEBUG_H
