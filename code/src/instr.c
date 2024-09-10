#include "header/instr.h"
#include "header/trap.h"
extern int running;
extern uint8_t* dram;
extern uint8_t* uart0_reg;
extern uint8_t* plic_reg;
extern uint32_t* clint_reg;
extern int32_t* reg;
extern float_t* freg;
extern uint32_t* csr;
extern bool in_exception;
#ifdef AMO_SUPPORT
extern bits reservation_map;
extern pthread_mutex_t* amo_mutex_list;
extern bool* mutex_flag_list;
#endif

extern enum Privileglevel current_pl;
extern uint32_t tmp_claim_complete;

//double round_for_rm(double value, uint8_t rm){
//    //TODO:舍入机制
//    switch (rm) {
//        case 0:{
//            //RNE
//
//        }
//        case 1:{
//            //RTZ
//        }
//        case 2:{
//            //RDN
//        }
//        case 3:{
//            //RUP
//        }
//        case 4:{
//            //RMM
//            return round(value);
//        }
//        case 7:{
//            //fcsr
//        }
//        default:{
//
//        }
//    }
//}

//逻辑实现内存映射
void* select_memory(uint32_t address){

    if(address >= DRAM_START){
        return dram + (address - DRAM_START);
    }else if(address < 0x2000000 && address > 0x0){
        //其他
        return dram + address;
    }else if(address >= UART0_START && address <= UART0_END){
        uint8_t offset = address - UART0_START;
        return uart0_reg + offset;
    }else if(address >= PLIC_REG_START && address < PLIC_REG_END){
        uint32_t t1 = address - PLIC_REG_START;
        if(t1 <= (53 * 4)){
            //priority
            return plic_reg + t1;
        }
        uint32_t t2 = t1 - 0x1000;
        if(t2 <= 4){
            //pending
            return plic_reg + plic_pending1 * 4 + t2;
        }
        uint32_t t3 = t1 - 0x2000;
        if(t3 <= 4){
            //enable
            return plic_reg + plic_enable1 * 4 + t3;
        }
        uint32_t t4 = t1 - 0x200000;
        if(t4 == 0){
            //threshold
            return plic_reg + plic_threshold * 4;
        }
        //claim/complete
        return &tmp_claim_complete;
    }else if(address >= CLINT_REG_START && address <= CLINT_REG_END){
        uint32_t t1 = address - CLINT_REG_START;
        if(t1 == 0){
            //MSIP
            return clint_reg;
        }
        if((t1 - 0xBFF8) == 0){
            //mtime
            return clint_reg + clint_mtime1;
        }else if((t1 - 0xBFF8) == 4){
            return clint_reg + clint_mtime2;
        }
        //mtimecmp
        if((t1 - 0x4000) == 0){
            return clint_reg + clint_mtimecmp1;
        }else if((t1 - 0x4000) == 4){
            return clint_reg + clint_mtimecmp2;
        }
    }
    return NULL;
}


void ADD(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] + reg[args->rs2];
}

void AND(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] & reg[args->rs2];
}
void DIV(instr_func_args* args){
    if(reg[args->rs2] == 0){
        //riscv没有除零异常
        reg[args->rd] = 0;
        csr[csr_fcsr] |= 0x8;
        return;
    }
    reg[args->rd] = reg[args->rs1] / reg[args->rs2];
}
void DIVU(instr_func_args* args){
    if(reg[args->rs2] == 0){
        reg[args->rd] = 0;
        csr[csr_fcsr] |= 0x8;
        return;
    }
    reg[args->rd] = (uint32_t)reg[args->rs1] / (uint32_t)reg[args->rs2];
}
void MUL(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] * reg[args->rs2];
}
void MULH(instr_func_args* args){
    reg[args->rd] = ((int64_t)reg[args->rs1] * (int64_t)reg[args->rs2]) >> 32;
}
void MULHSU(instr_func_args* args){
    reg[args->rd] = ((int64_t)reg[args->rs1] * (int64_t)(uint32_t)reg[args->rs2]) >> 32;
}
void MULHU(instr_func_args* args){
    reg[args->rd] = ((int64_t)(uint32_t)reg[args->rs1] * (int64_t)(uint32_t)reg[args->rs2]) >> 32;
}
void OR(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] | reg[args->rs2];
}
void REM(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] % reg[args->rs2];
}
void REMU(instr_func_args* args){
    reg[args->rd] = (uint32_t)reg[args->rs1] % (uint32_t)reg[args->rs2];
}
void SLL(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] << (reg[args->rs2] & 0x1F);
}
void SLT(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] < reg[args->rs2] ? 1 : 0;
}
void SLTU(instr_func_args* args){
    reg[args->rd] = (uint32_t)reg[args->rs1] < (uint32_t)reg[args->rs2] ? 1 : 0;
}
void SRA(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] >> (reg[args->rs2] & 0x1F);
}
void SRL(instr_func_args* args){
    reg[args->rd] = (uint32_t)reg[args->rs1] >> (reg[args->rs2] & 0x1F);
}
void SUB(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] - reg[args->rs2];
}
void XOR(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] ^ reg[args->rs2];
}
void ADDI(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] + args->imm16;
}
void ANDI(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] & args->imm16;
}
void ORI(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] | args->imm16;
}
void SLLI(instr_func_args* args){
    int8_t shamt = args->imm16;
    if(shamt & 0x20) return;
    reg[args->rd] = reg[args->rs1] << shamt;
}
void SLTI(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] < args->imm16 ? 1 : 0;
}
void SLTIU(instr_func_args* args){
    reg[args->rd] = (uint32_t)reg[args->rs1] < (int32_t)args->imm16 ? 1 : 0;
}
void SRLI(instr_func_args* args){
    int8_t shamt = args->imm16;
    if(shamt & 0x20) return;
    reg[args->rd] = (uint32_t)reg[args->rs1] >> shamt;
}
void XORI(instr_func_args* args){
    reg[args->rd] = reg[args->rs1] ^ args->imm16;
}
void JAL(instr_func_args* args){
    reg[args->rd] = reg[rg_pc] + 4;
    reg[rg_pc] += (int32_t)args->imm32;
    reg[rg_pc] -= 4;//抵消之后的pc+4
    CHECK_INSTR_ALIGNED
}
void BEQ(instr_func_args* args){
    if(reg[args->rs1] == reg[args->rs2]){
        reg[rg_pc] += args->imm16;
        reg[rg_pc] -= 4;
        CHECK_INSTR_ALIGNED
    }
}
void BGE(instr_func_args* args){
    if(reg[args->rs1] >= reg[args->rs2]){
        reg[rg_pc] += args->imm16;
        reg[rg_pc] -= 4;
        CHECK_INSTR_ALIGNED
    }
}
void BGEU(instr_func_args* args){
    if((uint32_t)reg[args->rs1] >= (uint32_t)reg[args->rs2]){
        reg[rg_pc] += args->imm16;
        reg[rg_pc] -= 4;
        CHECK_INSTR_ALIGNED
    }

}
void BLT(instr_func_args* args){
    if(reg[args->rs1] < reg[args->rs2]){
        reg[rg_pc] += args->imm16;
        reg[rg_pc] -= 4;
        CHECK_INSTR_ALIGNED
    }
}
void BLTU(instr_func_args* args){
    if((uint32_t)reg[args->rs1] < (uint32_t)reg[args->rs2]){
        reg[rg_pc] += args->imm16;
        reg[rg_pc] -= 4;
        CHECK_INSTR_ALIGNED
    }
}
void BNE(instr_func_args* args){
    if(reg[args->rs1] != reg[args->rs2]){
        reg[rg_pc] += args->imm16;
        reg[rg_pc] -= 4;
        CHECK_INSTR_ALIGNED
    }
}

void FADD_D(instr_func_args* args){}
void FADD_S(instr_func_args* args){
    freg[args->rd] = freg[args->rs1] + freg[args->rs2];
}
void FCLASS_D(instr_func_args* args){}
void FCLASS_S(instr_func_args* args) {
    uint32_t* t = (uint32_t*) &freg[args->rs1];
    uint32_t v = *t;
    uint8_t exp = (v >> 23) & 0xFF;
    bool flag = v & 0x10000000;
    uint32_t man = v & 0x7FFFFF;
    if((v & 0xFFFFFFF) == 0x0){
        if(flag){
            //-0
            reg[args->rd] = 0x8;
        }else{
            //+0
            reg[args->rd] = 0x10;
        }
    }else if (exp == 0x00) {
        if (flag) {
            //负非规格化
            reg[args->rd] = 0x4;
        } else {
            //正非规格化
            reg[args->rd] = 0x20;
        }
    } else if (exp == 0xFF) {
        if (man == 0x0) {
            if (flag) {
                //-inf
                reg[args->rd] = 0x1;
            } else {
                //inf
                reg[args->rd] = 0x80;

            }
        } else {
            if (v & 0x400000) {
                //qnan
                reg[args->rd] = 0x200;

            } else {
                //snan
                reg[args->rd] = 0x100;
            }
        }

    } else {
        if (flag) {
            //负规格化
            reg[args->rd] = 0x2;
        }else{
            //正规格化
            reg[args->rd] = 0x40;
        }
    }
}

void FCVT_D_S(instr_func_args* args){}
void FCVT_D_W(instr_func_args* args){}
void FCVT_D_WU(instr_func_args* args){}
void FCVT_S_D(instr_func_args* args){}
void FCVT_S_W(instr_func_args* args){
    freg[args->rd] = (float_t)reg[args->rs1];
}
void FCVT_S_WU(instr_func_args* args){
    freg[args->rd] = (float_t)(uint32_t)reg[args->rs1];
}
void FCVT_W_D(instr_func_args* args){}
void FCVT_WU_D(instr_func_args* args){}
void FCVT_W_S(instr_func_args* args){
    reg[args->rd] = (int32_t)freg[args->rs1];
}
void FCVT_WU_S(instr_func_args* args){
    reg[args->rd] = (uint32_t)freg[args->rs1];
}
void FDIV_D(instr_func_args* args){}
void FDIV_S(instr_func_args* args){
    if(reg[args->rs2] == 0){
        reg[args->rd] = 0;
        csr[csr_fcsr] |= 0x8;
        return;
    }
    freg[args->rd] = freg[args->rs1] / freg[args->rs2];
}
void FEQ_D(instr_func_args* args){}
void FEQ_S(instr_func_args* args){
    if(freg[args->rs1] == freg[args->rs2]){
        reg[args->rd] = 1;
    }else{
        reg[args->rd] = 0;
    }
}
void FLE_D(instr_func_args* args){}
void FLE_S(instr_func_args* args){
    if(freg[args->rs1] <= freg[args->rs2]){
        reg[args->rd] = 1;
    }else{
        reg[args->rd] = 0;
    }
}
void FLT_D(instr_func_args* args){}
void FLT_S(instr_func_args* args){
    if(freg[args->rs1] < freg[args->rs2]){
        reg[args->rd] = 1;
    }else{
        reg[args->rd] = 0;
    }
}
void FMAX_D(instr_func_args* args){}
void FMAX_S(instr_func_args* args){
    freg[args->rd] = fmax(freg[args->rs1], freg[args->rs2]);
}
void FMIN_D(instr_func_args* args){}
void FMIN_S(instr_func_args* args){
    freg[args->rd] = fmin(freg[args->rs1], freg[args->rs2]);
}
void FMUL_D(instr_func_args* args){}
void FMUL_S(instr_func_args* args){
    freg[args->rd] = freg[args->rs1] * freg[args->rs2];
}
void FMV_W_X(instr_func_args* args){
    memcpy(&freg[args->rs1], &reg[args->rd], sizeof(float_t));
}
void FMV_X_W(instr_func_args* args){
    memcpy(&reg[args->rd], &freg[args->rs1], sizeof(uint32_t));
}
void FSGNJ_D(instr_func_args* args){}
void FSGNJ_S(instr_func_args* args){
    uint32_t* rs1 = (uint32_t*)&freg[args->rs1];
    uint32_t* rs2 = (uint32_t*)&freg[args->rs2];
    uint32_t value = *rs1 & 0x7FFFFFFF;
    uint32_t flag = *rs2 & 0x80000000;
    if(flag){
        uint32_t i = value | 0x80000000;
        freg[args->rd] = *(float_t*)&i;
    }else{
        uint32_t i = value & 0x0FFFFFFF;
        freg[args->rd] = *(float_t*)&i;
    }
}
void FSGNJN_D(instr_func_args* args){}
void FSGNJN_S(instr_func_args* args){
    uint32_t* rs1 = (uint32_t*)&freg[args->rs1];
    uint32_t* rs2 = (uint32_t*)&freg[args->rs2];
    uint32_t value = *rs1 & 0x7FFFFFFF;
    uint32_t flag = *rs2 & 0x80000000;
    if(flag){
        uint32_t i = value & 0x0FFFFFFF;
        freg[args->rd] = *(float_t*)&i;
    }else{
        uint32_t i = value | 0x80000000;
        freg[args->rd] = *(float_t*)&i;
    }
}
void FSGNJX_D(instr_func_args* args){}
void FSGNJX_S(instr_func_args* args){
    uint32_t* rs1 = (uint32_t*)&freg[args->rs1];
    uint32_t* rs2 = (uint32_t*)&freg[args->rs2];
    uint32_t value = *rs1 & 0x7FFFFFFF;
    uint32_t flag1 = *rs1 & 0x80000000;
    uint32_t flag2 = *rs2 & 0x80000000;

    if(flag1 ^ flag2){
        uint32_t i = value | 0x80000000;
        freg[args->rd] = *(float_t*)&i;
    }else{
        uint32_t i = value & 0x0FFFFFFF;
        freg[args->rd] = *(float_t*)&i;
    }
}
void FSQRT_D(instr_func_args* args){}
void FSQRT_S(instr_func_args* args){
    freg[args->rd] = sqrt(freg[args->rs1]);}
void FSUB_D(instr_func_args* args){}
void FSUB_S(instr_func_args* args){
    freg[args->rd] = freg[args->rs1] - freg[args->rs2];
}

void MRET(instr_func_args* args){
    //恢复pc
    reg[rg_pc] = csr[csr_mepc];
    reg[rg_pc] -= 4;
    //恢复mstatus.MIE
    csr[csr_mstatus] |= (csr[csr_mstatus] & 0xF0) >> 4;
    csr[csr_mstatus] |= 0xF0;
    //恢复特权级
    current_pl = (csr[csr_mstatus] & 0x1800) >> 11;
    //恢复mcause
    csr[csr_mcause] = -1;
    in_exception = false;

}

void SFENCE_VMA(instr_func_args* args){}
void SRET(instr_func_args* args){
    reg[rg_pc] = csr[csr_sepc];
    reg[rg_pc] -= 4;
    csr[csr_sstatus] |= (csr[csr_sstatus] & 0x80) >> 4;
    csr[csr_sstatus] |= 0x80;
    current_pl = (csr[csr_sstatus] & 0x1800) >> 11;
    csr[csr_scause] = -1;
}
void WFI(instr_func_args* args){
    while((csr[csr_mie] & csr[csr_mip]) == 0);
}
void CSRRC(instr_func_args* args){
    uint32_t* t = &csr[args->imm16u];
    reg[args->rd] = *t;
    *t &= ~reg[args->rs1];
}
void CSRRCI(instr_func_args* args){
    uint32_t* t = &csr[args->imm16u];
    reg[args->rd] = *t;
    *t &= ~args->rs1;
}
void CSRRS(instr_func_args* args){
    uint32_t* t = &csr[args->imm16u];
    reg[args->rd] = *t;
    *t |= reg[args->rs1];
}
void CSRRSI(instr_func_args* args){
    uint32_t* t = &csr[args->imm16u];
    reg[args->rd] = *t;
    *t |= args->rs1;
}
void CSRRW(instr_func_args* args){
    uint32_t* t = &csr[args->imm16u];
    reg[args->rd] = *t;
    *t = reg[args->rs1];
}
void CSRRWI(instr_func_args* args){
    uint32_t* t = &csr[args->imm16u];
    reg[args->rd] = *t;
    *t = args->rs1;
}
void EBREAK(instr_func_args* args){
    //TODO:EBREAK 触发异常，进入debug模式
}
void ECALL(instr_func_args* args){
    //a7为系统调用编号
    SET_MCAUSE_ECLL(current_pl)
    csr[csr_mstatus] &= 0xFFFFE0FF;
    csr[csr_mstatus] |= (current_pl << 11);
    current_pl = M_MODE;
}
void FENCE(instr_func_args* args){}
void FENCE_I(instr_func_args* args){}
void FLD(instr_func_args* args){}
void FLW(instr_func_args* args){

    float_t* t = select_memory(reg[args->rs1] + (int32_t)args->imm16);
    freg[args->rd] = *t;
}
void JALR(instr_func_args* args){

    reg[args->rd] = reg[rg_pc] + 4;
    reg[rg_pc] = (reg[args->rs1] + args->imm16) & ~1;
    reg[rg_pc] -= 4;
    CHECK_INSTR_ALIGNED

}
void LB(instr_func_args* args){
    uint32_t addr = reg[args->rs1] + args->imm16;
    int8_t* t = select_memory(addr);
    CHECK_LOAD_FAULT(t, addr)
    reg[args->rd] = *t;

}
void LBU(instr_func_args* args){
    uint32_t addr = reg[args->rs1] + args->imm16;
    uint8_t* t = select_memory(addr);
    CHECK_LOAD_FAULT(t, addr)
    reg[args->rd] = *t;
}
void LH(instr_func_args* args){
    uint32_t addr = reg[args->rs1] + args->imm16;
    CHECK_LOAD_ALIGNED(addr, 2)
    int16_t* i = select_memory(addr);
    CHECK_LOAD_FAULT(i, addr)
    reg[args->rd] = *i;
}
void LHU(instr_func_args* args){
    uint32_t addr = reg[args->rs1] + args->imm16;
    CHECK_LOAD_ALIGNED(addr, 2)
    uint16_t* i = select_memory(addr);
    CHECK_LOAD_FAULT(i, addr)
    reg[args->rd] = *i;
}
void LW(instr_func_args* args){
    uint32_t addr = reg[args->rs1] + args->imm16;
    CHECK_LOAD_ALIGNED(addr, 4)
    int32_t* i = select_memory(addr);
    CHECK_LOAD_FAULT(i, addr)
    reg[args->rd] = *i;
}
void FSD(instr_func_args* args){}
void FSW(instr_func_args* args){

    uint32_t offset = reg[args->rs1] + (int32_t)args->imm16;
    CHECK_STORE_ALIGNED(offset, 4)
    float_t* t = select_memory(offset);
    CHECK_STORE_FAULT(t, offset)
    *t = freg[args->rs2];
}
void SB(instr_func_args* args){

    int32_t offset = reg[args->rs1] + (int32_t)args->imm16;

#ifdef AMO_SUPPORT
    while(mutex_flag_list[offset / 128]);
#endif

    uint8_t* t = select_memory(offset);

    CHECK_STORE_FAULT(t, offset)
    *t = (uint8_t)reg[args->rs2];

#ifdef AMO_SUPPORT
    bit_set(reservation_map, offset / 4, 0);
#endif

}
void SH(instr_func_args* args){

    uint16_t v = (uint16_t)reg[args->rs2];
    int32_t offset = reg[args->rs1] + (int32_t)args->imm16;
    CHECK_STORE_ALIGNED(offset, 2)
    uint16_t* i = select_memory(offset);
    CHECK_STORE_FAULT(i, offset)

#ifdef AMO_SUPPORT
    while(mutex_flag_list[offset / 128]);
#endif
    *i = v;
#ifdef AMO_SUPPORT
    switch (offset % 4) {
        case 0:
        case 1:
        case 2:{
            bit_set(reservation_map, offset / 4, 0);
            break;
        }
        case 3:{
            bit_set(reservation_map, offset / 4, 0);
            bit_set(reservation_map, (offset / 4) + 1, 0);
            break;
        }
    }
#endif


}
void SW(instr_func_args* args){
    int32_t offset = reg[args->rs1] + (int32_t)args->imm16;
    CHECK_STORE_ALIGNED(offset, 4)
    int32_t* i = select_memory(offset);
    CHECK_STORE_FAULT(i, offset)

#ifdef AMO_SUPPORT
    while(mutex_flag_list[offset / 128]);
#endif

    *i = reg[args->rs2];
#ifdef AMO_SUPPORT
    switch (offset % 4) {
        case 0:{
            bit_set(reservation_map, offset / 4, 0);
            break;
        }
        case 1:
        case 2:
        case 3:{
            bit_set(reservation_map, offset / 4, 0);
            bit_set(reservation_map, (offset / 4) + 1, 0);
            break;
        }
    }
#endif

}
void AUIPC(instr_func_args* args){
 
    reg[args->rd] = reg[rg_pc] + args->imm32;
}
void LUI(instr_func_args* args){
   
    reg[args->rd] = args->imm32;
}
void AMOADD_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);
            reg[args->rd] = *t;
            *t = *t + reg[args->rs2];


        AMO_MUTEX_FLAG2;

            int32_t* t= (int32_t*)select_memory(args->rs1);
            reg[args->rd] = *t;
            *t = *t + reg[args->rs2];

    AMO_MUTEX_FLAG3;
#endif

}
void AMOAND_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);
            reg[args->rd] = *t;
            *t = reg[args->rs2] & *t;


        AMO_MUTEX_FLAG2;

            int32_t* t= (int32_t*)select_memory(args->rs1);
            reg[args->rd] = *t;
            *t = reg[args->rs2] & *t;

    AMO_MUTEX_FLAG3;
#endif
}
void AMOMAX_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] > *t ? reg[args->rs2] : *t;

        AMO_MUTEX_FLAG2;
            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] > *t ? reg[args->rs2] : *t;
    AMO_MUTEX_FLAG3;
#endif

}
void AMOMAXU_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            uint32_t* t= (uint32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] > *t ? reg[args->rs2] : *t;

        AMO_MUTEX_FLAG2;

            uint32_t* t= (uint32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] > *t ? reg[args->rs2] : *t;
    AMO_MUTEX_FLAG3;
#endif
}
void AMOMIN_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] < *t ? reg[args->rs2] : *t;

        AMO_MUTEX_FLAG2;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] < *t ? reg[args->rs2] : *t;
    AMO_MUTEX_FLAG3;
#endif
}
void AMOMINU_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            uint32_t* t= (uint32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] < *t ? reg[args->rs2] : *t;

        AMO_MUTEX_FLAG2;
            uint32_t* t= (uint32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] < *t ? reg[args->rs2] : *t;
    AMO_MUTEX_FLAG3;
#endif
}
void AMOOR_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] | *t;

        AMO_MUTEX_FLAG2;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] | *t;

    AMO_MUTEX_FLAG3;
#endif
}
void AMOSWAP_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2];

        AMO_MUTEX_FLAG2;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2];

    AMO_MUTEX_FLAG3;
#endif
}
void AMOXOR_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    AMO_MUTEX_FLAG1;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] ^ *t;

        AMO_MUTEX_FLAG2;

            int32_t* t= (int32_t*)select_memory(args->rs1);;
            reg[args->rd] = *t;
            *t = reg[args->rs2] ^ *t;

    AMO_MUTEX_FLAG3;
#endif
}
void LR_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    int32_t* t = select_memory(reg[args->rs1]);
    reg[args->rd] = *t;
    bit_set(reservation_map, reg[args->rs1] / 4, 1);
#endif
}
void SC_W(instr_func_args* args){
#ifdef AMO_SUPPORT
    if(bit_get(reservation_map, reg[args->rs1] / 4)){
        int32_t* t = select_memory(reg[args->rs1]);
        *t = reg[args->rs2];
        reg[args->rd] = 0;
    }else{
        reg[args->rd] = 1;
    }
    bit_set(reservation_map, reg[args->rs1] / 4, 0);
#endif
}