/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006 by Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 */
#ifndef __ASM_LA_BARRIER_H
#define __ASM_LA_BARRIER_H

/*
 * Sync types defined by the LoongArch architecture (document MD00087 table 6.5)
 * These values are used with the sync instruction to perform memory barriers.
 * Types of ordering guarantees available through the SYNC instruction:
 * - Completion Barriers
 * - Ordering Barriers
 * As compared to the completion barrier, the ordering barrier is a
 * lighter-weight operation as it does not require the specified instructions
 * before the SYNC to be already completed. Instead it only requires that those
 * specified instructions which are subsequent to the SYNC in the instruction
 * stream are never re-ordered for processing ahead of the specified
 * instructions which are before the SYNC in the instruction stream.
 * This potentially reduces how many cycles the barrier instruction must stall
 * before it completes.
 * Implementations that do not use any of the non-zero values of stype to define
 * different barriers, such as ordering barriers, must make those stype values
 * act the same as stype zero.
 */

/*
 * Completion barriers:
 * - Every synchronizable specified memory instruction (loads or stores or both)
 *   that occurs in the instruction stream before the SYNC instruction must be
 *   already globally performed before any synchronizable specified memory
 *   instructions that occur after the SYNC are allowed to be performed, with
 *   respect to any other processor or coherent I/O module.
 *
 * - The barrier does not guarantee the order in which instruction fetches are
 *   performed.
 *
 * - A stype value of zero will always be defined such that it performs the most
 *   complete set of synchronization operations that are defined.This means
 *   stype zero always does a completion barrier that affects both loads and
 *   stores preceding the SYNC instruction and both loads and stores that are
 *   subsequent to the SYNC instruction. Non-zero values of stype may be defined
 *   by the architecture or specific implementations to perform synchronization
 *   behaviors that are less complete than that of stype zero. If an
 *   implementation does not use one of these non-zero values to define a
 *   different synchronization behavior, then that non-zero value of stype must
 *   act the same as stype zero completion barrier. This allows software written
 *   for an implementation with a lighter-weight barrier to work on another
 *   implementation which only implements the stype zero completion barrier.
 *
 * - A completion barrier is required, potentially in conjunction with SSNOP (in
 *   Release 1 of the Architecture) or EHB (in Release 2 of the Architecture),
 *   to guarantee that memory reference results are visible across operating
 *   mode changes. For example, a completion barrier is required on some
 *   implementations on entry to and exit from Debug Mode to guarantee that
 *   memory effects are handled correctly.
 */

/*
 * stype 0 - A completion barrier that affects preceding loads and stores and
 * subsequent loads and stores.
 * Older instructions which must reach the load/store ordering point before the
 * SYNC instruction completes: Loads, Stores
 * Younger instructions which must reach the load/store ordering point only
 * after the SYNC instruction completes: Loads, Stores
 * Older instructions which must be globally performed when the SYNC instruction
 * completes: Loads, Stores
 */
#define STYPE_SYNC 0x0

/*
 * Ordering barriers:
 * - Every synchronizable specified memory instruction (loads or stores or both)
 *   that occurs in the instruction stream before the SYNC instruction must
 *   reach a stage in the load/store datapath after which no instruction
 *   re-ordering is possible before any synchronizable specified memory
 *   instruction which occurs after the SYNC instruction in the instruction
 *   stream reaches the same stage in the load/store datapath.
 *
 * - If any memory instruction before the SYNC instruction in program order,
 *   generates a memory request to the external memory and any memory
 *   instruction after the SYNC instruction in program order also generates a
 *   memory request to external memory, the memory request belonging to the
 *   older instruction must be globally performed before the time the memory
 *   request belonging to the younger instruction is globally performed.
 *
 * - The barrier does not guarantee the order in which instruction fetches are
 *   performed.
 */

/*
 * stype 0x10 - An ordering barrier that affects preceding loads and stores and
 * subsequent loads and stores.
 * Older instructions which must reach the load/store ordering point before the
 * SYNC instruction completes: Loads, Stores
 * Younger instructions which must reach the load/store ordering point only
 * after the SYNC instruction completes: Loads, Stores
 * Older instructions which must be globally performed when the SYNC instruction
 * completes: N/A
 */
#define STYPE_SYNC_MB 0x10


#define __sync()				\
	__asm__ __volatile__(			\
		"dbar 0\n\t"			\
		: /* no output */		\
		: /* no input */		\
		: "memory")

#define fast_wmb()	__sync()
#define fast_rmb()	__sync()
#define fast_mb()	__sync()

#define fast_iob()	__sync()

#define wmb()		fast_wmb()
#define rmb()		fast_rmb()
#define mb()		fast_mb()
#define iob()		fast_iob()
#define __iormb()	fast_rmb()
#define __iowmb()	fast_wmb()

/**
 * array_index_mask_nospec() - generate a ~0 mask when index < size, 0 otherwise
 * @index: array element index
 * @size: number of elements in array
 *
 * Returns:
 *     0 - (@index < @size)
 */
#define array_index_mask_nospec array_index_mask_nospec
static inline unsigned long array_index_mask_nospec(unsigned long index,
						    unsigned long size)
{
	unsigned long mask;

	__asm__ __volatile__(
		"sltu	%0, %1, %2\n\t"
#if (_LOONGARCH_SZLONG == 32)
		"sub.w	%0, $r0, %0\n\t"
#elif (_LOONGARCH_SZLONG == 64)
		"sub.d	%0, $r0, %0\n\t"
#endif
		: "=r" (mask)
		: "r" (index), "r" (size)
		:);

	return mask;
}

#if defined(CONFIG_WEAK_ORDERING)
#define __smp_mb()	__asm__ __volatile__("dbar 0" : : :"memory")
#define __smp_rmb()	__asm__ __volatile__("dbar 0" : : :"memory")
#define __smp_wmb()	__asm__ __volatile__("dbar 0" : : :"memory")
#else
#define __smp_mb()	barrier()
#define __smp_rmb()	barrier()
#define __smp_wmb()	barrier()
#endif

#if defined(CONFIG_WEAK_REORDERING_BEYOND_LLSC) && defined(CONFIG_SMP)
#define __WEAK_LLSC_MB		"	dbar 0  \n"
#else
#define __WEAK_LLSC_MB		"		\n"
#endif

#define __smp_mb__before_atomic()	barrier()
#define __smp_mb__after_atomic()	barrier()

#define __smp_store_release(p, v)						\
do {										\
	union { typeof(*p) __val; char __c[1]; } __u =				\
		{ .__val = (__force typeof(*p)) (v) }; 				\
	unsigned long __tmp;							\
	compiletime_assert_atomic_type(*p);					\
	switch (sizeof(*p)) {							\
	case 1:									\
		__smp_mb();							\
		*(volatile __u8 *)p = *(__u8 *)__u.__c;				\
		break;								\
	case 2:									\
		__smp_mb();							\
		*(volatile __u16 *)p = *(__u16 *)__u.__c;			\
		break;								\
	case 4:									\
		__asm__ __volatile__(						\
		"amswap_db.w %[tmp], %[val], %[mem]	\n"			\
		: [mem] "+ZB" (*(u32 *)p), [tmp] "=&r" (__tmp)			\
		: [val] "r" (*(__u32 *)__u.__c)					\
		: );								\
		break;								\
	case 8:									\
		__asm__ __volatile__(						\
		"amswap_db.d %[tmp], %[val], %[mem]	\n"			\
		: [mem] "+ZB" (*(u64 *)p), [tmp] "=&r" (__tmp)			\
		: [val] "r" (*(__u64 *)__u.__c)					\
		: );								\
		break;								\
	default:								\
		__smp_mb();							\
		__builtin_memcpy((void *)p, (const void *)__u.__c, sizeof(*p));	\
		barrier();							\
	}									\
} while (0)

#define __smp_load_acquire(p)							\
({										\
	union { typeof(*p) __val; char __c[1]; } __u;				\
	unsigned long __tmp = 0;							\
	compiletime_assert_atomic_type(*p);					\
	switch (sizeof(*p)) {							\
	case 1:									\
		*(__u8 *)__u.__c = *(volatile __u8 *)p;				\
		__smp_mb();							\
		break;								\
	case 2:									\
		*(__u16 *)__u.__c = *(volatile __u16 *)p;			\
		__smp_mb();							\
		break;								\
	case 4:									\
		__asm__ __volatile__(						\
		"amor_db.w %[val], %[tmp], %[mem]	\n"				\
		: [val] "=&r" (*(__u32 *)__u.__c)				\
		: [mem] "ZB" (*(u32 *) p), [tmp] "r" (__tmp)			\
		: "memory");							\
		break;								\
	case 8:									\
		__asm__ __volatile__(						\
		"amor_db.d %[val], %[tmp], %[mem]	\n"				\
		: [val] "=&r" (*(__u64 *)__u.__c)				\
		: [mem] "ZB" (*(u64 *) p), [tmp] "r" (__tmp)			\
		: "memory");							\
		break;								\
	default:								\
		barrier();							\
		__builtin_memcpy((void *)__u.__c, (const void *)p, sizeof(*p));	\
		__smp_mb();							\
	}									\
	__u.__val;								\
})

#define __smp_store_mb(p, v)							\
do {										\
	union { typeof(p) __val; char __c[1]; } __u =				\
		{ .__val = (__force typeof(p)) (v) }; 				\
	unsigned long __tmp;							\
	switch (sizeof(p)) {							\
	case 1:									\
		*(volatile __u8 *)&p = *(__u8 *)__u.__c;			\
		__smp_mb();							\
		break;								\
	case 2:									\
		*(volatile __u16 *)&p = *(__u16 *)__u.__c;			\
		__smp_mb();							\
		break;								\
	case 4:									\
		__asm__ __volatile__(						\
		"amswap_db.w %[tmp], %[val], %[mem]	\n"			\
		: [mem] "+ZB" (*(u32 *)&p), [tmp] "=&r" (__tmp)			\
		: [val] "r" (*(__u32 *)__u.__c) 				\
		: );								\
		break;								\
	case 8:									\
		__asm__ __volatile__(						\
		"amswap_db.d %[tmp], %[val], %[mem]	\n"			\
		: [mem] "+ZB" (*(u64 *)&p), [tmp] "=&r" (__tmp)			\
		: [val] "r" (*(__u64 *)__u.__c)					\
		: );								\
		break;								\
	default:								\
		__builtin_memcpy((void *)&p, (const void *)__u.__c, sizeof(p));	\
		__smp_mb();							\
	}									\
} while (0)

#endif /* __ASM_LA_BARRIER_H */
