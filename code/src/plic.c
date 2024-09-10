//
// Created by Administrator on 2024/9/4.
//
#include "header/plic.h"
extern uint8_t* plic_reg;
extern uint32_t* csr;
bool wait_read_claim = false;

#define pending ((uint64_t*)((uint32_t*)plic_reg + plic_pending1))
#define enable ((uint64_t*)((uint32_t*)plic_reg + plic_enable1))
#define threshold ((uint32_t*)plic_reg + plic_threshold)
#define claim_complete ((uint32_t*)plic_reg + plic_claim_complete)


void plic_func(){
    uint64_t inter_enable_pending;
    static uint64_t old_inter_pending = 0;
    static uint32_t will_process_irq = 0;
    static uint32_t max_priority = 0;

    if(!wait_read_claim){
        inter_enable_pending = *pending & *enable;
        if (inter_enable_pending ^ old_inter_pending){
            //有不同，可能是hart处理了一个中断，或有中断源新申请了中断
            if(inter_enable_pending){
                old_inter_pending = inter_enable_pending;
                for (int irq = 0; irq < 54; ++irq) {
                    if(inter_enable_pending == 0)
                        break;
                    if (inter_enable_pending % 2){
                        //当前位对应中断源有待处理的中断
                        uint32_t pri = *((uint32_t*)plic_reg + irq);
                        if(*threshold < pri && max_priority < pri){
                            will_process_irq = irq;
                            max_priority = pri;
                        }
                    }
                    inter_enable_pending >>= 1;
                }
                *claim_complete = will_process_irq;
                wait_read_claim = true;
                //mip.MEIP
                csr[csr_mip] |= 0x800;
            }
        }
    }else{
        //hart读完claim需要置0
        if(*claim_complete == 0){
            CLEAN_PENDING(will_process_irq)
        }
    }

}