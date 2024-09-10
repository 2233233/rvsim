//
// Created by Administrator on 2024/8/29.
//

#ifndef MYVM_INSTR_H
#define MYVM_INSTR_H
#include "riscv.h"

#ifdef AMO_SUPPORT
#define AMO_MUTEX_FLAG1 int32_t offset = reg[args->rs1] / 128; \
                        pthread_mutex_t* mutex1 = &amo_mutex_list[offset]; \
                        pthread_mutex_t* mutex2 = &amo_mutex_list[offset + 1]; \
                        switch (reg[args->rs1] % 128) { \
                            case 127: \
                            case 126: \
                            case 125:{ \
                                pthread_mutex_lock(mutex1); \
                                pthread_mutex_lock(mutex2); \
                                mutex_flag_list[offset] = 1; \
                                mutex_flag_list[offset + 1] = 1;

#define AMO_MUTEX_FLAG2         pthread_mutex_unlock(mutex1); \
                                pthread_mutex_unlock(mutex2); \
                                mutex_flag_list[offset] = 0; \
                                mutex_flag_list[offset + 1] = 0; \
                            break; \
                            } \
                            default:{ \
                                pthread_mutex_lock(mutex1); \
                                mutex_flag_list[offset] = 1;


#define AMO_MUTEX_FLAG3         pthread_mutex_unlock(mutex1); \
                                mutex_flag_list[offset] = 0; \
                                break; \
                            } \
                        }
#endif

void ADD(instr_func_args* args);
void AND(instr_func_args* args);
void DIV(instr_func_args* args);
void DIVU(instr_func_args* args);
void MUL(instr_func_args* args);
void MULH(instr_func_args* args);
void MULHSU(instr_func_args* args);
void MULHU(instr_func_args* args);
void OR(instr_func_args* args);
void REM(instr_func_args* args);
void REMU(instr_func_args* args);
void SLL(instr_func_args* args);
void SLT(instr_func_args* args);
void SLTU(instr_func_args* args);
void SRA(instr_func_args* args);
void SRL(instr_func_args* args);
void SUB(instr_func_args* args);
void XOR(instr_func_args* args);
void ADDI(instr_func_args* args);
void ANDI(instr_func_args* args);
void ORI(instr_func_args* args);
void SLLI(instr_func_args* args);
void SLTI(instr_func_args* args);
void SLTIU(instr_func_args* args);
void SRLI(instr_func_args* args);
void XORI(instr_func_args* args);
void JAL(instr_func_args* args);
void BEQ(instr_func_args* args);
void BGE(instr_func_args* args);
void BGEU(instr_func_args* args);
void BLT(instr_func_args* args);
void BLTU(instr_func_args* args);
void BNE(instr_func_args* args);
void FADD_D(instr_func_args* args);
void FADD_S(instr_func_args* args);
void FCLASS_D(instr_func_args* args);
void FCLASS_S(instr_func_args* args);
void FCVT_D_S(instr_func_args* args);
void FCVT_D_W(instr_func_args* args);
void FCVT_D_WU(instr_func_args* args);
void FCVT_S_D(instr_func_args* args);
void FCVT_S_W(instr_func_args* args);
void FCVT_S_WU(instr_func_args* args);
void FCVT_W_D(instr_func_args* args);
void FCVT_WU_D(instr_func_args* args);
void FCVT_W_S(instr_func_args* args);
void FCVT_WU_S(instr_func_args* args);
void FDIV_D(instr_func_args* args);
void FDIV_S(instr_func_args* args);
void FEQ_D(instr_func_args* args);
void FEQ_S(instr_func_args* args);
void FLE_D(instr_func_args* args);
void FLE_S(instr_func_args* args);
void FLT_D(instr_func_args* args);
void FLT_S(instr_func_args* args);
void FMAX_D(instr_func_args* args);
void FMAX_S(instr_func_args* args);
void FMIN_D(instr_func_args* args);
void FMIN_S(instr_func_args* args);
void FMUL_D(instr_func_args* args);
void FMUL_S(instr_func_args* args);
void FMV_W_X(instr_func_args* args);
void FMV_X_W(instr_func_args* args);
void FSGNJ_D(instr_func_args* args);
void FSGNJ_S(instr_func_args* args);
void FSGNJN_D(instr_func_args* args);
void FSGNJN_S(instr_func_args* args);
void FSGNJX_D(instr_func_args* args);
void FSGNJX_S(instr_func_args* args);
void FSQRT_D(instr_func_args* args);
void FSQRT_S(instr_func_args* args);
void FSUB_D(instr_func_args* args);
void FSUB_S(instr_func_args* args);
void MRET(instr_func_args* args);
void SFENCE_VMA(instr_func_args* args);
void SRET(instr_func_args* args);
void WFI(instr_func_args* args);
void CSRRC(instr_func_args* args);
void CSRRCI(instr_func_args* args);
void CSRRS(instr_func_args* args);
void CSRRSI(instr_func_args* args);
void CSRRW(instr_func_args* args);
void CSRRWI(instr_func_args* args);
void EBREAK(instr_func_args* args);
void ECALL(instr_func_args* args);
void FENCE(instr_func_args* args);
void FENCE_I(instr_func_args* args);
void FLD(instr_func_args* args);
void FLW(instr_func_args* args);
void JALR(instr_func_args* args);
void LB(instr_func_args* args);
void LBU(instr_func_args* args);
void LH(instr_func_args* args);
void LHU(instr_func_args* args);
void LW(instr_func_args* args);
void FSD(instr_func_args* args);
void FSW(instr_func_args* args);
void SB(instr_func_args* args);
void SH(instr_func_args* args);
void SW(instr_func_args* args);
void AUIPC(instr_func_args* args);
void LUI(instr_func_args* args);
void AMOADD_W(instr_func_args* args);
void AMOAND_W(instr_func_args* args);
void AMOMAX_W(instr_func_args* args);
void AMOMAXU_W(instr_func_args* args);
void AMOMIN_W(instr_func_args* args);
void AMOMINU_W(instr_func_args* args);
void AMOOR_W(instr_func_args* args);
void AMOSWAP_W(instr_func_args* args);
void AMOXOR_W(instr_func_args* args);
void LR_W(instr_func_args* args);
void SC_W(instr_func_args* args);
#endif //MYVM_INSTR_H
