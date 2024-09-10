//
// Created by Administrator on 2024/8/19.
//

#ifndef MYVM_RISCV_H
#define MYVM_RISCV_H

#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#define HAVE_STRUCT_TIMESPEC
#include "pthread.h"
#include "bitmap.h"
/* windows only */
#include <Windows.h>
#include <conio.h>  // _kbhit


// 通用寄存器
enum
{
    rg_zero = 0,
    rg_ra, // return address
    rg_sp, // stack point
    rg_gp, // global pointer
    rg_tp, // thread pointer
    rg_t0, rg_t1, rg_t2, // temporary
    rg_s0, // saved register,frame pointer
    rg_s1, // saved register
    rg_a0, rg_a1, // function argument,return value
    rg_a2, rg_a3, rg_a4, rg_a5, rg_a6, rg_a7, // function argument
    rg_s2, rg_s3, rg_s4, rg_s5, rg_s6, rg_s7, rg_s8, rg_s9, rg_s10, rg_s11, //saved register
    rg_t3, rg_t4, rg_t5, rg_t6, // temporary
    rg_pc,
    rg_count
};
enum{
    rf_t0 = 0, rf_t1, rf_t2, rf_t3, rf_t4, rf_t5, rf_t6, rf_t7, //temporary variables
    rf_s0, rf_s1, // saved variables
    rf_a0, rf_a1, // function arguments, return values
    rf_a2, rf_a3, rf_a4, rf_a5, rf_a6, rf_a7, // function arguments
    rf_s2, rf_s3, rf_s4, rf_s5, rf_s6, rf_s7, rf_s8, rf_s9, rf_s10, rf_s11, // saved variables
    rf_t8, rf_t9, rf_t10, rf_t11, // temporary variables
    rf_count
};

enum {
    csr_mvendorid = 0xF11,
    csr_marchid = 0xF12,
    csr_mimpid = 0xF13,
    csr_mhartid = 0xF14, // hart id

    csr_mstatus = 0x300,
    csr_misa = 0x301,
    csr_medeleg = 0x302,
    csr_mideleg = 0x303,
    csr_mie = 0x304,
    csr_mtvec = 0x305,
    csr_mcounteren = 0x306,

    csr_mscratch = 0x340,
    csr_mepc = 0x341,
    csr_mcause = 0x342,
    csr_mtval = 0x343,
    csr_mip = 0x344,
    //pmpcfg
    //pmpaddr
    csr_mcycle = 0xB00,
    csr_minstret = 0xB02,
    //mhpmcounter
    csr_mcycleh = 0xB80,
    csr_minstreth = 0xB82,
    //mhpmcounter h
    csr_mcountinhibit = 0x320,
    //mhpmevent
    csr_tselect = 0x7A0,
    //tdata
    csr_dcsr = 0x7B0,
    csr_dpc = 0x7B1,
    csr_dscratch0 = 0x7B2,
    csr_dscratch1 = 0x7B3,

};
enum {
    csr_sstatus = 0x100,
    csr_sedeleg = 0x102,
    csr_sideleg = 0x103,
    csr_sie = 0x104,
    csr_stvec = 0x105,
    csr_scounteren = 0x106,
    csr_sscratch = 0x140,
    csr_sepc = 0x141,
    csr_scause = 0x142,
    csr_stval = 0x143,
    csr_sip = 0x144,
    csr_satp = 0x180,
};
enum{
    csr_ustatus = 0x000,
    csr_uie = 0x004,
    csr_utvec = 0x005,
    csr_uscratch = 0x040,
    csr_uepc = 0x041,
    csr_ucause = 0x042,
    csr_utval = 0x043,
    csr_uip = 0x044,
    csr_fflags = 0x001,
    csr_frm = 0x002,
    csr_fcsr = 0x003,
    csr_cycle = 0xC00,
    csr_time = 0xC01,
    csr_instret = 0xC02,
    //hpmcounter
    csr_cycleh = 0xC80,
    csr_timeh = 0xC81,
    csr_instreth = 0xC82,
    //hpmcounter h
};

enum Privileglevel{
    M_MODE = 3,
    H_MODE = 2,
    S_MODE = 1,
    U_MODE = 0,
};

enum{
    plic_priority0 = 0,//...
    plic_pending1 = 54, plic_pending2 = 55,
    plic_enable1 = 56, plic_enable2 = 57,
    plic_threshold = 58,
    plic_claim_complete = 59,
};

enum{
    clint_MSIP = 0,
    clint_mtime1 = 1,
    clint_mtime2 = 2,
    clint_mtimecmp1 = 3,
    clint_mtimecmp2 = 4,
};

//#define AMO_SUPPORT

#define DRAM_SIZE 0x8000000
#define DRAM_START 0x80000000L
#define UART0_SIZE 0x100
#define UART0_START 0x10000000L
#define UART0_END (UART0_START + UART0_SIZE)

//1-53中断源 0号预留不用 1个hart 54+2+2+1+1
#define PLIC_REG_COUNT 60
#define PLIC_REG_START 0x0C000000L
#define PLIC_REG_END 0x10000000L

//MSIP mtime mtimecmp
#define CLINT_REG_COUNT 5
#define CLINT_REG_START 0x2000000L
#define CLINT_REG_END 0x2010000L

#define STACK_SIZE 1024
#define CSR_COUNT 0x1000
//一个mutex对应128字节
#define AMO_MEMORY_MUTEX_COUNT 0x200000
#define SUPPORT_INSTR_COUNT 112
#define GROUP_INSTR_COUNT 19

#define R_TYPE_MASK 0xFE00707F
#define R_F_RM_DO_TYPE_MASK 0xFE00007F
#define R_F_RM_SO_TYPE_MASK 0xFFF0007F
#define R_F_NRM_DO_TYPE_MASK 0xFE00707F
#define R_F_NRM_SO_TYPE_MASK 0xFFF0707F
#define I_TYPE_MASK 0x0000707F
#define SYS_TYPE_MASK 0xFFFFFFFF
#define S_TYPE_MASK 0x0000707F
#define B_TYPE_MASK 0x0000707F
#define U_TYPE_MASK 0x0000007F
#define J_TYPE_MASK 0x0000007F


void disable_input_buffering();
void init_instr_map();
void uart0_print_func();
void clint_func();
void plic_func();
void init_disasm_instr_map();

typedef struct{
    uint8_t rs2;
    uint8_t rs1;
    uint8_t rd;
    int16_t imm16;
    int32_t imm32;
    uint16_t imm16u;
}instr_func_args;

typedef struct {
    uint32_t instr_mask;
    void (*func)(instr_func_args* args);
}instr_map_entry;

typedef struct{
    uint32_t mask;
    void (*args_build_func)(uint32_t instr, instr_func_args* args);
}mask_argsfunc;



#endif //MYVM_RISCV_H
