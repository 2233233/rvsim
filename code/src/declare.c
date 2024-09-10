#include "header/riscv.h"
#include "header/avltree.h"

//根据opcode的不同分组，列出各种不同opcode
uint8_t* opcode_list;
//每组在instr_map中的开头索引
uint8_t* group_index;
//每组指令对应的操作数构建函数和mask
mask_argsfunc* group_mask_argsfunc;
//包含所有指令对应的标识（opcode、funct357）和处理函数
instr_map_entry* instr_map;
int running = 1;
//memory
uint8_t* dram;
uint8_t* uart0_reg;
uint8_t* plic_reg;
uint32_t* clint_reg;

int32_t* reg;
uint32_t* csr;
float_t* freg;

#ifdef AMO_SUPPORT
//以字为单位进行映射的bitmap，对应memory中一个字的保留位
bits reservation_map;
//以128字节为单位，对memory进行划区的锁列表，主要控制amo指令间的互斥
pthread_mutex_t* amo_mutex_list;
//同amo_mutex_list，但是主要负责控制普通写内存指令不会干扰amo指令
bool* mutex_flag_list;
#endif

bool is_debug_mode = false;
//false=STEP true=RUN
bool instr_run_state = false;
//avl树，用于管理断点
Node* bpTree = NULL;
//当前特权级
enum Privileglevel current_pl = M_MODE;
//保留中断的claim
uint32_t tmp_claim_complete = 0;
//debug disasm用
char** instr_name_map;

HANDLE hStdin = INVALID_HANDLE_VALUE;