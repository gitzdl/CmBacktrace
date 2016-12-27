/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Initialize function and other general function.
 * Created on: 2016-12-15
 */

#include <cm_backtrace.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#if defined(__CC_ARM)
    #define SECTION_START(_name_)                _name_##$$Base
    #define SECTION_END(_name_)                  _name_##$$Limit
    #define IMAGE_SECTION_START(_name_)          Image$$##_name_##$$Base
    #define IMAGE_SECTION_END(_name_)            Image$$##_name_##$$Limit
    #define CSTACK_BLOCK_START(_name_)           SECTION_START(_name_)
    #define CSTACK_BLOCK_END(_name_)             SECTION_END(_name_)
    #define CODE_SECTION_START(_name_)           IMAGE_SECTION_START(_name_)
    #define CODE_SECTION_END(_name_)             IMAGE_SECTION_END(_name_)

    extern const int CODE_SECTION_START(CMB_CODE_SECTION_NAME);
    extern const int CODE_SECTION_END(CMB_CODE_SECTION_NAME);
    extern const int CSTACK_BLOCK_START(CMB_CSTACK_BLOCK_NAME);
    extern const int CSTACK_BLOCK_END(CMB_CSTACK_BLOCK_NAME);
    //TODO 待测试
#elif defined(__ICCARM__)
    #pragma section=CMB_CSTACK_BLOCK_NAME
    #pragma section=CMB_CODE_SECTION_NAME
#elif defined(__GNUC__)
    #error "not implemented, I hope you can do this"
    //TODO 待实现
#else
  #error "not supported compiler"
#endif

enum {
    PRINT_FIRMWARE_INFO,
    PRINT_ASSERT_ON_THREAD,
    PRINT_ASSERT_ON_HANDLER,
    PRINT_THREAD_STACK_INFO,
    PRINT_MAIN_STACK_INFO,
    PRINT_CALL_STACK_INFO,
    PRINT_CALL_STACK_ERR,
    PRINT_FAULT_ON_THREAD,
    PRINT_FAULT_ON_HANDLER,
    PRINT_REGS_TITLE,
    PRINT_HFSR_VECTBL,
    PRINT_MFSR_IACCVIOL,
    PRINT_MFSR_DACCVIOL,
    PRINT_MFSR_MUNSTKERR,
    PRINT_MFSR_MSTKERR,
    PRINT_MFSR_MLSPERR,
    PRINT_BFSR_IBUSERR,
    PRINT_BFSR_PRECISERR,
    PRINT_BFSR_IMPREISERR,
    PRINT_BFSR_UNSTKERR,
    PRINT_BFSR_STKERR,
    PRINT_BFSR_LSPERR,
    PRINT_UFSR_UNDEFINSTR,
    PRINT_UFSR_INVSTATE,
    PRINT_UFSR_INVPC,
    PRINT_UFSR_NOCP,
    PRINT_UFSR_UNALIGNED,
    PRINT_UFSR_DIVBYZERO,
    PRINT_DFSR_HALTED,
    PRINT_DFSR_BKPT,
    PRINT_DFSR_DWTTRAP,
    PRINT_DFSR_VCATCH,
    PRINT_DFSR_EXTERNAL,
};

static const char *print_info[] = {
#if (CMB_PRINT_LANGUAGE == CMB_PRINT_LANGUAGE_ENGLISH)
        [PRINT_FIRMWARE_INFO]        = {"Firmware name: %s, hardware version: %s, software version: %s"},
        [PRINT_ASSERT_ON_THREAD]     = {"Assert on thread %s"},
        [PRINT_ASSERT_ON_HANDLER]    = {"Assert on interrupt or bare metal(no OS) environment"},
        [PRINT_THREAD_STACK_INFO]    = {"===== Thread stack information ====="},
        [PRINT_MAIN_STACK_INFO]      = {"====== Main stack information ======"},
        [PRINT_CALL_STACK_INFO]      = {"Show more call stack info by run: addr2line -e %s%s -a -f %.*s"},
        [PRINT_CALL_STACK_ERR]       = {"Dump call stack has an error"},
        [PRINT_FAULT_ON_THREAD]      = {"Fault on thread %s"},
        [PRINT_FAULT_ON_HANDLER]     = {"Fault on interrupt or bare metal(no OS) environment"},
        [PRINT_REGS_TITLE]           = {"=================== Registers information ===================="},
        [PRINT_HFSR_VECTBL]          = {"Hard fault is caused by failed vector fetch"},
        [PRINT_MFSR_IACCVIOL]        = {"Memory management fault is caused by instruction access violation"},
        [PRINT_MFSR_DACCVIOL]        = {"Memory management fault is caused by data access violation"},
        [PRINT_MFSR_MUNSTKERR]       = {"Memory management fault is caused by unstacking error"},
        [PRINT_MFSR_MSTKERR]         = {"Memory management fault is caused by stacking error"},
        [PRINT_MFSR_MLSPERR]         = {"Memory management fault is caused by floating-point lazy state preservation"},
        [PRINT_BFSR_IBUSERR]         = {"Bus fault is caused by instruction access violation"},
        [PRINT_BFSR_PRECISERR]       = {"Bus fault is caused by precise data access violation"},
        [PRINT_BFSR_IMPREISERR]      = {"Bus fault is caused by imprecise data access violation"},
        [PRINT_BFSR_UNSTKERR]        = {"Bus fault is caused by unstacking error"},
        [PRINT_BFSR_STKERR]          = {"Bus fault is caused by stacking error"},
        [PRINT_BFSR_LSPERR]          = {"Bus fault is caused by floating-point lazy state preservation"},
        [PRINT_UFSR_UNDEFINSTR]      = {"Usage fault is caused by attempts to execute an undefined instruction"},
        [PRINT_UFSR_INVSTATE]        = {"Usage fault is caused by attempts to switch to an invalid state (e.g., ARM)"},
        [PRINT_UFSR_INVPC]           = {"Usage fault is caused by attempts to do an exception with a bad value in the EXC_RETURN number"},
        [PRINT_UFSR_NOCP]            = {"Usage fault is caused by attempts to execute a coprocessor instruction"},
        [PRINT_UFSR_UNALIGNED]       = {"Usage fault is caused by indicates that an unaligned access fault has taken place"},
        [PRINT_UFSR_DIVBYZERO]       = {"Usage fault is caused by Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set)"},
        [PRINT_DFSR_HALTED]          = {"Debug fault is caused by halt requested in NVIC"},
        [PRINT_DFSR_BKPT]            = {"Debug fault is caused by BKPT instruction executed"},
        [PRINT_DFSR_DWTTRAP]         = {"Debug fault is caused by DWT match occurred"},
        [PRINT_DFSR_VCATCH]          = {"Debug fault is caused by Vector fetch occurred"},
        [PRINT_DFSR_EXTERNAL]        = {"Debug fault is caused by EDBGRQ signal asserted"},
#elif (CMB_PRINT_LANGUAGE == CMB_PRINT_LANUUAGE_CHINESE)
        [PRINT_FIRMWARE_INFO]        = {"固件名称：%s，硬件版本号：%s，软件版本号：%s"},
        [PRINT_ASSERT_ON_THREAD]     = {"在线程(%s)中发生断言"},
        [PRINT_ASSERT_ON_HANDLER]    = {"在中断或裸机环境下发生断言"},
        [PRINT_THREAD_STACK_INFO]    = {"=========== 线程堆栈信息 ==========="},
        [PRINT_MAIN_STACK_INFO]      = {"============ 主堆栈信息 ============"},
        [PRINT_CALL_STACK_INFO]      = {"查看更多函数调用栈信息，请运行：addr2line -e %s%s -a -f %.*s"},
        [PRINT_CALL_STACK_ERR]       = {"获取函数调用栈失败"},
        [PRINT_FAULT_ON_THREAD]      = { "在线程(%s)中发生错误异常" },
        [PRINT_FAULT_ON_HANDLER]     = {"在中断或裸机环境下发生错误异常"},
        [PRINT_REGS_TITLE]           = {"========================= 寄存器信息 ========================="},
        [PRINT_HFSR_VECTBL]          = {"发生硬错误，原因：取中断向量时出错"},
        [PRINT_MFSR_IACCVIOL]        = {"发生存储器管理错误，原因：企图从不允许访问的区域取指令"},
        [PRINT_MFSR_DACCVIOL]        = {"发生存储器管理错误，原因：企图从不允许访问的区域读、写数据"},
        [PRINT_MFSR_MUNSTKERR]       = {"发生存储器管理错误，原因：出栈时企图访问不被允许的区域"},
        [PRINT_MFSR_MSTKERR]         = {"发生存储器管理错误，原因：入栈时企图访问不被允许的区域"},
        [PRINT_MFSR_MLSPERR]         = {"发生存储器管理错误，原因：惰性保存浮点状态时发生错误"},
        [PRINT_BFSR_IBUSERR]         = {"发生总线错误，原因：指令总线错误"},
        [PRINT_BFSR_PRECISERR]       = {"发生总线错误，原因：精确的数据总线错误"},
        [PRINT_BFSR_IMPREISERR]      = {"发生总线错误，原因：不精确的数据总线错误"},
        [PRINT_BFSR_UNSTKERR]        = {"发生总线错误，原因：出栈时发生错误"},
        [PRINT_BFSR_STKERR]          = {"发生总线错误，原因：入栈时发生错误"},
        [PRINT_BFSR_LSPERR]          = {"发生总线错误，原因：惰性保存浮点状态时发生错误"},
        [PRINT_UFSR_UNDEFINSTR]      = {"发生用法错误，原因：企图执行未定义指令"},
        [PRINT_UFSR_INVSTATE]        = {"发生用法错误，原因：试图切换到 ARM 状态"},
        [PRINT_UFSR_INVPC]           = {"发生用法错误，原因：无效的异常返回码"},
        [PRINT_UFSR_NOCP]            = {"发生用法错误，原因：企图执行协处理器指令"},
        [PRINT_UFSR_UNALIGNED]       = {"发生用法错误，原因：企图执行非对齐访问"},
        [PRINT_UFSR_DIVBYZERO]       = {"发生用法错误，原因：企图执行除 0 操作"},
        [PRINT_DFSR_HALTED]          = {"发生调试错误，原因：NVIC 停机请求"},
        [PRINT_DFSR_BKPT]            = {"发生调试错误，原因：执行 BKPT 指令"},
        [PRINT_DFSR_DWTTRAP]         = {"发生调试错误，原因：数据监测点匹配"},
        [PRINT_DFSR_VCATCH]          = {"发生调试错误，原因：发生向量捕获"},
        [PRINT_DFSR_EXTERNAL]        = {"发生调试错误，原因：外部调试请求"},
#else
    #error "CMB_PRINT_LANGUAGE defined error in 'cmb_cfg.h'"
#endif
};

static char fw_name[CMB_NAME_MAX] = {0};
static char hw_ver[CMB_NAME_MAX] = {0};
static char sw_ver[CMB_NAME_MAX] = {0};
static uint32_t main_stack_start_addr = 0;
static size_t main_stack_size = 0;
static uint32_t code_start_addr = 0;
static size_t code_size = 0;
static bool init_ok = false;
static char call_stack_info[CMB_CALL_STACK_MAX_DEPTH * (8 + 1)] = { 0 };
static bool on_fault = false;
static struct cmb_hard_fault_regs regs;
static bool on_thread_before_fault = false;
static bool statck_has_fpu_regs = false;

/**
 * library initialize
 */
void cm_backtrace_init(const char *firmware_name, const char *hardware_ver, const char *software_ver) {
    strncpy(fw_name, firmware_name, CMB_NAME_MAX);
    strncpy(hw_ver, hardware_ver, CMB_NAME_MAX);
    strncpy(sw_ver, software_ver, CMB_NAME_MAX);

#if defined(__CC_ARM)
    main_stack_start_addr = (uint32_t)&CODE_SECTION_START(CMB_CODE_SECTION_NAME);
    main_stack_size = (uint32_t)&CODE_SECTION_END(CMB_CODE_SECTION_NAME) - main_stack_start_addr;
    code_start_addr = (uint32_t)&CSTACK_BLOCK_START(CMB_CSTACK_BLOCK_NAME);
    code_size = (uint32_t)&CSTACK_BLOCK_END(CMB_CSTACK_BLOCK_NAME) - code_start_addr;
    //TODO 待测试
#elif defined(__ICCARM__)
    main_stack_start_addr = (uint32_t)__section_begin(CMB_CSTACK_BLOCK_NAME);
    main_stack_size = (uint32_t)__section_end(CMB_CSTACK_BLOCK_NAME) - main_stack_start_addr;
    code_start_addr = (uint32_t)__section_begin(CMB_CODE_SECTION_NAME);
    code_size = (uint32_t)__section_end(CMB_CODE_SECTION_NAME) - code_start_addr;
#elif defined(__GNUC__)
    #error "not implemented, I hope you can do this"
    //TODO 待实现
#else
  #error "not supported compiler"
#endif

    init_ok = true;
}

/**
 * print firmware information, such as: firmware name, hardware version, software version
 */
void cm_backtrace_firmware_info(void) {
    cmb_println(print_info[PRINT_FIRMWARE_INFO], fw_name, hw_ver, sw_ver);
}

#ifdef CMB_USING_OS_PLATFORM
/**
 * Get current thread stack information
 *
 * @param sp stack current pointer
 * @param start_addr stack start address
 * @param size stack size
 */
static void get_cur_thread_stack_info(uint32_t sp, uint32_t *start_addr, size_t *size) {
    CMB_ASSERT(start_addr);
    CMB_ASSERT(size);

#if (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_RTT)
    *start_addr = (uint32_t) rt_thread_self()->stack_addr;
    *size = rt_thread_self()->stack_size;
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSII)
    extern OS_TCB *OSTCBCur;

#if OS_TASK_PROFILE_EN > 0
    *start_addr = (uint32_t) OSTCBCur->OSTCBStkBase;
    *size = (sp + OSTCBCur->OSTCBStkUsed) - *start_addr;
#else
        #error "OS_TASK_PROFILE_EN isn't enable in 'OS_CFG.H'"
#endif /* OS_TASK_PROFILE_EN > 0 */

    //TODO 测试
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSIII)
    #error "not implemented, I hope you can do this"
    //TODO 待实现
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_FREERTOS)
    #error "not implemented, I hope you can do this"
    //TODO 待实现
#endif
}

/**
 * Get current thread name
 */
static const char *get_cur_thread_name(void) {
#if (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_RTT)
    return rt_thread_self()->name;
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSII)
    extern OS_TCB *OSTCBCur;

#if OS_TASK_NAME_SIZE > 1
        return OSTCBCur->OSTCBTaskName;
#else
        return NULL;
#endif /* OS_TASK_NAME_SIZE > 1 */

    //TODO 待测试
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_UCOSIII)
    #error "not implemented, I hope you can do this"
    //TODO 待实现
#elif (CMB_OS_PLATFORM_TYPE == CMB_OS_PLATFORM_FREERTOS)
    #error "not implemented, I hope you can do this"
    //TODO 待实现
#endif
}

/**
 * dump current thread stack information
 */
static void dump_cur_thread_stack(uint32_t stack_start_addr, size_t stack_size, uint32_t *stack_pointer) {
    cmb_println(print_info[PRINT_THREAD_STACK_INFO]);
    for (; (uint32_t) stack_pointer < stack_start_addr + stack_size; stack_pointer++) {
        cmb_println("  addr: %08x    data: %08x", stack_pointer, *stack_pointer);
    }
    cmb_println("====================================");
}
#endif /* CMB_USING_OS_PLATFORM */

/**
 * dump current main stack information
 */
static void dump_main_stack(uint32_t stack_start_addr, size_t stack_size, uint32_t *stack_pointer) {
    cmb_println(print_info[PRINT_MAIN_STACK_INFO]);
    for (; (uint32_t) stack_pointer < stack_start_addr + stack_size; stack_pointer++) {
        cmb_println("  addr: %08x    data: %08x", stack_pointer, *stack_pointer);
    }
    cmb_println("====================================");
}

/**
 * backtrace function call stack
 *
 * @param buffer call stack buffer
 * @param size buffer size
 * @param sp stack pointer
 *
 * @return depth
 */
size_t cm_backtrace_call_stack(uint32_t *buffer, size_t size, uint32_t sp) {
    uint32_t stack_start_addr = main_stack_start_addr, stack_size = main_stack_size, cur_stack_pointer;
    size_t depth = 0;

    if (on_fault) {
        /* first depth is PC */
        buffer[depth++] = regs.saved.pc;
        /* program is running on thread before fault */
        if (on_thread_before_fault) {
            get_cur_thread_stack_info(sp, &stack_start_addr, &stack_size);
        }
    } else {
        cur_stack_pointer = __get_SP();

#ifdef CMB_USING_OS_PLATFORM
        /* OS environment */
        if (cur_stack_pointer == __get_PSP()) {
            get_cur_thread_stack_info(sp, &stack_start_addr, &stack_size);
        }
#endif /* CMB_USING_OS_PLATFORM */

    }

    /* copy called function address */
    for (; sp < stack_start_addr + stack_size; sp += sizeof(size_t)) {
        if ((*((uint32_t *) sp) >= code_start_addr) && (*((uint32_t *) sp) <= code_start_addr + code_size)
                && (depth < CMB_CALL_STACK_MAX_DEPTH) && (depth < size)) {
            /* this get value maybe LR, so need decrease a word to PC */
            buffer[depth++] = *((uint32_t *) sp) - sizeof(size_t);
        }
    }

    return depth;
}

/**
 * dump function call stack
 *
 * @param sp stack pointer
 */
static void print_call_stack(uint32_t sp) {
    size_t i, cur_depth = 0;
    uint32_t call_stack_buf[CMB_CALL_STACK_MAX_DEPTH] = {0};

    cur_depth = cm_backtrace_call_stack(call_stack_buf, CMB_CALL_STACK_MAX_DEPTH, sp);

    for (i = 0; i < cur_depth; i++) {
        sprintf(call_stack_info + i * (8 + 1), "%08x", call_stack_buf[i]);
        call_stack_info[i * (8 + 1) + 8] = ' ';
    }

    if (cur_depth) {
        cmb_println(print_info[PRINT_CALL_STACK_INFO], fw_name, CMB_ELF_FILE_EXTENSION_NAME, cur_depth * (8 + 1),
                call_stack_info);
    } else {
        cmb_println(print_info[PRINT_CALL_STACK_ERR]);
    }
}

/**
 * backtrace for assert
 *
 * @param sp the stack pointer when on assert occurred
 */
void cm_backtrace_assert(uint32_t sp) {
    uint32_t stack_start_addr = main_stack_start_addr, stack_size = main_stack_size, cur_stack_pointer;

    CMB_ASSERT(init_ok);

    cur_stack_pointer = __get_SP();
    cm_backtrace_firmware_info();

#ifdef CMB_USING_DUMP_STACK_INFO
#ifdef CMB_USING_OS_PLATFORM
    /* OS environment */
    if (cur_stack_pointer == __get_MSP()) {
        cmb_println(print_info[PRINT_ASSERT_ON_HANDLER]);
        dump_main_stack(stack_start_addr, stack_size, (uint32_t *) sp);
    } else if (cur_stack_pointer == __get_PSP()) {
        cmb_println(print_info[PRINT_ASSERT_ON_THREAD], get_cur_thread_name());
        get_cur_thread_stack_info(sp, &stack_start_addr, &stack_size);
        dump_cur_thread_stack(stack_start_addr, stack_size, (uint32_t *) sp);
    }
#else
    /* bare metal(no OS) environment */
    dump_main_stack();
#endif /* CMB_USING_OS_PLATFORM */
#endif /* CMB_USING_DUMP_STACK_INFO */

    print_call_stack(sp);
}

#if (CMB_CPU_PLATFORM_TYPE != CMB_CPU_ARM_CORTEX_M0)
/**
 * fault diagnosis then print cause of fault
 */
static void fault_diagnosis(void) {
    if (regs.hfsr.bits.VECTBL) {
        cmb_println(print_info[PRINT_HFSR_VECTBL]);
    }
    if (regs.hfsr.bits.FORCED) {
        /* Memory Management Fault */
        if (regs.mfsr.value) {
            if (regs.mfsr.bits.IACCVIOL) {
                cmb_println(print_info[PRINT_MFSR_IACCVIOL]);
            }
            if (regs.mfsr.bits.DACCVIOL) {
                cmb_println(print_info[PRINT_MFSR_DACCVIOL]);
            }
            if (regs.mfsr.bits.MUNSTKERR) {
                cmb_println(print_info[PRINT_MFSR_MUNSTKERR]);
            }
            if (regs.mfsr.bits.MSTKERR) {
                cmb_println(print_info[PRINT_MFSR_MSTKERR]);
            }

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7)
            if (regs.mfsr.bits.MLSPERR) {
                cmb_println(print_info[PRINT_MFSR_MLSPERR]);
            }
#endif

            if (regs.mfsr.bits.MMARVALID) {
                //TODO
            }
        }
        /* Bus Fault */
        if (regs.bfsr.value) {
            if (regs.bfsr.bits.IBUSERR) {
                cmb_println(print_info[PRINT_BFSR_IBUSERR]);
            }
            if (regs.bfsr.bits.PRECISERR) {
                cmb_println(print_info[PRINT_BFSR_PRECISERR]);
            }
            if (regs.bfsr.bits.IMPREISERR) {
                cmb_println(print_info[PRINT_BFSR_IMPREISERR]);
            }
            if (regs.bfsr.bits.UNSTKERR) {
                cmb_println(print_info[PRINT_BFSR_UNSTKERR]);
            }
            if (regs.bfsr.bits.STKERR) {
                cmb_println(print_info[PRINT_BFSR_STKERR]);
            }

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7)
            if (regs.bfsr.bits.LSPERR) {
                cmb_println(print_info[PRINT_BFSR_LSPERR]);
            }
#endif

            if (regs.bfsr.bits.BFARVALID) {
                //TODO
            }
        }
        /* Usage Fault */
        if (regs.ufsr.value) {
            if (regs.ufsr.bits.UNDEFINSTR) {
                cmb_println(print_info[PRINT_UFSR_UNDEFINSTR]);
            }
            if (regs.ufsr.bits.INVSTATE) {
                cmb_println(print_info[PRINT_UFSR_INVSTATE]);
            }
            if (regs.ufsr.bits.INVPC) {
                cmb_println(print_info[PRINT_UFSR_INVPC]);
            }
            if (regs.ufsr.bits.NOCP) {
                cmb_println(print_info[PRINT_UFSR_NOCP]);
            }
            if (regs.ufsr.bits.UNALIGNED) {
                cmb_println(print_info[PRINT_UFSR_UNALIGNED]);
            }
            if (regs.ufsr.bits.DIVBYZERO) {
                cmb_println(print_info[PRINT_UFSR_DIVBYZERO]);
            }
        }
    }
    /* Debug Fault */
    if (regs.hfsr.bits.DEBUGEVT) {
        if (regs.dfsr.value) {
            if (regs.dfsr.bits.HALTED) {
                cmb_println(print_info[PRINT_DFSR_HALTED]);
            }
            if (regs.dfsr.bits.BKPT) {
                cmb_println(print_info[PRINT_DFSR_BKPT]);
            }
            if (regs.dfsr.bits.DWTTRAP) {
                cmb_println(print_info[PRINT_DFSR_DWTTRAP]);
            }
            if (regs.dfsr.bits.VCATCH) {
                cmb_println(print_info[PRINT_DFSR_VCATCH]);
            }
            if (regs.dfsr.bits.EXTERNAL) {
                cmb_println(print_info[PRINT_DFSR_EXTERNAL]);
            }
        }
    }
}
#endif /* (CMB_CPU_PLATFORM_TYPE != CMB_CPU_ARM_CORTEX_M0) */

static uint32_t statck_del_fpu_regs(uint32_t fault_handler_lr, uint32_t sp) {
    statck_has_fpu_regs = (fault_handler_lr & (1UL << 4)) == 0 ? true : false;

    /* the stack has S0~S15 and FPSCR registers when statck_has_fpu_regs is true, double word align */
    return statck_has_fpu_regs == true ? sp + sizeof(size_t) * 18 : sp;
}

/**
 * backtrace for fault
 * @note only call once
 *
 * @param fault_handler_lr the LR register value on fault handler
 * @param fault_handler_sp the stack pointer on fault handler
 */
void cm_backtrace_fault(uint32_t fault_handler_lr, uint32_t fault_handler_sp) {
    uint32_t stack_start_addr = main_stack_start_addr, stack_size = main_stack_size, stack_pointer =
            fault_handler_sp, saved_regs_addr = stack_pointer;
    const char *regs_name[] = { "R0 ", "R1 ", "R2 ", "R3 ", "R12", "LR ", "PC ", "PSR" };

    CMB_ASSERT(init_ok);
    /* only call once */
    CMB_ASSERT(!on_fault);

    on_fault = true;
    cm_backtrace_firmware_info();

    /* delete saved  R0~R3, R12, LR, PC, xPSR registers address */
    stack_pointer += sizeof(size_t) * 8;

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7)
    stack_pointer = statck_del_fpu_regs(fault_handler_lr, stack_pointer);
#endif

#ifdef CMB_USING_DUMP_STACK_INFO
#ifdef CMB_USING_OS_PLATFORM
    on_thread_before_fault = fault_handler_lr & (1UL << 2);
    /* check which stack was used before (MSP or PSP) */
    if (on_thread_before_fault) {
        cmb_println(print_info[PRINT_FAULT_ON_THREAD], get_cur_thread_name());
        saved_regs_addr = stack_pointer = __get_PSP();
        get_cur_thread_stack_info(stack_pointer, &stack_start_addr, &stack_size);

        /* delete saved R0~R3, R12, LR,PC,xPSR registers space */
        stack_pointer += sizeof(size_t) * 8;

#if (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M4) || (CMB_CPU_PLATFORM_TYPE == CMB_CPU_ARM_CORTEX_M7)
        stack_pointer = statck_del_fpu_regs(fault_handler_lr, stack_pointer);
#endif

        dump_cur_thread_stack(stack_start_addr, stack_size, (uint32_t *) stack_pointer);

    } else {
        cmb_println(print_info[PRINT_FAULT_ON_HANDLER]);
        dump_main_stack(stack_start_addr, stack_size, (uint32_t *) stack_pointer);
    }
#else
    /* bare metal(no OS) environment */
    cmb_println(print_info[PRINT_FAULT_ON_HANDLER]);
    dump_main_stack(stack_start_addr, stack_size, (uint32_t *) stack_pointer);
#endif /* CMB_USING_OS_PLATFORM */
#endif /* CMB_USING_DUMP_STACK_INFO */

    /* dump register */
    cmb_println(print_info[PRINT_REGS_TITLE]);

    regs.saved.r0        = ((uint32_t *)saved_regs_addr)[0];  // Register R0
    regs.saved.r1        = ((uint32_t *)saved_regs_addr)[1];  // Register R1
    regs.saved.r2        = ((uint32_t *)saved_regs_addr)[2];  // Register R2
    regs.saved.r3        = ((uint32_t *)saved_regs_addr)[3];  // Register R3
    regs.saved.r12       = ((uint32_t *)saved_regs_addr)[4];  // Register R12
    regs.saved.lr        = ((uint32_t *)saved_regs_addr)[5];  // Link register LR
    regs.saved.pc        = ((uint32_t *)saved_regs_addr)[6];  // Program counter PC
    regs.saved.psr.value = ((uint32_t *)saved_regs_addr)[7];  // Program status word PSR


    cmb_println("  %s: %08x  %s: %08x  %s: %08x  %s: %08x", regs_name[0], regs.saved.r0,
                                                            regs_name[1], regs.saved.r1,
                                                            regs_name[2], regs.saved.r2,
                                                            regs_name[3], regs.saved.r3);
    cmb_println("  %s: %08x  %s: %08x  %s: %08x  %s: %08x", regs_name[4], regs.saved.r12,
                                                            regs_name[5], regs.saved.lr,
                                                            regs_name[6], regs.saved.pc,
                                                            regs_name[7], regs.saved.psr.value);
    cmb_println("==============================================================");

    /* the Cortex-M0 is not support fault diagnosis */
#if (CMB_CPU_PLATFORM_TYPE != CMB_CPU_ARM_CORTEX_M0)
    regs.syshndctrl.value = CMB_SYSHND_CTRL;  // System Handler Control and State Register
    regs.mfsr.value       = CMB_NVIC_MFSR;    // Memory Fault Status Register
    regs.bfsr.value       = CMB_NVIC_BFSR;    // Bus Fault Status Register
    regs.bfar             = CMB_NVIC_BFAR;    // Bus Fault Manage Address Register
    regs.ufsr.value       = CMB_NVIC_UFSR;    // Usage Fault Status Register
    regs.hfsr.value       = CMB_NVIC_HFSR;    // Hard Fault Status Register
    regs.dfsr.value       = CMB_NVIC_DFSR;    // Debug Fault Status Register
    regs.afsr             = CMB_NVIC_AFSR;    // Auxiliary Fault Status Register

    fault_diagnosis();
#endif

    print_call_stack(stack_pointer);
}
