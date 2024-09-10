#include "header/riscv.h"
#include "header/trap.h"
#include "header/debug.h"
#include "header/avltree.h"

extern uint8_t* opcode_list;
extern uint8_t* group_index;
extern mask_argsfunc* group_mask_argsfunc;
extern instr_map_entry* instr_map;
extern int running;
extern uint8_t* dram;
extern uint8_t* uart0_reg;
extern uint8_t* plic_reg;
extern uint32_t* clint_reg;
extern int32_t* reg;
extern uint32_t* csr;
extern float_t* freg;

#ifdef AMO_SUPORT
extern bits reservation_map;
extern pthread_mutex_t* amo_mutex_list;
extern bool* mutex_flag_list;
#endif

extern bool is_debug_mode;
extern bool instr_run_state;
extern Node* bpTree;

extern HANDLE hStdin;
DWORD fdwMode, fdwOldMode;
pthread_t keyboard_thread;

void keyboard_thread_run(void);

void disable_input_buffering()
{
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
    fdwMode = fdwOldMode
            ^ ENABLE_ECHO_INPUT  /* no input echo */
            ^ ENABLE_LINE_INPUT; /* return when one or
                                    more characters are available */
    SetConsoleMode(hStdin, fdwMode); /* set new mode */
    FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
    SetConsoleMode(hStdin, fdwOldMode);
}

void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

void read_image_file(FILE* file, uint32_t* origin)
{
    uint32_t max_read = DRAM_SIZE - *origin;
    uint8_t* p = dram + *origin;
    size_t read = fread(p, sizeof(uint8_t), max_read, file);
    *origin += read;
}
int read_image(const char* image_path, uint32_t* origin)
{
    FILE* file = fopen(image_path, "rb");
    if (!file) { return 0; };
    read_image_file(file, origin);
    if(*origin >= DRAM_SIZE){
        return 0;
    }
    fclose(file);
    return 1;
}

#define GET_INSTR(addr) *((uint32_t*)(dram + addr))

void same_opcode_different_group(uint32_t instr, int* group_id){
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

void (*instr_match(uint32_t instr, instr_func_args* func_args))(instr_func_args*){
    uint8_t opcode = instr & 0x7F;
    if(instr == 0xFFFFFFFF || instr == 0){
        printf("Illegal Instructions:%x", instr);
        return NULL;
    }
    for (int i = 0; i < GROUP_INSTR_COUNT; ++i) {
        //匹配opcode
        if(opcode == opcode_list[i]){
            same_opcode_different_group(instr, &i);
            //i为组编号，遍历组
            for(int j = group_index[i]; j < group_index[i + 1]; j++){
                if((instr & group_mask_argsfunc[i].mask) == instr_map[j].instr_mask){
                    //find instr function
                    group_mask_argsfunc[i].args_build_func(instr, func_args);
                    return instr_map[j].func;
                }
            }
        }
    }
    //not find function
    printf("unknown instruction:%x", instr);
    return NULL;
}

void init_csr(){
//    csr[csr_mhartid] = 0;
//    csr[csr_mstatus] = 0;
//    csr[csr_sstatus] = 0;
//    csr[csr_mie] = 0;
//    csr[csr_mtvec] = 0;
//    csr[csr_mepc] = 0;
    csr[csr_mcause] = -1;
    csr[csr_mtval] = -1;
//    csr[csr_mip] = 0;
    csr[csr_ucause] = -1;
    csr[csr_utval] = -1;
}

void init_resource(){
    dram = (uint8_t*)malloc(sizeof(uint8_t) * DRAM_SIZE);
    uart0_reg = (uint8_t*)malloc(sizeof(uint8_t) * UART0_SIZE);
    memset(uart0_reg, 0, sizeof(uint8_t) * UART0_SIZE);
    plic_reg = (uint8_t*)malloc(sizeof(uint8_t) * 4 * PLIC_REG_COUNT);
    memset(plic_reg, 0, sizeof(uint32_t) * PLIC_REG_COUNT);
    clint_reg = (uint32_t*)malloc(sizeof(uint32_t) * CLINT_REG_COUNT);
    memset(clint_reg, 0, sizeof(uint32_t) * CLINT_REG_COUNT);

    reg = (int32_t*)malloc(sizeof(int32_t) * rg_count);
    memset(reg, 0, sizeof(int32_t) * rg_count);
    csr = (uint32_t*)malloc(sizeof(uint32_t) * CSR_COUNT);
    memset(csr, 0, sizeof(uint32_t) * CSR_COUNT);
    instr_map = (instr_map_entry *) (uint32_t *) malloc(sizeof(instr_map_entry) * SUPPORT_INSTR_COUNT);
    freg = (float_t*)malloc(sizeof(float_t) * rf_count);
    memset(freg, 0, sizeof(float_t) * rf_count);
    opcode_list = (uint8_t*)malloc(sizeof(uint8_t) * GROUP_INSTR_COUNT);
    group_index = (uint8_t*)malloc(sizeof(uint8_t) * GROUP_INSTR_COUNT);
    group_mask_argsfunc = (mask_argsfunc*)malloc(sizeof(mask_argsfunc) * GROUP_INSTR_COUNT);

#ifdef AMO_SUPPORT
    reservation_map = bit_new(DRAM_SIZE / 4);
    amo_mutex_list = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * AMO_MEMORY_MUTEX_COUNT);
    for (int i = 0; i < AMO_MEMORY_MUTEX_COUNT; ++i) {
        pthread_mutex_init(&amo_mutex_list[i], NULL);
    }
    mutex_flag_list = (bool*)malloc(sizeof(bool) * AMO_MEMORY_MUTEX_COUNT);
    memset(mutex_flag_list, 0, sizeof(bool) * AMO_MEMORY_MUTEX_COUNT);
#endif
}

void free_resource(){
    free(dram);
    free(uart0_reg);
    free(plic_reg);
    free(clint_reg);

    free(reg);
    free(csr);
    free(instr_map);
    free(freg);
    free(opcode_list);
    free(group_index);
    free(group_mask_argsfunc);

#ifdef AMO_SUPPORT
    bit_destroy(reservation_map);
    for (int i = 0; i < AMO_MEMORY_MUTEX_COUNT; ++i) {
        pthread_mutex_destroy(&amo_mutex_list[i]);
    }
    free(amo_mutex_list);
    free(mutex_flag_list);
#endif
    pthread_join(keyboard_thread, NULL);

}

void parse_exe_arg(int argc, const char* argv[]){
    if (argc < 2)
    {
        /* show usage string */
        printf("rvvm <operation> [image-file1] ...\n");
        exit(2);
    }
    if(argc == 2 && strcmp(argv[1], "-h") == 0){
        printf("rvvm <operation> [image-file1] ...\n");
        printf("operation:\n");
        printf("\t-d debug mode\n");
        printf("\t-h help\n");
    }
    int i = 1;
    for (; i < argc; ++i) {
        if(argv[i][0] == '-'){
            switch (argv[i][1]) {
                case 'd':{
                    is_debug_mode = true;
                    init_disasm_instr_map();
                    break;
                }
                default:{
                    printf("unsupported operation\n");
                    exit(2);
                }
            }
        }else{
            break;
        }
    }

}


int main(int argc, const char* argv[])
{
    parse_exe_arg(argc, argv);
    init_resource();
    //image origin address
    uint32_t kernel_origin_addr = 0;
    for (int j = is_debug_mode + 1 ; j < argc; ++j)
    {
        if (!read_image(argv[j], &kernel_origin_addr))
        {
            printf("failed to load image: %s\n", argv[j]);
            free_resource();
            exit(1);
        }
    }
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();
    setbuf(stdout, 0);
    init_instr_map();
    init_csr();
    //创建键盘线程，负责接收键盘输入并发送到uart0寄存器映射的内存
    pthread_create(&keyboard_thread, NULL, keyboard_thread_run, NULL);


    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum { PC_START = 0x0 };
    reg[rg_pc] = PC_START;

    instr_func_args* iargs = (instr_func_args*)malloc(sizeof(instr_func_args));
    while (running)
    {
        memset(iargs, 0, sizeof(instr_func_args));
        if(reg[rg_pc] >= DRAM_START){
            reg[rg_pc] &= 0x7FFFFFFF;
        }
        CHECK_INSTR_FAULT(reg[rg_pc])
        uint32_t instr = GET_INSTR(reg[rg_pc]);

        if(is_debug_mode) {
            DEBUG_PROCESS_CODE()
        }

        void (*pFunc)(instr_func_args*) = instr_match(instr, iargs);
        if(pFunc != NULL){
            pFunc(iargs);
        }else{
            //非法指令异常
            SET_MCAUSE_II
        }

        //在这里进行host clint plic 的执行
        uart0_print_func();
        plic_func();
        clint_func();

        check_trap();

        reg[rg_pc] += 4;
        reg[rg_zero] = 0;
    }
    restore_input_buffering();

    free(iargs);
    free_tree(bpTree);
    free_resource();
    return 0;
}

