#include "header/debug.h"
#include "header/avltree.h"
extern Node* bpTree;
extern bool instr_run_state;
extern uint8_t* dram;
extern uint8_t* uart0_reg;
extern uint8_t* plic_reg;
extern uint32_t* clint_reg;
extern int32_t* reg;
extern uint32_t* csr;
extern float_t* freg;

extern uint8_t* opcode_list;
extern uint8_t* group_index;
extern mask_argsfunc* group_mask_argsfunc;
extern instr_map_entry* instr_map;
extern char** instr_name_map;

void init_disasm_instr_map(){
    instr_name_map = (char**)malloc(sizeof(char*) * SUPPORT_INSTR_COUNT);

    instr_name_map[0]   =   "ADD";
    instr_name_map[1]   =   "AND";
    instr_name_map[2]   =   "DIV";
    instr_name_map[3]   =   "DIVU";
    instr_name_map[4]   =   "MUL";
    instr_name_map[5]   =   "MULH";
    instr_name_map[6]   =   "MULHSU";
    instr_name_map[7]   =   "MULHU";
    instr_name_map[8]   =   "OR";
    instr_name_map[9]   =   "REM";
    instr_name_map[10]  =   "REMU";
    instr_name_map[11]  =   "SLL";
    instr_name_map[12]  =   "SLT";
    instr_name_map[13]  =   "SLTU";
    instr_name_map[14]  =   "SRA";
    instr_name_map[15]  =   "SRL";
    instr_name_map[16]  =   "SUB";
    instr_name_map[17]  =   "XOR";
    instr_name_map[18]  =   "ADDI";
    instr_name_map[19]  =   "ANDI";
    instr_name_map[20]  =   "ORI";
    instr_name_map[21]  =   "SLLI";
    instr_name_map[22]  =   "SLTI";
    instr_name_map[23]  =   "SLTIU";
    instr_name_map[24]  =   "SRLI";
    instr_name_map[25]  =   "XORI";
    instr_name_map[26]  =   "JAL";
    instr_name_map[27]  =   "BEQ";
    instr_name_map[28]  =   "BGE";
    instr_name_map[29]  =   "BGEU";
    instr_name_map[30]  =   "BLT";
    instr_name_map[31]  =   "BLTU";
    instr_name_map[32]  =   "BNE";
    instr_name_map[33]  =   "FADD.D";
    instr_name_map[34]  =   "FADD.S";
    instr_name_map[35]  =   "FCVT.W.D";
    instr_name_map[36]  =   "FCVT.WU.D";
    instr_name_map[37]  =   "FCVT.W.S";
    instr_name_map[38]  =   "FCVT.WU.S";
    instr_name_map[39]  =   "FDIV.D";
    instr_name_map[40]  =   "FDIV.S";
    instr_name_map[41]  =   "FMUL.D";
    instr_name_map[42]  =   "FMUL.S";
    instr_name_map[43]  =   "FSUB.D";
    instr_name_map[44]  =   "FSUB.S";
    instr_name_map[45]  =   "FCVT.D.S";
    instr_name_map[46]  =   "FCVT.D.W";
    instr_name_map[47]  =   "FCVT.D.WU";
    instr_name_map[48]  =   "FCVT.S.D";
    instr_name_map[49]  =   "FCVT.S.W";
    instr_name_map[50]  =   "FCVT.S.WU";
    instr_name_map[51]  =   "FSQRT.D";
    instr_name_map[52]  =   "FSQRT.S";
    instr_name_map[53]  =   "FEQ.D";
    instr_name_map[54]  =   "FEQ.S";
    instr_name_map[55]  =   "FLE.D";
    instr_name_map[56]  =   "FLE.S";
    instr_name_map[57]  =   "FLT.D";
    instr_name_map[58]  =   "FLT.S";
    instr_name_map[59]  =   "FMAX.D";
    instr_name_map[60]  =   "FMAX.S";
    instr_name_map[61]  =   "FMIN.D";
    instr_name_map[62]  =   "FMIN.S";
    instr_name_map[63]  =   "FSGNJ.D";
    instr_name_map[64]  =   "FSGNJ.S";
    instr_name_map[65]  =   "FSGNJN.D";
    instr_name_map[66]  =   "FSGNJN.S";
    instr_name_map[67]  =   "FSGNJX.D";
    instr_name_map[68]  =   "FSGNJX.S";
    instr_name_map[69]  =   "FCLASS.D";
    instr_name_map[70]  =   "FCLASS.S";
    instr_name_map[71]  =   "FMV.W.X";
    instr_name_map[72]  =   "FMV.X.W";
    instr_name_map[73]  =   "CSRRC";
    instr_name_map[74]  =   "CSRRCI";
    instr_name_map[75]  =   "CSRRS";
    instr_name_map[76]  =   "CSRRSI";
    instr_name_map[77]  =   "CSRRW";
    instr_name_map[78]  =   "CSRRWI";
    instr_name_map[79]  =   "MRET";
    instr_name_map[80]  =   "SRET";
    instr_name_map[81]  =   "WFI";
    instr_name_map[82]  =   "EBREAK";
    instr_name_map[83]  =   "ECALL";
    instr_name_map[84]  =   "FENCE";
    instr_name_map[85]  =   "FENCE.I";
    instr_name_map[86]  =   "FLD";
    instr_name_map[87]  =   "FLW";
    instr_name_map[88]  =   "JALR";
    instr_name_map[89]  =   "LB";
    instr_name_map[90]  =   "LBU";
    instr_name_map[91]  =   "LH";
    instr_name_map[92]  =   "LHU";
    instr_name_map[93]  =   "LW";
    instr_name_map[94]  =   "FSD";
    instr_name_map[95]  =   "FSW";
    instr_name_map[96]  =   "SB";
    instr_name_map[97]  =   "SH";
    instr_name_map[98]  =   "SW";
    instr_name_map[99] =   "AUIPC";
    instr_name_map[100] =   "LUI";
    instr_name_map[101] =   "AMOADD.W";
    instr_name_map[102] =   "AMOAND.W";
    instr_name_map[103] =   "AMOMAX.W";
    instr_name_map[104] =   "AMOMAXU.W";
    instr_name_map[105] =   "AMOMIN.W";
    instr_name_map[106] =   "AMOMINU.W";
    instr_name_map[107] =   "AMOOR.W";
    instr_name_map[108] =   "AMOSWAP.W";
    instr_name_map[109] =   "AMOXOR.W";
    instr_name_map[110] =   "LR.W";
    instr_name_map[111] =   "SC.W";
}

void console(){
    while(true){
        printf("debug>>");
        char cmd_input[256];
        gets(cmd_input);
        char* op = NULL;
        char* arg = NULL;
        bool next_target = true;
        for (int i = 0; cmd_input[i] != '\0'; ++i) {
            if(cmd_input[i] == ' ') {
                cmd_input[i] = '\0';
                next_target = true;
                continue;
            }else if(next_target) {
                if (op == NULL) {
                    op = cmd_input + i;
                    next_target = false;
                } else if (arg == NULL) {
                    arg = cmd_input + i;
                    break;
                }
            }
        }
        if(*op == 'b' || strcmp(op, "breakpoint") == 0){
            char* endptr;
            uint32_t addr = strtol(arg, &endptr, 16);
            if(addr % 4 != 0){
                printf("指令地址未对齐\r\n");
                continue;
            }
            Insert(bpTree, addr);
            printf("breakpoint:%X\n", addr);
        }else if(*op == 'r'){
            if(strcmp(op, "run") == 0){
                instr_run_state = AUTO_STATE;
                break;
            }else if(strcmp(op, "reg") == 0) {
                if (arg == NULL) {
                    //输出通用寄存器
                    print_xreg();
                } else if (strcmp(arg, "csr") == 0) {
                    //输出csr
                    print_csr();
                } else if (strcmp(arg, "plic") == 0) {
                    //输出plic
                    print_plic_reg();
                } else if (strcmp(arg, "clint") == 0) {
                    //输出clint
                    print_clint_reg();
                }
            }
        }else if(strcmp(op, "mem") == 0){
            char* endptr;
            uint32_t addr = strtol(arg, &endptr, 16);
            //输出内存数据
            print_mem(addr);
        }else if(strcmp(op, "disasm") == 0){
            char* endptr;
            uint32_t addr;
            if(arg == NULL){
                addr = reg[rg_pc];
            }else{
                addr = strtol(arg, &endptr, 16);
            }
            if(addr % 4 != 0){
                printf("指令地址未对齐\r\n");
                continue;
            }
            //反汇编
            print_disasm(addr);
        }else if(strcmp(op, "step") == 0){
            break;
        }

    }
}

//same as main.c
void disasm_same_opcode_different_group(uint32_t instr, int* group_id){
    uint8_t func7 = instr >> 25;
    if(*group_id == 4){
        //浮点组
        if(func7 & 0x10){
            //功能型浮点指令，固定rm
            if(func7 & 0x20){
                //单操作数
                *group_id = 7;
            }
            *group_id = 6;
            return;
        }else{
            //算数型浮点指令，rm为参数
            if(func7 & 0x20){
                //单操作数
                *group_id = 5;
            }
            *group_id = 4;
            return;
        }
    }else if(*group_id == 8){
        if(instr & 0x7000){
            //系统状态型
            *group_id = 8;
        }else{
            //系统特殊型
            *group_id = 9;
        }
    }
}

void print_instr(uint32_t type_mask, uint32_t addr, uint32_t instr, char* name, instr_func_args* args){

    switch (type_mask) {
        case R_TYPE_MASK:
        case R_F_RM_DO_TYPE_MASK:{
            //XXX rd, rs1, rs2
            printf("%08X\t%08X\t%s\t\tx%d, x%d, x%d\n", addr, instr, name, args->rd, args->rs1, args->rs2);
            break;
        }
        case R_F_RM_SO_TYPE_MASK:
        case R_F_NRM_SO_TYPE_MASK:{
            //XXX rd, rs1
            printf("%08X\t%08X\t%s\t\tx%d, x%d\n", addr, instr, name, args->rd, args->rs1);
            break;
        }
        case I_TYPE_MASK:{
            //I+S+B
            uint8_t opcode = instr & 0x7F;
            if(opcode == 0b0100011){
                //S
                printf("%08X\t%08X\t%s\t\tx%d, %d(x%d)\n", addr, instr, name, args->rs2, args->imm16, args->rs1);
            }else if(opcode == 0b1100011){
                //B
                printf("%08X\t%08X\t%s\t\tx%d, x%d, %d\n", addr, instr, name, args->rs1, args->rs2, args->imm16);
            }else{
                //I
                printf("%08X\t%08X\t%s\t\tx%d, x%d, %d\n", addr, instr, name, args->rd, args->rs1, args->imm16);
            }
            break;
        }
        case U_TYPE_MASK:{
            //U+J
            //XXX rd, imm
            printf("%08X\t%08X\t%s\t\tx%d, 0x%X\n", addr, instr, name, args->rd, args->imm32);
            break;
        }
        case SYS_TYPE_MASK:{
            printf("%08X\t%08X\t%s\n", addr, instr, name);
        }
        default:{
            printf("未知指令:%08X\n", instr);
            break;
        }
    }
}

void disasm_instr_match(uint32_t instr, uint32_t addr){
    uint8_t opcode = instr & 0x7F;
    if(instr == 0xFFFFFFFF || instr == 0){
        printf("Illegal Instructions:%x", instr);
        return;
    }
    for (int i = 0; i < GROUP_INSTR_COUNT; ++i) {
        //匹配opcode
        if(opcode == opcode_list[i]){
            disasm_same_opcode_different_group(instr, &i);
            //i为组编号，遍历组
            for(int j = group_index[i]; j < group_index[i + 1]; j++){
                if((instr & group_mask_argsfunc[i].mask) == instr_map[j].instr_mask){
                    //find instr function
                    instr_func_args args;
                    group_mask_argsfunc[i].args_build_func(instr, &args);
                    print_instr(group_mask_argsfunc[i].mask, addr, instr, instr_name_map[j], &args);
                    return;
                }
            }
        }
    }
    //not find function
    printf("unknown instruction:%x", instr);
}


void print_disasm(uint32_t instr_addr){
    for (int i = 0; i <= 10; ++i) {
        disasm_instr_match(*((uint32_t*)(dram + instr_addr)), instr_addr);
        instr_addr += 4;
    }
}

void print_mem(uint32_t mem_addr){
    printf(" address\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\r\n");
    for (int i = 0; i <= 10; ++i) {
        // 10行
        printf("%08X\t", mem_addr + i * 16);
        for (int j = 0; j < 16; j++) {
            printf("%02X\t", dram[mem_addr + i * 16 + j]);
        }
        printf("\n");
    }
}

void print_xreg(){
    printf("zero=%X\n", reg[rg_zero]);
    printf("ra=%X\n", reg[rg_ra]);
    printf("sp=%X\n", reg[rg_sp]);
    printf("gp=%X\n", reg[rg_gp]);
    printf("tp=%X\n", reg[rg_tp]);
    printf("t0=%X\tt1=%X\tt2=%X\n", reg[rg_t0], reg[rg_t1], reg[rg_t2]);
    printf("s0=%X\n", reg[rg_s0]);
    printf("s1=%X\n", reg[rg_s1]);
    printf("a0=%X\ta1=%X\n", reg[rg_a0], reg[rg_a1]);
    printf("a2=%X\ta3=%X\ta4=%X\ta5=%X\ta6=%X\ta7=%X\n", reg[rg_a2], reg[rg_a3], reg[rg_a4], reg[rg_a5],
           reg[rg_a6], reg[rg_a7]);
    printf("s2=%X\ts3=%X\ts4=%X\ts5=%X\ts6=%X\ts7=%X\ts8=%X\ts9=%X\ts10=%X\ts11=%X\n", reg[rg_s2], reg[rg_s3],
           reg[rg_s4], reg[rg_s5], reg[rg_s6], reg[rg_s7], reg[rg_s8], reg[rg_s9], reg[rg_s10], reg[rg_s11]);
    printf("t3=%X\tt4=%X\tt5=%X\tt6=%X\n", reg[rg_t3], reg[rg_t4], reg[rg_t5], reg[rg_t6]);
    printf("pc=%X\n", reg[rg_pc]);
}


void print_csr(){
    printf("mhartid=%X\n", csr[csr_mhartid]);
    printf("mstatus=%X\n", csr[csr_mstatus]);
    printf("mie=%X\n", csr[csr_mie]);
    printf("mtvec=%X\n", csr[csr_mtvec]);
    printf("mepc=%X\n", csr[csr_mepc]);
    printf("mcause=%X\n", csr[csr_mcause]);
    printf("mtval=%X\n", csr[csr_mtval]);
    printf("mip=%X\n", csr[csr_mip]);
}

void printf_bin(uint64_t num)
{
    int i, j, k;
    unsigned char *p = (unsigned char*)&num + 7;
    for (i = 0; i < 8; i++)
    {
        j = *(p - i); //取每个字节的首地址
        for (k = 7; k >= 0; k--) //处理每个字节的8个位
        {
            if (j & (1 << k))
                printf("1");
            else
                printf("0");
        }
        printf(" ");
    }
}


void print_plic_reg(){
    printf("priority=");
    for (int i = 0; i < 54; ++i) {
        printf("%d, ", ((uint32_t*)plic_reg)[i]);
    }
    printf("\npending=");
    printf_bin(*((uint64_t*)plic_reg + 27));
    printf("\nenable=");
    printf_bin(*((uint64_t*)plic_reg + 28));
    printf("\nthreshold=%d", *((uint32_t*)plic_reg + plic_threshold));
    printf("\nclaim/complete=%d\n", *((uint32_t*)plic_reg + plic_claim_complete));
    
}

#define MTIME ((uint64_t*)(clint_reg + clint_mtime1))
#define MTIMECMP ((uint64_t*)(clint_reg + clint_mtimecmp1))
void print_clint_reg(){
    printf("MSIP=%d\n", clint_reg[clint_MSIP] & 0x1);
    printf("mtime=%llX\n", *MTIME);
    printf("mtimecmp=%llX\n", *MTIMECMP);
}
