//
// Created by Administrator on 2024/9/3.
//

#ifndef MYVM_PLIC_H
#define MYVM_PLIC_H
#include "riscv.h"

#define SET_PENDING(id) {  \
                        int t = id / 32; \
                        uint32_t* p = (uint32_t*)plic_reg + (plic_pending1 + t);         \
                        *p |= (1 << (id - 32 * t)); \
                        }

#define CLEAN_PENDING(id) { \
                            int t = id / 32; \
                            uint32_t* p = (uint32_t*)plic_reg + (plic_pending1 + t);         \
                            *p &= ~(1 << (id - 32 * t)); \
                            }

#endif //MYVM_PLIC_H
