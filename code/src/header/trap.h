//
// Created by Administrator on 2024/9/3.
//

#ifndef MYVM_TRAP_H
#define MYVM_TRAP_H
#include "riscv.h"
void check_trap();

//instruction address misaligned
#define SET_MCAUSE_IAM csr[csr_mcause] = 0x0;
//instruction access fault
#define SET_MCAUSE_IAF csr[csr_mcause] = 0x1;
//illegal instruction
#define SET_MCAUSE_II csr[csr_mcause] = 0x2;
//breakpoint
#define SET_MCAUSE_B csr[csr_mcause] = 0x3;
//load address misaligned
#define SET_MCAUSE_LAM csr[csr_mcause] = 0x4;
//load access fault
#define SET_MCAUSE_LAF csr[csr_mcause] = 0x5;
//store/amo address misaligned
#define SET_MCAUSE_SAM csr[csr_mcause] = 0x6;
//store/amo access fault
#define SET_MCAUSE_SAF csr[csr_mcause] = 0x7;
//environment call from u
#define SET_MCAUSE_ECU csr[csr_mcause] = 0x8;
//environment call from s
#define SET_MCAUSE_ECS csr[csr_mcause] = 0x9;
//environment call from m
#define SET_MCAUSE_ECM csr[csr_mcause] = 0xB;
//instruction page fault
#define SET_MCAUSE_IPF csr[csr_mcause] = 0xC;
//load page fault
#define SET_MCAUSE_LPF csr[csr_mcause] = 0xD;
//store/amo page fault
#define SET_MCAUSE_SPF csr[csr_mcause] = 0xF;

//发生异常时设置有关的地址
#define SET_MTVAL(ADDRESS) csr[csr_mtval] = (uint32_t)ADDRESS;

#define SET_MCAUSE_ECLL(pl) csr[csr_mcause] = 0x8 + pl;

#define CHECK_LOAD_ALIGNED(addr, w) if(((uint32_t)addr % w) != 0){ \
                                        SET_MCAUSE_LAM             \
                                        SET_MTVAL(addr)             \
                                        return;                               \
                                    }
#define CHECK_LOAD_FAULT(t, addr) if(t == NULL){ \
                                SET_MCAUSE_LAF \
                                SET_MTVAL(addr)                 \
                                return;    \
                            }

#define CHECK_STORE_ALIGNED(addr, w)  if(((uint32_t)addr % w) != 0){ \
                                            SET_MCAUSE_SAM           \
                                            SET_MTVAL(addr)             \
                                            return;                             \
                                        }

#define CHECK_STORE_FAULT(t, addr) if(t == NULL){ \
                                SET_MCAUSE_SAF    \
                                SET_MTVAL(addr)   \
                                return;     \
                            }

#define CHECK_INSTR_ALIGNED if(((uint32_t)reg[rg_pc] % 4) != 0){ \
                                SET_MCAUSE_IAM                   \
                                SET_MTVAL(reg[rg_pc])            \
                                return;                                     \
                            }

#define CHECK_INSTR_FAULT(instr_addr) if((uint32_t)instr_addr > DRAM_SIZE){ \
                                        SET_MCAUSE_IAF                      \
                                        SET_MTVAL(instr_addr)                     \
                                        check_trap();                       \
                                    }

#define TRAP_UPDATE_MSTATUS { \
                                uint32_t x = csr[csr_mstatus] & 0xF; \
                                x |= (current_pl * 0x80); \
                                x <<= 4; \
                                x |= (csr[csr_mstatus] & 0xFFFFE000); \
                                csr[csr_mstatus] = x; \
                            }

#define PC2MEPC_MTVEC2PC_INTERRUPT { \
                            csr[csr_mepc] = reg[rg_pc] + 4; \
                            reg[rg_pc] = csr[csr_mtvec]; \
                            reg[rg_pc] -= 4;            \
                        }
#define PC2MEPC_MTVEC2PC_EXCEPTION { \
                            csr[csr_mepc] = reg[rg_pc]; \
                            reg[rg_pc] = csr[csr_mtvec]; \
                            reg[rg_pc] -= 4;            \
                        }
#endif //MYVM_TRAP_H
