#include "header/trap.h"
extern uint8_t* plic_reg;
extern uint32_t* clint_reg;
extern uint32_t* csr;
extern int32_t* reg;
extern enum Privileglevel current_pl;
extern uint32_t tmp_claim_complete;
bool in_exception = false;
void check_trap(){
    //异常处理过程无法被打断
    if(in_exception){
        return;
    }
    if(csr[csr_mcause] != -1 && !(csr[csr_mcause] & 0x80000000)){
        //有异常发生
        PC2MEPC_MTVEC2PC_EXCEPTION
        TRAP_UPDATE_MSTATUS
        in_exception = true;
    }else{
        if(csr[csr_mstatus] & (1 << current_pl)){
            uint32_t t = csr[csr_mip] & csr[csr_mie];
            //当前特权级的全局中断为开
            if(t & 0xF00){
                //发生外部中断
                //设置mstatus,设置xPP,保存xIE到xPIE,清空xIE
                TRAP_UPDATE_MSTATUS
                //设置mepc和pc
                PC2MEPC_MTVEC2PC_INTERRUPT
                //设置mcause
                csr[csr_mcause] = 0x80000008 + current_pl;
                //读claim后清空，从而让plic清除pending
                uint32_t* c = (uint32_t*)plic_reg + plic_claim_complete;
                tmp_claim_complete = *c;
                *c = 0;
                //清mip
                csr[csr_mip] &= ~(0x100 << current_pl);

            }else if(t & 0xF0){
                //发生定时中断
                PC2MEPC_MTVEC2PC_INTERRUPT
                TRAP_UPDATE_MSTATUS
                csr[csr_mcause] = 0x80000004 + current_pl;
                csr[csr_mip] &= ~(0x10 << current_pl);
            }else if(t & 0xF){
                //发生软中断
                PC2MEPC_MTVEC2PC_INTERRUPT
                TRAP_UPDATE_MSTATUS
                csr[csr_mcause] = 0x80000000 + current_pl;
                csr[csr_mip] &= ~(0x1 << current_pl);
            }
        }
    }
}