/*
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the <ORGANIZATION> nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MACHINE_SYSTEM_H
#define _MACHINE_SYSTEM_H

/* RPMSG MU channel index */
#define RPMSG_MU_CHANNEL            (1)

/*
 * Linux requires the ALIGN to 0x1000(4KB) instead of 0x80
 */
#ifndef VRING_ALIGN
#define VRING_ALIGN                       0x1000
#endif

/*
 * Linux has a different alignment requirement, and its have 512 buffers instead of 32 buffers for the 2 ring
 */
#ifndef VRING0_BASE
#define VRING0_BASE                       0xBFFF0000
#endif

#ifndef VRING1_BASE
#define VRING1_BASE                       0xBFFF8000
#endif

/* IPI_VECT here defines VRING index in MU */
#define VRING0_IPI_VECT                   0
#define VRING1_IPI_VECT                   1

#define MASTER_CPU_ID                     0
#define REMOTE_CPU_ID                     1

/*
 * 32 MSG (16 rx, 16 tx), 512 bytes each, it is only used when RPMSG driver is working in master mode, otherwise
 * the share memory is managed by the other side.
 * When working with Linux, SHM_ADDR and SHM_SIZE is not used
 */
#define SHM_ADDR                    (0)
#define SHM_SIZE                    (0)


/* Memory barrier */
#if (defined(__CC_ARM))
#define MEM_BARRIER() __schedule_barrier()
#elif (defined(__GNUC__))
#define MEM_BARRIER() asm volatile("dsb" : : : "memory")
#else
#define MEM_BARRIER()
#endif

static inline unsigned int xchg(void* plock, unsigned int lockVal)
{
	volatile unsigned int tmpVal = 0;
	volatile unsigned int tmpVal1 = 0;

#if defined(__GNUC__) && !defined(__CC_ARM)

	asm (
		"1:                                \n\t"
		"LDREX  %[tmpVal], [%[plock]]      \n\t"
		"STREX  %[tmpVal1], %[lockVal], [%[plock]] \n\t"
		"CMP    %[tmpVal1], #0                     \n\t"
		"BNE    1b                         \n\t"
		"DMB                               \n\t"
		: [tmpVal] "=&r"(tmpVal)
		: [tmpVal1] "r" (tmpVal1), [lockVal] "r"(lockVal), [plock] "r"(plock)
		: "cc", "memory"
	);

#endif

	return tmpVal;
}

void platform_time_delay(int num_msec);
int  platform_in_isr(void);
int  platform_interrupt_enable(unsigned int vector_id, unsigned int trigger_type, unsigned int priority);
int  platform_interrupt_disable(unsigned int vector_id);
void platform_interrupt_enable_all(void);
void platform_interrupt_disable_all(void);
void platform_map_mem_region(unsigned int va,unsigned int pa, unsigned int size, unsigned int flags);
void platform_cache_all_flush_invalidate(void);
void platform_cache_disable(void);
unsigned long platform_vatopa(void *addr);
void *platform_patova(unsigned long addr);
void platform_isr(int vect_id, void *data);
int  platform_init(void);
int  platform_deinit(void);
void rpmsg_handler(void);

#endif /* _MACHINE_SYSTEM_H */