#include "header/riscv.h"
#include "header/instr.h"
extern uint8_t* opcode_list;
extern uint8_t* group_index;
extern mask_argsfunc* group_mask_argsfunc;
extern instr_map_entry* instr_map;


void R_instr_args_build(uint32_t instr, instr_func_args* args){
    args->rs1 = (instr >> 15) & 0x1F;
    args->rs2 = (instr >> 20) & 0x1F;
    args->rd = (instr >> 7) & 0x1F;
}

void R_F_instr_args_build(uint32_t instr, instr_func_args* args){
    args->rs1 = (instr >> 15) & 0x1F;
    args->rs2 = (instr >> 20) & 0x1F;
    args->rd = (instr >> 7) & 0x1F;
    //imm16里放rm
    args->imm16 = (instr >> 12) & 0x7;
}

void I_instr_args_build(uint32_t instr, instr_func_args* args){
    args->rs1 = (instr >> 15) & 0x1F;
    args->rd = (instr >> 7) & 0x1F;
    args->imm16u = instr >> 20;
    args->imm16 = (int32_t)instr >> 20;
}

void S_instr_args_build(uint32_t instr, instr_func_args* args){
    uint16_t offset1 = (instr >> 7) & 0x1F;
    args->rs1 = (instr >> 15) & 0x1F;
    args->rs2 = (instr >> 20) & 0x1F;
    int16_t offset2 = ((int32_t)instr >> 20) & 0xFFE0;
    //清空符号扩展
    args->imm16u = (offset2 & 0x07FF) | offset1;
    //有符号扩展
    args->imm16 = offset2 | offset1;
}

void B_instr_args_build(uint32_t instr, instr_func_args* args){
    args->rs1 = (instr >> 15) & 0x1F;
    args->rs2 = (instr >> 20) & 0x1F;
    uint8_t imm1 = (instr >> 7) & 0x1F;
    uint8_t imm2 = (instr >> 25) & 0x7F;

    uint16_t i = imm1 & 0x1E;
    i |= ((imm2 & 0x3F) | ((imm1 & 1) << 6)) << 5;
    i |= (imm2 & 0x40) << 6;
    args->imm16u = i;
    if(imm2 & 0x40)
        i |= 0xE000;
    args->imm16 = i;
}

void U_instr_args_build(uint32_t instr, instr_func_args* args){
    args->rd = (instr >> 7) & 0x1F;
    args->imm32 = instr & 0xFFFFF000;
}

void J_instr_args_build(uint32_t instr, instr_func_args* args){
    args->rd = (instr >> 7) & 0x1F;
    uint32_t imm = instr >> 12;
    uint32_t tmp = (imm & 0x80000) | ((imm & 0x100) << 2);
    // 19:12
    tmp |= (imm & 0xFF) << 11;
    // 10:1
    tmp |= (imm & 0x7FE00) >> 9;
    tmp <<= 1;
    //手动加符号扩展
    if(tmp & 0x100000)
        args->imm32 = tmp | 0xFFE00000;
    else
        args->imm32 = tmp;
}

void init_instr_map(){
    //各个组的opcode
    opcode_list[0] = 0x33;
    opcode_list[1] = 0x13;
    opcode_list[2] = 0x6F;
    opcode_list[3] = 0x63;
    //浮点组细分为4个
    opcode_list[4] = 0x53;
    opcode_list[5] = 0x53;
    opcode_list[6] = 0x53;
    opcode_list[7] = 0x53;
    //系统状态组细分为2个
    opcode_list[8] = 0x73;
    opcode_list[9] = 0x73;
    //
    opcode_list[10] = 0x0F;
    opcode_list[11] = 0x07;
    opcode_list[12] = 0x67;
    opcode_list[13] = 0x03;
    opcode_list[14] = 0x27;
    opcode_list[15] = 0x23;
    opcode_list[16] = 0x17;
    opcode_list[17] = 0x37;
    opcode_list[18] = 0x2F;

    //组索引
    group_index[0] = 0;
    group_index[1] = 18;
    group_index[2] = 26;
    group_index[3] = 27;
    //
    group_index[4] = 33;
    group_index[5] = 45;
    group_index[6] = 53;
    group_index[7] = 69;
    //
    group_index[8] = 73;
    group_index[9] = 79;
    group_index[10] = 84;
    group_index[11] = 86;
    group_index[12] = 88;
    group_index[13] = 89;
    group_index[14] = 94;
    group_index[15] = 96;
    group_index[16] = 99;
    group_index[17] = 100;
    group_index[18] = 101;

    group_mask_argsfunc[0].mask = R_TYPE_MASK;
    group_mask_argsfunc[1].mask = I_TYPE_MASK;
    group_mask_argsfunc[2].mask = J_TYPE_MASK;
    group_mask_argsfunc[3].mask = B_TYPE_MASK;
    group_mask_argsfunc[4].mask = R_F_RM_DO_TYPE_MASK;
    group_mask_argsfunc[5].mask = R_F_RM_SO_TYPE_MASK;
    group_mask_argsfunc[6].mask = R_F_NRM_DO_TYPE_MASK;
    group_mask_argsfunc[7].mask = R_F_NRM_SO_TYPE_MASK;
    group_mask_argsfunc[8].mask = I_TYPE_MASK;
    group_mask_argsfunc[9].mask = SYS_TYPE_MASK;
    group_mask_argsfunc[10].mask = I_TYPE_MASK;
    group_mask_argsfunc[11].mask = I_TYPE_MASK;
    group_mask_argsfunc[12].mask = I_TYPE_MASK;
    group_mask_argsfunc[13].mask = I_TYPE_MASK;
    group_mask_argsfunc[14].mask = S_TYPE_MASK;
    group_mask_argsfunc[15].mask = S_TYPE_MASK;
    group_mask_argsfunc[16].mask = U_TYPE_MASK;
    group_mask_argsfunc[17].mask = U_TYPE_MASK;
    group_mask_argsfunc[18].mask = R_TYPE_MASK;

    group_mask_argsfunc[0].args_build_func = R_instr_args_build;
    group_mask_argsfunc[1].args_build_func = I_instr_args_build;
    group_mask_argsfunc[2].args_build_func = J_instr_args_build;
    group_mask_argsfunc[3].args_build_func = B_instr_args_build;
    group_mask_argsfunc[4].args_build_func = R_F_instr_args_build;
    group_mask_argsfunc[5].args_build_func = R_F_instr_args_build;
    group_mask_argsfunc[6].args_build_func = R_F_instr_args_build;
    group_mask_argsfunc[7].args_build_func = R_F_instr_args_build;
    group_mask_argsfunc[8].args_build_func = I_instr_args_build;
    group_mask_argsfunc[9].args_build_func = I_instr_args_build;
    group_mask_argsfunc[10].args_build_func = I_instr_args_build;
    group_mask_argsfunc[11].args_build_func = I_instr_args_build;
    group_mask_argsfunc[12].args_build_func = I_instr_args_build;
    group_mask_argsfunc[13].args_build_func = I_instr_args_build;
    group_mask_argsfunc[14].args_build_func = S_instr_args_build;
    group_mask_argsfunc[15].args_build_func = S_instr_args_build;
    group_mask_argsfunc[16].args_build_func = U_instr_args_build;
    group_mask_argsfunc[17].args_build_func = U_instr_args_build;
    group_mask_argsfunc[18].args_build_func = R_instr_args_build;

    //普通R型
    instr_map[0].instr_mask = 0x00000033;//add
    instr_map[1].instr_mask = 0x00007033;//and
    instr_map[2].instr_mask = 0x02004033;//div
    instr_map[3].instr_mask = 0x02005033;//divu
    instr_map[4].instr_mask = 0x02000033;//mul
    instr_map[5].instr_mask = 0x02001033;//mulh
    instr_map[6].instr_mask = 0x02002033;//mulhsu
    instr_map[7].instr_mask = 0x02003033;//mulhu
    instr_map[8].instr_mask = 0x00006033;//or
    instr_map[9].instr_mask = 0x02006033;//rem
    instr_map[10].instr_mask = 0x02007033;//remu
    instr_map[11].instr_mask = 0x00001033;//sll
    instr_map[12].instr_mask = 0x00002033;//slt
    instr_map[13].instr_mask = 0x00003033;//sltu
    instr_map[14].instr_mask = 0x40005033;//sra
    instr_map[15].instr_mask = 0x00005033;//srl
    instr_map[16].instr_mask = 0x40000033;//sub
    instr_map[17].instr_mask = 0x00004033;//xor

    //普通I型
    instr_map[18].instr_mask = 0x00000013;//addi
    instr_map[19].instr_mask = 0x00007013;//andi
    instr_map[20].instr_mask = 0x00006013;//ori
    instr_map[21].instr_mask = 0x00001013;//slli
    instr_map[22].instr_mask = 0x00002013;//slti
    instr_map[23].instr_mask = 0x00003013;//sltiu
    instr_map[24].instr_mask = 0x00005013;//srli
    instr_map[25].instr_mask = 0x00004013;//xori

    //J型
    instr_map[26].instr_mask = 0x0000006F;//jal

    //B型
    instr_map[27].instr_mask = 0x00000063;//beq
    instr_map[28].instr_mask = 0x00005063;//bge
    instr_map[29].instr_mask = 0x00007063;//bgeu
    instr_map[30].instr_mask = 0x00004063;//blt
    instr_map[31].instr_mask = 0x00006063;//bltu
    instr_map[32].instr_mask = 0x00001063;//bne

    //算数型浮点F-RM 双操作数DO
    instr_map[33].instr_mask = 0x02000053;//fadd.d
    instr_map[34].instr_mask = 0x00000053;//fadd.s
    instr_map[35].instr_mask = 0xC2000053;//fcvt.w.d
    instr_map[36].instr_mask = 0xC2100053;//fcvt.wu.d
    instr_map[37].instr_mask = 0xC0000053;//fcvt.w.s
    instr_map[38].instr_mask = 0xC0100053;//fcvt.wu.s
    instr_map[39].instr_mask = 0x1A000053;//fdiv.d
    instr_map[40].instr_mask = 0x18000053;//fdiv.s
    instr_map[41].instr_mask = 0x12000053;//fmul.d
    instr_map[42].instr_mask = 0x10000053;//fmul.s
    instr_map[43].instr_mask = 0x0A000053;//fsub.d
    instr_map[44].instr_mask = 0x08000053;//fsub.s

    //算数型浮F-RM 单操作数SO
    instr_map[45].instr_mask = 0x42000053;//fcvt.d.s
    instr_map[46].instr_mask = 0xD2000053;//fcvt.d.w
    instr_map[47].instr_mask = 0xD2100053;//fcvt.d.wu
    instr_map[48].instr_mask = 0x40100053;//fcvt.s.d
    instr_map[49].instr_mask = 0xD0000053;//fcvt.s.w
    instr_map[50].instr_mask = 0xD0100053;//fcvt.s.wu
    instr_map[51].instr_mask = 0x5A000053;//fsqrt.d
    instr_map[52].instr_mask = 0x58000053;//fsqrt.s

    //功能型浮F-NRM 双操作数DO
    instr_map[53].instr_mask = 0xA2002053;//feq.d
    instr_map[54].instr_mask = 0xA0002053;//feq.s
    instr_map[55].instr_mask = 0xA2000053;//fle.d
    instr_map[56].instr_mask = 0xA0000053;//fle.s
    instr_map[57].instr_mask = 0xA2001053;//flt.d
    instr_map[58].instr_mask = 0xA0001053;//flt.s
    instr_map[59].instr_mask = 0x2A001053;//fmax.d
    instr_map[60].instr_mask = 0x28001053;//fmax.s
    instr_map[61].instr_mask = 0x2A000053;//fmin.d
    instr_map[62].instr_mask = 0x28000053;//fmin.s
    instr_map[63].instr_mask = 0x22000053;//fsgnj.d
    instr_map[64].instr_mask = 0x20000053;//fsgnj.s
    instr_map[65].instr_mask = 0x22001053;//fsgnjn.d
    instr_map[66].instr_mask = 0x20001053;//fsgnjn.s
    instr_map[67].instr_mask = 0x22002053;//fsgnjx.d
    instr_map[68].instr_mask = 0x20002053;//fsgnjx.s

    //功能型浮F-NRM 单操作数SO
    instr_map[69].instr_mask = 0xE2001053;//fclass.d
    instr_map[70].instr_mask = 0xE0001053;//fclass.s
    instr_map[71].instr_mask = 0xF0000053;//fmv.w.x
    instr_map[72].instr_mask = 0xE0000053;//fmv.x.w

    //系统状态I型
//    instr_map[73].instr_mask = 0x0;//sfence.vma
    instr_map[73].instr_mask = 0x00003073;//csrrc
    instr_map[74].instr_mask = 0x00007073;//csrrci
    instr_map[75].instr_mask = 0x00002073;//csrrs
    instr_map[76].instr_mask = 0x00006073;//csrrsi
    instr_map[77].instr_mask = 0x00001073;//csrrw
    instr_map[78].instr_mask = 0x00005073;//csrrwi

    //系统特殊型
    instr_map[79].instr_mask = 0x30200073;//mret//
    instr_map[80].instr_mask = 0x10200073;//sret//
    instr_map[81].instr_mask = 0x10500073;//wfi//
    instr_map[82].instr_mask = 0x00100073;//Ebreak//
    instr_map[83].instr_mask = 0x00000073;//ecall//

    //同步I型
    instr_map[84].instr_mask = 0x0000000F;//fence
    instr_map[85].instr_mask = 0x0000100F;//fence.i

    //浮点加载I型
    instr_map[86].instr_mask = 0x00003007;//fld
    instr_map[87].instr_mask = 0x00002007;//flw

    //跳转连接I型
    instr_map[88].instr_mask = 0x00000067;//jalr

    //加载I型
    instr_map[89].instr_mask = 0x00000003;//lb
    instr_map[90].instr_mask = 0x00004003;//lbu
    instr_map[91].instr_mask = 0x00001003;//lh
    instr_map[92].instr_mask = 0x00005003;//lhu
    instr_map[93].instr_mask = 0x00002003;//lw

    //浮点存储S型
    instr_map[94].instr_mask = 0x00003027;//fsd
    instr_map[95].instr_mask = 0x00002027;//fsw

    //存储S型
    instr_map[96].instr_mask = 0x00000023;//sb
    instr_map[97].instr_mask = 0x00001023;//sh
    instr_map[98].instr_mask = 0x00002023;//sw

    //PC-U型
    instr_map[99].instr_mask = 0x00000017;//auipc

    //加载U型
    instr_map[100].instr_mask = 0x00000037;//lui

    //AMO-R型
    instr_map[101].instr_mask = 0x0000202F;//amoadd.w
    instr_map[102].instr_mask = 0x6000202F;//amoand.w
    instr_map[103].instr_mask = 0xA000202F;//amomax.w
    instr_map[104].instr_mask = 0xE000202F;//amomaxu.w
    instr_map[105].instr_mask = 0x8000202F;//amomin.w
    instr_map[106].instr_mask = 0xC000202F;//amominu.w
    instr_map[107].instr_mask = 0x4000202F;//amoor.w
    instr_map[108].instr_mask = 0x0800202F;//amoswap.w
    instr_map[109].instr_mask = 0x2000202F;//amoxor.w
    instr_map[110].instr_mask = 0x1000202F;//lr.w
    instr_map[111].instr_mask = 0x1800202F;//sc.w


    instr_map[0].func   =   ADD         ;
    instr_map[1].func   =   AND         ;
    instr_map[2].func   =   DIV         ;
    instr_map[3].func   =   DIVU        ;
    instr_map[4].func   =   MUL         ;
    instr_map[5].func   =   MULH        ;
    instr_map[6].func   =   MULHSU      ;
    instr_map[7].func   =   MULHU       ;
    instr_map[8].func   =   OR          ;
    instr_map[9].func   =   REM         ;
    instr_map[10].func  =   REMU        ;
    instr_map[11].func  =   SLL         ;
    instr_map[12].func  =   SLT         ;
    instr_map[13].func  =   SLTU        ;
    instr_map[14].func  =   SRA         ;
    instr_map[15].func  =   SRL         ;
    instr_map[16].func  =   SUB         ;
    instr_map[17].func  =   XOR         ;
    instr_map[18].func  =   ADDI        ;
    instr_map[19].func  =   ANDI        ;
    instr_map[20].func  =   ORI         ;
    instr_map[21].func  =   SLLI        ;
    instr_map[22].func  =   SLTI        ;
    instr_map[23].func  =   SLTIU       ;
    instr_map[24].func  =   SRLI        ;
    instr_map[25].func  =   XORI        ;
    instr_map[26].func  =   JAL         ;
    instr_map[27].func  =   BEQ         ;
    instr_map[28].func  =   BGE         ;
    instr_map[29].func  =   BGEU        ;
    instr_map[30].func  =   BLT         ;
    instr_map[31].func  =   BLTU        ;
    instr_map[32].func  =   BNE         ;
    instr_map[33].func  =   FADD_D      ;
    instr_map[34].func  =   FADD_S      ;
    instr_map[35].func  =   FCVT_W_D    ;
    instr_map[36].func  =   FCVT_WU_D   ;
    instr_map[37].func  =   FCVT_W_S    ;
    instr_map[38].func  =   FCVT_WU_S   ;
    instr_map[39].func  =   FDIV_D      ;
    instr_map[40].func  =   FDIV_S      ;
    instr_map[41].func  =   FMUL_D      ;
    instr_map[42].func  =   FMUL_S      ;
    instr_map[43].func  =   FSUB_D      ;
    instr_map[44].func  =   FSUB_S      ;
    instr_map[45].func  =   FCVT_D_S    ;
    instr_map[46].func  =   FCVT_D_W    ;
    instr_map[47].func  =   FCVT_D_WU   ;
    instr_map[48].func  =   FCVT_S_D    ;
    instr_map[49].func  =   FCVT_S_W    ;
    instr_map[50].func  =   FCVT_S_WU   ;
    instr_map[51].func  =   FSQRT_D     ;
    instr_map[52].func  =   FSQRT_S     ;
    instr_map[53].func  =   FEQ_D       ;
    instr_map[54].func  =   FEQ_S       ;
    instr_map[55].func  =   FLE_D       ;
    instr_map[56].func  =   FLE_S       ;
    instr_map[57].func  =   FLT_D       ;
    instr_map[58].func  =   FLT_S       ;
    instr_map[59].func  =   FMAX_D      ;
    instr_map[60].func  =   FMAX_S      ;
    instr_map[61].func  =   FMIN_D      ;
    instr_map[62].func  =   FMIN_S      ;
    instr_map[63].func  =   FSGNJ_D     ;
    instr_map[64].func  =   FSGNJ_S     ;
    instr_map[65].func  =   FSGNJN_D    ;
    instr_map[66].func  =   FSGNJN_S    ;
    instr_map[67].func  =   FSGNJX_D    ;
    instr_map[68].func  =   FSGNJX_S    ;
    instr_map[69].func  =   FCLASS_D    ;
    instr_map[70].func  =   FCLASS_S    ;
    instr_map[71].func  =   FMV_W_X     ;
    instr_map[72].func  =   FMV_X_W     ;
    instr_map[73].func  =   CSRRC       ;
    instr_map[74].func  =   CSRRCI      ;
    instr_map[75].func  =   CSRRS       ;
    instr_map[76].func  =   CSRRSI      ;
    instr_map[77].func  =   CSRRW       ;
    instr_map[78].func  =   CSRRWI      ;
    instr_map[79].func  =   MRET        ;
    instr_map[80].func  =   SRET        ;
    instr_map[81].func  =   WFI         ;
    instr_map[82].func  =   EBREAK      ;
    instr_map[83].func  =   ECALL       ;
    instr_map[84].func  =   FENCE       ;
    instr_map[85].func  =   FENCE_I     ;
    instr_map[86].func  =   FLD         ;
    instr_map[87].func  =   FLW         ;
    instr_map[88].func  =   JALR        ;
    instr_map[89].func  =   LB          ;
    instr_map[90].func  =   LBU         ;
    instr_map[91].func  =   LH          ;
    instr_map[92].func  =   LHU         ;
    instr_map[93].func  =   LW          ;
    instr_map[94].func  =   FSD         ;
    instr_map[95].func  =   FSW         ;
    instr_map[96].func  =   SB          ;
    instr_map[97].func  =   SH          ;
    instr_map[98].func  =   SW          ;
    instr_map[99].func =   AUIPC        ;
    instr_map[100].func =   LUI         ;
    instr_map[101].func =   AMOADD_W    ;
    instr_map[102].func =   AMOAND_W    ;
    instr_map[103].func =   AMOMAX_W    ;
    instr_map[104].func =   AMOMAXU_W   ;
    instr_map[105].func =   AMOMIN_W    ;
    instr_map[106].func =   AMOMINU_W   ;
    instr_map[107].func =   AMOOR_W     ;
    instr_map[108].func =   AMOSWAP_W   ;
    instr_map[109].func =   AMOXOR_W    ;
    instr_map[110].func =   LR_W        ;
    instr_map[111].func =   SC_W        ;
}