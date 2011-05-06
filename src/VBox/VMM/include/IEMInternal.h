/* $Id$ */
/** @file
 * IEM - Internal header file.
 */

/*
 * Copyright (C) 2011 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef ___IEMInternal_h
#define ___IEMInternal_h

#include <VBox/vmm/stam.h>
#include <VBox/vmm/cpum.h>
#include <VBox/param.h>


RT_C_DECLS_BEGIN


/** @defgroup grp_iem_int       Internals
 * @ingroup grp_iem
 * @internal
 * @{
 */


/**
 * Operand or addressing mode.
 */
typedef enum IEMMODE
{
    IEMMODE_16BIT = 0,
    IEMMODE_32BIT,
    IEMMODE_64BIT
} IEMMODE;
AssertCompileSize(IEMMODE, 4);

/**
 * Extended operand mode that includes a representation of 8-bit.
 *
 * This is used for packing down modes when invoking some C instruction
 * implementations.
 */
typedef enum IEMMODEX
{
    IEMMODEX_16BIT = IEMMODE_16BIT,
    IEMMODEX_32BIT = IEMMODE_32BIT,
    IEMMODEX_64BIT = IEMMODE_64BIT,
    IEMMODEX_8BIT
} IEMMODEX;
AssertCompileSize(IEMMODEX, 4);


#ifdef IEM_VERIFICATION_MODE

/**
 * Verification event type.
 */
typedef enum IEMVERIFYEVENT
{
    IEMVERIFYEVENT_INVALID = 0,
    IEMVERIFYEVENT_IOPORT_READ,
    IEMVERIFYEVENT_IOPORT_WRITE,
    IEMVERIFYEVENT_RAM_WRITE,
    IEMVERIFYEVENT_RAM_READ
} IEMVERIFYEVENT;

/** Checks if the event type is a RAM read or write. */
# define IEMVERIFYEVENT_IS_RAM(a_enmType)    ((a_enmType) == IEMVERIFYEVENT_RAM_WRITE || (a_enmType) == IEMVERIFYEVENT_RAM_READ)

/**
 * Verification event record.
 */
typedef struct IEMVERIFYEVTREC
{
    /** Pointer to the next record in the list. */
    struct IEMVERIFYEVTREC *pNext;
    /** The event type. */
    IEMVERIFYEVENT          enmEvent;
    /** The event data. */
    union
    {
        /** IEMVERIFYEVENT_IOPORT_READ */
        struct
        {
            RTIOPORT    Port;
            uint32_t    cbValue;
        } IOPortRead;

        /** IEMVERIFYEVENT_IOPORT_WRITE */
        struct
        {
            RTIOPORT    Port;
            uint32_t    cbValue;
            uint32_t    u32Value;
        } IOPortWrite;

        /** IEMVERIFYEVENT_RAM_READ */
        struct
        {
            RTGCPHYS    GCPhys;
            uint32_t    cb;
        } RamRead;

        /** IEMVERIFYEVENT_RAM_WRITE */
        struct
        {
            RTGCPHYS    GCPhys;
            uint32_t    cb;
            uint8_t     ab[32];
        } RamWrite;
    } u;
} IEMVERIFYEVTREC;
/** Pointer to an IEM event verification records. */
typedef IEMVERIFYEVTREC *PIEMVERIFYEVTREC;

#endif /* IEM_VERIFICATION_MODE */


/**
 * The per-CPU IEM state.
 */
typedef struct IEMCPU
{
    /** Pointer to the CPU context - ring-3 contex. */
    R3PTRTYPE(PCPUMCTX)     pCtxR3;
    /** Pointer to the CPU context - ring-0 contex. */
    R0PTRTYPE(PCPUMCTX)     pCtxR0;
    /** Pointer to the CPU context - raw-mode contex. */
    RCPTRTYPE(PCPUMCTX)     pCtxRC;

    /** Offset of the VMCPU structure relative to this structure (negative). */
    int32_t                 offVMCpu;
    /** Offset of the VM structure relative to this structure (negative). */
    int32_t                 offVM;

    /** Whether to bypass access handlers or not. */
    bool                    fByPassHandlers;
    /** Explicit alignment padding. */
    bool                    afAlignment0[6];

    /** The CPL. */
    uint8_t                 uCpl;
    /** The current CPU execution mode (CS). */
    IEMMODE                 enmCpuMode;

    /** @name Statistics
     * @{  */
    /** The number of instructions we've executed. */
    uint32_t                cInstructions;
    /** The number of potential exits. */
    uint32_t                cPotentialExits;
#ifdef IEM_VERIFICATION_MODE
    /** The Number of I/O port reads that has been performed. */
    uint32_t                cIOReads;
    /** The Number of I/O port writes that has been performed. */
    uint32_t                cIOWrites;
    /** Set if no comparison to REM is currently performed.
     * This is used to skip past really slow bits.  */
    bool                    fNoRem;
    bool                    afAlignment1[3];
    /** Mask of undefined eflags.
     * The verifier will any difference in these flags. */
    uint32_t                fUndefinedEFlags;
    /** The physical address corresponding to abOpcodes[0]. */
    RTGCPHYS                GCPhysOpcodes;
#endif
    /** @}  */

    /** @name Decoder state.
     * @{ */

    /** The default addressing mode . */
    IEMMODE                 enmDefAddrMode;
    /** The effective addressing mode . */
    IEMMODE                 enmEffAddrMode;
    /** The default operand mode . */
    IEMMODE                 enmDefOpSize;
    /** The effective operand mode . */
    IEMMODE                 enmEffOpSize;

    /** The prefix mask (IEM_OP_PRF_XXX). */
    uint32_t                fPrefixes;
    /** The extra REX ModR/M register field bit (REX.R << 3). */
    uint8_t                 uRexReg;
    /** The extra REX ModR/M r/m field, SIB base and opcode reg bit
     * (REX.B << 3). */
    uint8_t                 uRexB;
    /** The extra REX SIB index field bit (REX.X << 3). */
    uint8_t                 uRexIndex;
    /** The effective segment register (X86_SREG_XXX). */
    uint8_t                 iEffSeg;

    /** The current offset into abOpcodes. */
    uint8_t                 offOpcode;
    /** The size of what has currently been fetched into abOpcodes. */
    uint8_t                 cbOpcode;
    /** The opcode bytes. */
    uint8_t                 abOpcode[15];

    /** @}*/

    /** Alignment padding for aMemMappings. */
    uint8_t                 abAlignment2[5];

    /** The number of active guest memory mappings. */
    uint8_t                 cActiveMappings;
    /** The next unused mapping index. */
    uint8_t                 iNextMapping;
    /** Records for tracking guest memory mappings. */
    struct
    {
        /** The address of the mapped bytes. */
        void               *pv;
#if defined(IN_RC) && HC_ARCH_BITS == 64
        uint32_t            u32Alignment3; /**< Alignment padding. */
#endif
        /** The access flags (IEM_ACCESS_XXX).
         * IEM_ACCESS_INVALID if the entry is unused. */
        uint32_t            fAccess;
#if HC_ARCH_BITS == 64
        uint32_t            u32Alignment4; /**< Alignment padding. */
#endif
    } aMemMappings[3];

    /** Bounce buffer info.
     * This runs in parallel to aMemMappings. */
    struct
    {
        /** The physical address of the first byte. */
        RTGCPHYS            GCPhysFirst;
        /** The physical address of the second page. */
        RTGCPHYS            GCPhysSecond;
        /** The number of bytes in the first page. */
        uint16_t            cbFirst;
        /** The number of bytes in the second page. */
        uint16_t            cbSecond;
        /** Whether it's unassigned memory. */
        bool                fUnassigned;
        /** Explicit alignment padding. */
        bool                afAlignment5[3];
    } aMemBbMappings[3];

    /** Bounce buffer storage.
     * This runs in parallel to aMemMappings and aMemBbMappings. */
    struct
    {
        uint8_t             ab[64];
    } aBounceBuffers[3];

#ifdef IEM_VERIFICATION_MODE
    /** The event verification records for what IEM did (LIFO). */
    R3PTRTYPE(PIEMVERIFYEVTREC)     pIemEvtRecHead;
    /** Insertion point for pIemEvtRecHead. */
    R3PTRTYPE(PIEMVERIFYEVTREC *)   ppIemEvtRecNext;
    /** The event verification records for what the other party did (FIFO). */
    R3PTRTYPE(PIEMVERIFYEVTREC)     pOtherEvtRecHead;
    /** Insertion point for pOtherEvtRecHead. */
    R3PTRTYPE(PIEMVERIFYEVTREC *)   ppOtherEvtRecNext;
    /** List of free event records. */
    R3PTRTYPE(PIEMVERIFYEVTREC)     pFreeEvtRec;
#endif
} IEMCPU;
/** Pointer to the per-CPU IEM state. */
typedef IEMCPU *PIEMCPU;

/** Converts a IEMCPU pointer to a VMCPU pointer.
 * @returns VMCPU pointer.
 * @param   a_pIemCpu       The IEM per CPU instance data.
 */
#define IEMCPU_TO_VMCPU(a_pIemCpu)  ((PVMCPU)( (uintptr_t)(a_pIemCpu) + a_pIemCpu->offVMCpu ))

/** Converts a IEMCPU pointer to a VM pointer.
 * @returns VM pointer.
 * @param   a_pIemCpu       The IEM per CPU instance data.
 */
#define IEMCPU_TO_VM(a_pIemCpu)     ((PVM)( (uintptr_t)(a_pIemCpu) + a_pIemCpu->offVM ))

/** @name IEM_ACCESS_XXX - Access details.
 * @{ */
#define IEM_ACCESS_INVALID              UINT32_C(0x000000ff)
#define IEM_ACCESS_TYPE_READ            UINT32_C(0x00000001)
#define IEM_ACCESS_TYPE_WRITE           UINT32_C(0x00000002)
#define IEM_ACCESS_TYPE_EXEC            UINT32_C(0x00000004)
#define IEM_ACCESS_TYPE_MASK            UINT32_C(0x00000007)
#define IEM_ACCESS_WHAT_CODE            UINT32_C(0x00000010)
#define IEM_ACCESS_WHAT_DATA            UINT32_C(0x00000020)
#define IEM_ACCESS_WHAT_STACK           UINT32_C(0x00000030)
#define IEM_ACCESS_WHAT_SYS             UINT32_C(0x00000040)
#define IEM_ACCESS_WHAT_MASK            UINT32_C(0x00000070)
/** Used in aMemMappings to indicate that the entry is bounce buffered. */
#define IEM_ACCESS_BOUNCE_BUFFERED      UINT32_C(0x00000100)
/** Read+write data alias. */
#define IEM_ACCESS_DATA_RW              (IEM_ACCESS_TYPE_READ  | IEM_ACCESS_TYPE_WRITE | IEM_ACCESS_WHAT_DATA)
/** Write data alias. */
#define IEM_ACCESS_DATA_W               (IEM_ACCESS_TYPE_WRITE | IEM_ACCESS_WHAT_DATA)
/** Read data alias. */
#define IEM_ACCESS_DATA_R               (IEM_ACCESS_TYPE_READ  | IEM_ACCESS_WHAT_DATA)
/** Instruction fetch alias. */
#define IEM_ACCESS_INSTRUCTION          (IEM_ACCESS_TYPE_EXEC  | IEM_ACCESS_WHAT_CODE)
/** Stack write alias. */
#define IEM_ACCESS_STACK_W              (IEM_ACCESS_TYPE_WRITE | IEM_ACCESS_WHAT_STACK)
/** Stack read alias. */
#define IEM_ACCESS_STACK_R              (IEM_ACCESS_TYPE_READ  | IEM_ACCESS_WHAT_STACK)
/** @} */

/** @name Prefix constants (IEMCPU::fPrefixes)
 * @{ */
#define IEM_OP_PRF_SEG_CS               RT_BIT_32(0)
#define IEM_OP_PRF_SEG_SS               RT_BIT_32(1)
#define IEM_OP_PRF_SEG_DS               RT_BIT_32(2)
#define IEM_OP_PRF_SEG_ES               RT_BIT_32(3)
#define IEM_OP_PRF_SEG_FS               RT_BIT_32(4)
#define IEM_OP_PRF_SEG_GS               RT_BIT_32(5)
#define IEM_OP_PRF_SEG_MASK             UINT32_C(0x3f)

#define IEM_OP_PRF_SIZE_OP              RT_BIT_32(8)
#define IEM_OP_PRF_SIZE_REX_W           RT_BIT_32(9)
#define IEM_OP_PRF_SIZE_ADDR            RT_BIT_32(10)

#define IEM_OP_PRF_LOCK                 RT_BIT_32(16)
#define IEM_OP_PRF_REPNZ                RT_BIT_32(17)
#define IEM_OP_PRF_REPZ                 RT_BIT_32(18)

#define IEM_OP_PRF_REX                  RT_BIT_32(24)
#define IEM_OP_PRF_REX_R                RT_BIT_32(25)
#define IEM_OP_PRF_REX_B                RT_BIT_32(26)
#define IEM_OP_PRF_REX_X                RT_BIT_32(27)
/** @} */

/**
 * Tests if verification mode is enabled.
 *
 * This expands to @c false when IEM_VERIFICATION_MODE is not defined and
 * should therefore cause the compiler to eliminate the verification branch
 * of an if statement.  */
#ifdef IEM_VERIFICATION_MODE
# define IEM_VERIFICATION_ENABLED(a_pIemCpu)    (!(a_pIemCpu)->fNoRem)
#else
# define IEM_VERIFICATION_ENABLED(a_pIemCpu)    (false)
#endif

/**
 * Indicates to the verifier that the given flag set is undefined.
 *
 * Can be invoked again to add more flags.
 *
 * This is a NOOP if the verifier isn't compiled in.
 */
#ifdef IEM_VERIFICATION_MODE
# define IEMOP_VERIFICATION_UNDEFINED_EFLAGS(a_fEfl) do { pIemCpu->fUndefinedEFlags |= (a_fEfl); } while (0)
#else
# define IEMOP_VERIFICATION_UNDEFINED_EFLAGS(a_fEfl) do { } while (0)
#endif


/** @def IEM_DECL_IMPL_TYPE
 * For typedef'ing an instruction implementation function.
 *
 * @param   a_RetType           The return type.
 * @param   a_Name              The name of the type.
 * @param   a_ArgList           The argument list enclosed in parentheses.
 */

/** @def IEM_DECL_IMPL_DEF
 * For defining an instruction implementation function.
 *
 * @param   a_RetType           The return type.
 * @param   a_Name              The name of the type.
 * @param   a_ArgList           The argument list enclosed in parentheses.
 */

#if defined(__GNUC__) && defined(RT_ARCH_X86)
# define IEM_DECL_IMPL_TYPE(a_RetType, a_Name, a_ArgList) \
    __attribute__((__fastcall__)) a_RetType (a_Name) a_ArgList
# define IEM_DECL_IMPL_DEF(a_RetType, a_Name, a_ArgList) \
    __attribute__((__fastcall__, __nothrow__)) a_RetType a_Name a_ArgList

#elif defined(_MSC_VER) && defined(RT_ARCH_X86)
# define IEM_DECL_IMPL_TYPE(a_RetType, a_Name, a_ArgList) \
    a_RetType (__fastcall a_Name) a_ArgList
# define IEM_DECL_IMPL_DEF(a_RetType, a_Name, a_ArgList) \
    a_RetType __fastcall a_Name a_ArgList

#else
# define IEM_DECL_IMPL_TYPE(a_RetType, a_Name, a_ArgList) \
    a_RetType (VBOXCALL a_Name) a_ArgList
# define IEM_DECL_IMPL_DEF(a_RetType, a_Name, a_ArgList) \
    a_RetType VBOXCALL a_Name a_ArgList

#endif

/** @name Arithmetic assignment operations on bytes (binary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLBINU8,  (uint8_t  *pu8Dst,  uint8_t  u8Src,  uint32_t *pEFlags));
typedef FNIEMAIMPLBINU8  *PFNIEMAIMPLBINU8;
FNIEMAIMPLBINU8 iemAImpl_add_u8, iemAImpl_add_u8_locked;
FNIEMAIMPLBINU8 iemAImpl_adc_u8, iemAImpl_adc_u8_locked;
FNIEMAIMPLBINU8 iemAImpl_sub_u8, iemAImpl_sub_u8_locked;
FNIEMAIMPLBINU8 iemAImpl_sbb_u8, iemAImpl_sbb_u8_locked;
FNIEMAIMPLBINU8  iemAImpl_or_u8,  iemAImpl_or_u8_locked;
FNIEMAIMPLBINU8 iemAImpl_xor_u8, iemAImpl_xor_u8_locked;
FNIEMAIMPLBINU8 iemAImpl_and_u8, iemAImpl_and_u8_locked;
/** @} */

/** @name Arithmetic assignment operations on words (binary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLBINU16,  (uint16_t *pu16Dst, uint16_t u16Src, uint32_t *pEFlags));
typedef FNIEMAIMPLBINU16  *PFNIEMAIMPLBINU16;
FNIEMAIMPLBINU16 iemAImpl_add_u16, iemAImpl_add_u16_locked;
FNIEMAIMPLBINU16 iemAImpl_adc_u16, iemAImpl_adc_u16_locked;
FNIEMAIMPLBINU16 iemAImpl_sub_u16, iemAImpl_sub_u16_locked;
FNIEMAIMPLBINU16 iemAImpl_sbb_u16, iemAImpl_sbb_u16_locked;
FNIEMAIMPLBINU16  iemAImpl_or_u16,  iemAImpl_or_u16_locked;
FNIEMAIMPLBINU16 iemAImpl_xor_u16, iemAImpl_xor_u16_locked;
FNIEMAIMPLBINU16 iemAImpl_and_u16, iemAImpl_and_u16_locked;
/** @}  */

/** @name Arithmetic assignment operations on double words (binary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLBINU32, (uint32_t *pu32Dst, uint32_t u32Src, uint32_t *pEFlags));
typedef FNIEMAIMPLBINU32 *PFNIEMAIMPLBINU32;
FNIEMAIMPLBINU32 iemAImpl_add_u32, iemAImpl_add_u32_locked;
FNIEMAIMPLBINU32 iemAImpl_adc_u32, iemAImpl_adc_u32_locked;
FNIEMAIMPLBINU32 iemAImpl_sub_u32, iemAImpl_sub_u32_locked;
FNIEMAIMPLBINU32 iemAImpl_sbb_u32, iemAImpl_sbb_u32_locked;
FNIEMAIMPLBINU32  iemAImpl_or_u32,  iemAImpl_or_u32_locked;
FNIEMAIMPLBINU32 iemAImpl_xor_u32, iemAImpl_xor_u32_locked;
FNIEMAIMPLBINU32 iemAImpl_and_u32, iemAImpl_and_u32_locked;
/** @}  */

/** @name Arithmetic assignment operations on quad words (binary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLBINU64, (uint64_t *pu64Dst, uint64_t u64Src, uint32_t *pEFlags));
typedef FNIEMAIMPLBINU64 *PFNIEMAIMPLBINU64;
FNIEMAIMPLBINU64 iemAImpl_add_u64, iemAImpl_add_u64_locked;
FNIEMAIMPLBINU64 iemAImpl_adc_u64, iemAImpl_adc_u64_locked;
FNIEMAIMPLBINU64 iemAImpl_sub_u64, iemAImpl_sub_u64_locked;
FNIEMAIMPLBINU64 iemAImpl_sbb_u64, iemAImpl_sbb_u64_locked;
FNIEMAIMPLBINU64  iemAImpl_or_u64,  iemAImpl_or_u64_locked;
FNIEMAIMPLBINU64 iemAImpl_xor_u64, iemAImpl_xor_u64_locked;
FNIEMAIMPLBINU64 iemAImpl_and_u64, iemAImpl_and_u64_locked;
/** @}  */

/** @name Compare operations (thrown in with the binary ops).
 * @{ */
FNIEMAIMPLBINU8  iemAImpl_cmp_u8;
FNIEMAIMPLBINU16 iemAImpl_cmp_u16;
FNIEMAIMPLBINU32 iemAImpl_cmp_u32;
FNIEMAIMPLBINU64 iemAImpl_cmp_u64;
/** @}  */

/** @name Test operations (thrown in with the binary ops).
 * @{ */
FNIEMAIMPLBINU8  iemAImpl_test_u8;
FNIEMAIMPLBINU16 iemAImpl_test_u16;
FNIEMAIMPLBINU32 iemAImpl_test_u32;
FNIEMAIMPLBINU64 iemAImpl_test_u64;
/** @}  */

/** @name Bit operations operations (thrown in with the binary ops).
 * @{ */
FNIEMAIMPLBINU16 iemAImpl_bt_u16,  iemAImpl_bt_u16_locked;
FNIEMAIMPLBINU32 iemAImpl_bt_u32,  iemAImpl_bt_u32_locked;
FNIEMAIMPLBINU64 iemAImpl_bt_u64,  iemAImpl_bt_u64_locked;
FNIEMAIMPLBINU16 iemAImpl_btc_u16, iemAImpl_btc_u16_locked;
FNIEMAIMPLBINU32 iemAImpl_btc_u32, iemAImpl_btc_u32_locked;
FNIEMAIMPLBINU64 iemAImpl_btc_u64, iemAImpl_btc_u64_locked;
FNIEMAIMPLBINU16 iemAImpl_btr_u16, iemAImpl_btr_u16_locked;
FNIEMAIMPLBINU32 iemAImpl_btr_u32, iemAImpl_btr_u32_locked;
FNIEMAIMPLBINU64 iemAImpl_btr_u64, iemAImpl_btr_u64_locked;
FNIEMAIMPLBINU16 iemAImpl_bts_u16, iemAImpl_bts_u16_locked;
FNIEMAIMPLBINU32 iemAImpl_bts_u32, iemAImpl_bts_u32_locked;
FNIEMAIMPLBINU64 iemAImpl_bts_u64, iemAImpl_bts_u64_locked;
/** @}  */

/** @name Exchange memory with register operations.
 * @{ */
IEM_DECL_IMPL_DEF(void, iemAImpl_xchg_u8, (uint8_t  *pu8Mem,  uint8_t  *pu8Reg));
IEM_DECL_IMPL_DEF(void, iemAImpl_xchg_u16,(uint16_t *pu16Mem, uint16_t *pu16Reg));
IEM_DECL_IMPL_DEF(void, iemAImpl_xchg_u32,(uint32_t *pu32Mem, uint32_t *pu32Reg));
IEM_DECL_IMPL_DEF(void, iemAImpl_xchg_u64,(uint64_t *pu64Mem, uint64_t *pu64Reg));
/** @}  */

/** @name Double precision shifts
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTDBLU16,(uint16_t *pu16Dst, uint16_t u16Src, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTDBLU16  *PFNIEMAIMPLSHIFTDBLU16;
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTDBLU32,(uint32_t *pu32Dst, uint32_t u32Src, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTDBLU32  *PFNIEMAIMPLSHIFTDBLU32;
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTDBLU64,(uint64_t *pu64Dst, uint64_t u64Src, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTDBLU64  *PFNIEMAIMPLSHIFTDBLU64;
FNIEMAIMPLSHIFTDBLU16 iemAImpl_shld_u16;
FNIEMAIMPLSHIFTDBLU32 iemAImpl_shld_u32;
FNIEMAIMPLSHIFTDBLU64 iemAImpl_shld_u64;
FNIEMAIMPLSHIFTDBLU16 iemAImpl_shrd_u16;
FNIEMAIMPLSHIFTDBLU32 iemAImpl_shrd_u32;
FNIEMAIMPLSHIFTDBLU64 iemAImpl_shrd_u64;
/** @}  */


/** @name Bit search operations (thrown in with the binary ops).
 * @{ */
FNIEMAIMPLBINU16 iemAImpl_bsf_u16;
FNIEMAIMPLBINU32 iemAImpl_bsf_u32;
FNIEMAIMPLBINU64 iemAImpl_bsf_u64;
FNIEMAIMPLBINU16 iemAImpl_bsr_u16;
FNIEMAIMPLBINU32 iemAImpl_bsr_u32;
FNIEMAIMPLBINU64 iemAImpl_bsr_u64;
/** @}  */

/** @name Signed multiplication operations (thrown in with the binary ops).
 * @{ */
FNIEMAIMPLBINU16 iemAImpl_imul_two_u16;
FNIEMAIMPLBINU32 iemAImpl_imul_two_u32;
FNIEMAIMPLBINU64 iemAImpl_imul_two_u64;
/** @}  */

/** @name Arithmetic assignment operations on bytes (unary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLUNARYU8,  (uint8_t  *pu8Dst,  uint32_t *pEFlags));
typedef FNIEMAIMPLUNARYU8  *PFNIEMAIMPLUNARYU8;
FNIEMAIMPLUNARYU8 iemAImpl_inc_u8, iemAImpl_inc_u8_locked;
FNIEMAIMPLUNARYU8 iemAImpl_dec_u8, iemAImpl_dec_u8_locked;
FNIEMAIMPLUNARYU8 iemAImpl_not_u8, iemAImpl_not_u8_locked;
FNIEMAIMPLUNARYU8 iemAImpl_neg_u8, iemAImpl_neg_u8_locked;
/** @} */

/** @name Arithmetic assignment operations on words (unary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLUNARYU16,  (uint16_t  *pu16Dst,  uint32_t *pEFlags));
typedef FNIEMAIMPLUNARYU16  *PFNIEMAIMPLUNARYU16;
FNIEMAIMPLUNARYU16 iemAImpl_inc_u16, iemAImpl_inc_u16_locked;
FNIEMAIMPLUNARYU16 iemAImpl_dec_u16, iemAImpl_dec_u16_locked;
FNIEMAIMPLUNARYU16 iemAImpl_not_u16, iemAImpl_not_u16_locked;
FNIEMAIMPLUNARYU16 iemAImpl_neg_u16, iemAImpl_neg_u16_locked;
/** @} */

/** @name Arithmetic assignment operations on double words (unary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLUNARYU32,  (uint32_t  *pu32Dst,  uint32_t *pEFlags));
typedef FNIEMAIMPLUNARYU32  *PFNIEMAIMPLUNARYU32;
FNIEMAIMPLUNARYU32 iemAImpl_inc_u32, iemAImpl_inc_u32_locked;
FNIEMAIMPLUNARYU32 iemAImpl_dec_u32, iemAImpl_dec_u32_locked;
FNIEMAIMPLUNARYU32 iemAImpl_not_u32, iemAImpl_not_u32_locked;
FNIEMAIMPLUNARYU32 iemAImpl_neg_u32, iemAImpl_neg_u32_locked;
/** @} */

/** @name Arithmetic assignment operations on quad words (unary).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLUNARYU64,  (uint64_t  *pu64Dst,  uint32_t *pEFlags));
typedef FNIEMAIMPLUNARYU64  *PFNIEMAIMPLUNARYU64;
FNIEMAIMPLUNARYU64 iemAImpl_inc_u64, iemAImpl_inc_u64_locked;
FNIEMAIMPLUNARYU64 iemAImpl_dec_u64, iemAImpl_dec_u64_locked;
FNIEMAIMPLUNARYU64 iemAImpl_not_u64, iemAImpl_not_u64_locked;
FNIEMAIMPLUNARYU64 iemAImpl_neg_u64, iemAImpl_neg_u64_locked;
/** @} */


/** @name Shift operations on bytes (Group 2).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTU8,(uint8_t *pu8Dst, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTU8  *PFNIEMAIMPLSHIFTU8;
FNIEMAIMPLSHIFTU8 iemAImpl_rol_u8;
FNIEMAIMPLSHIFTU8 iemAImpl_ror_u8;
FNIEMAIMPLSHIFTU8 iemAImpl_rcl_u8;
FNIEMAIMPLSHIFTU8 iemAImpl_rcr_u8;
FNIEMAIMPLSHIFTU8 iemAImpl_shl_u8;
FNIEMAIMPLSHIFTU8 iemAImpl_shr_u8;
FNIEMAIMPLSHIFTU8 iemAImpl_sar_u8;
/** @} */

/** @name Shift operations on words (Group 2).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTU16,(uint16_t *pu16Dst, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTU16  *PFNIEMAIMPLSHIFTU16;
FNIEMAIMPLSHIFTU16 iemAImpl_rol_u16;
FNIEMAIMPLSHIFTU16 iemAImpl_ror_u16;
FNIEMAIMPLSHIFTU16 iemAImpl_rcl_u16;
FNIEMAIMPLSHIFTU16 iemAImpl_rcr_u16;
FNIEMAIMPLSHIFTU16 iemAImpl_shl_u16;
FNIEMAIMPLSHIFTU16 iemAImpl_shr_u16;
FNIEMAIMPLSHIFTU16 iemAImpl_sar_u16;
/** @} */

/** @name Shift operations on double words (Group 2).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTU32,(uint32_t *pu32Dst, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTU32  *PFNIEMAIMPLSHIFTU32;
FNIEMAIMPLSHIFTU32 iemAImpl_rol_u32;
FNIEMAIMPLSHIFTU32 iemAImpl_ror_u32;
FNIEMAIMPLSHIFTU32 iemAImpl_rcl_u32;
FNIEMAIMPLSHIFTU32 iemAImpl_rcr_u32;
FNIEMAIMPLSHIFTU32 iemAImpl_shl_u32;
FNIEMAIMPLSHIFTU32 iemAImpl_shr_u32;
FNIEMAIMPLSHIFTU32 iemAImpl_sar_u32;
/** @} */

/** @name Shift operations on words (Group 2).
 * @{ */
typedef IEM_DECL_IMPL_TYPE(void, FNIEMAIMPLSHIFTU64,(uint64_t *pu64Dst, uint8_t cShift, uint32_t *pEFlags));
typedef FNIEMAIMPLSHIFTU64  *PFNIEMAIMPLSHIFTU64;
FNIEMAIMPLSHIFTU64 iemAImpl_rol_u64;
FNIEMAIMPLSHIFTU64 iemAImpl_ror_u64;
FNIEMAIMPLSHIFTU64 iemAImpl_rcl_u64;
FNIEMAIMPLSHIFTU64 iemAImpl_rcr_u64;
FNIEMAIMPLSHIFTU64 iemAImpl_shl_u64;
FNIEMAIMPLSHIFTU64 iemAImpl_shr_u64;
FNIEMAIMPLSHIFTU64 iemAImpl_sar_u64;
/** @} */

/** @name Multiplication and division operations.
 * @{ */
typedef IEM_DECL_IMPL_TYPE(int, FNIEMAIMPLMULDIVU8,(uint16_t *pu16AX, uint8_t u8FactorDivisor, uint32_t *pEFlags));
typedef FNIEMAIMPLMULDIVU8  *PFNIEMAIMPLMULDIVU8;
FNIEMAIMPLMULDIVU8 iemAImpl_mul_u8, iemAImpl_imul_u8;
FNIEMAIMPLMULDIVU8 iemAImpl_div_u8, iemAImpl_idiv_u8;

typedef IEM_DECL_IMPL_TYPE(int, FNIEMAIMPLMULDIVU16,(uint16_t *pu16AX, uint16_t *pu16DX, uint16_t u16FactorDivisor, uint32_t *pEFlags));
typedef FNIEMAIMPLMULDIVU16  *PFNIEMAIMPLMULDIVU16;
FNIEMAIMPLMULDIVU16 iemAImpl_mul_u16, iemAImpl_imul_u16;
FNIEMAIMPLMULDIVU16 iemAImpl_div_u16, iemAImpl_idiv_u16;

typedef IEM_DECL_IMPL_TYPE(int, FNIEMAIMPLMULDIVU32,(uint32_t *pu32EAX, uint32_t *pu32EDX, uint32_t u32FactorDivisor, uint32_t *pEFlags));
typedef FNIEMAIMPLMULDIVU32  *PFNIEMAIMPLMULDIVU32;
FNIEMAIMPLMULDIVU32 iemAImpl_mul_u32, iemAImpl_imul_u32;
FNIEMAIMPLMULDIVU32 iemAImpl_div_u32, iemAImpl_idiv_u32;

typedef IEM_DECL_IMPL_TYPE(int, FNIEMAIMPLMULDIVU64,(uint64_t *pu64RAX, uint64_t *pu64RDX, uint64_t u64FactorDivisor, uint32_t *pEFlags));
typedef FNIEMAIMPLMULDIVU64  *PFNIEMAIMPLMULDIVU64;
FNIEMAIMPLMULDIVU64 iemAImpl_mul_u64, iemAImpl_imul_u64;
FNIEMAIMPLMULDIVU64 iemAImpl_div_u64, iemAImpl_idiv_u64;
/** @} */


/** @name C instruction implementations for anything slightly complicated.
 * @{ */

/**
 * For typedef'ing or declaring a C instruction implementation function taking
 * no extra arguments.
 *
 * @param   a_Name              The name of the type.
 */
# define IEM_CIMPL_DECL_TYPE_0(a_Name) \
    IEM_DECL_IMPL_TYPE(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr))
/**
 * For defining a C instruction implementation function taking no extra
 * arguments.
 *
 * @param   a_Name              The name of the function
 */
# define IEM_CIMPL_DEF_0(a_Name) \
    IEM_DECL_IMPL_DEF(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr))
/**
 * For calling a C instruction implementation function taking no extra
 * arguments.
 *
 * This special call macro adds default arguments to the call and allow us to
 * change these later.
 *
 * @param   a_fn                The name of the function.
 */
# define IEM_CIMPL_CALL_0(a_fn)            a_fn(pIemCpu, cbInstr)

/**
 * For typedef'ing or declaring a C instruction implementation function taking
 * one extra argument.
 *
 * @param   a_Name              The name of the type.
 * @param   a_Type0             The argument type.
 * @param   a_Arg0              The argument name.
 */
# define IEM_CIMPL_DECL_TYPE_1(a_Name, a_Type0, a_Arg0) \
    IEM_DECL_IMPL_TYPE(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0))
/**
 * For defining a C instruction implementation function taking one extra
 * argument.
 *
 * @param   a_Name              The name of the function
 * @param   a_Type0             The argument type.
 * @param   a_Arg0              The argument name.
 */
# define IEM_CIMPL_DEF_1(a_Name, a_Type0, a_Arg0) \
    IEM_DECL_IMPL_DEF(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0))
/**
 * For calling a C instruction implementation function taking one extra
 * argument.
 *
 * This special call macro adds default arguments to the call and allow us to
 * change these later.
 *
 * @param   a_fn                The name of the function.
 * @param   a0                  The name of the 1st argument.
 */
# define IEM_CIMPL_CALL_1(a_fn, a0)        a_fn(pIemCpu, cbInstr, (a0))

/**
 * For typedef'ing or declaring a C instruction implementation function taking
 * two extra arguments.
 *
 * @param   a_Name              The name of the type.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 */
# define IEM_CIMPL_DECL_TYPE_2(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1) \
    IEM_DECL_IMPL_TYPE(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0, a_Type1 a_Arg1))
/**
 * For defining a C instruction implementation function taking two extra
 * arguments.
 *
 * @param   a_Name              The name of the function.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 */
# define IEM_CIMPL_DEF_2(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1) \
    IEM_DECL_IMPL_DEF(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0, a_Type1 a_Arg1))
/**
 * For calling a C instruction implementation function taking two extra
 * arguments.
 *
 * This special call macro adds default arguments to the call and allow us to
 * change these later.
 *
 * @param   a_fn                The name of the function.
 * @param   a0                  The name of the 1st argument.
 * @param   a1                  The name of the 2nd argument.
 */
# define IEM_CIMPL_CALL_2(a_fn, a0, a1)    a_fn(pIemCpu, cbInstr, (a0), (a1))

/**
 * For typedef'ing or declaring a C instruction implementation function taking
 * three extra arguments.
 *
 * @param   a_Name              The name of the type.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 * @param   a_Type2             The type of the 3rd argument.
 * @param   a_Arg2              The name of the 3rd argument.
 */
# define IEM_CIMPL_DECL_TYPE_3(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1, a_Type2, a_Arg2) \
    IEM_DECL_IMPL_TYPE(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0, a_Type1 a_Arg1, a_Type2 a_Arg2))
/**
 * For defining a C instruction implementation function taking three extra
 * arguments.
 *
 * @param   a_Name              The name of the function.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 * @param   a_Type2             The type of the 3rd argument.
 * @param   a_Arg2              The name of the 3rd argument.
 */
# define IEM_CIMPL_DEF_3(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1, a_Type2, a_Arg2) \
    IEM_DECL_IMPL_DEF(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0, a_Type1 a_Arg1, a_Type2 a_Arg2))
/**
 * For calling a C instruction implementation function taking three extra
 * arguments.
 *
 * This special call macro adds default arguments to the call and allow us to
 * change these later.
 *
 * @param   a_fn                The name of the function.
 * @param   a0                  The name of the 1st argument.
 * @param   a1                  The name of the 2nd argument.
 * @param   a2                  The name of the 3rd argument.
 */
# define IEM_CIMPL_CALL_3(a_fn, a0, a1, a2) a_fn(pIemCpu, cbInstr, (a0), (a1), (a2))


/**
 * For typedef'ing or declaring a C instruction implementation function taking
 * four extra arguments.
 *
 * @param   a_Name              The name of the type.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 * @param   a_Type2             The type of the 3rd argument.
 * @param   a_Arg2              The name of the 3rd argument.
 * @param   a_Type3             The type of the 4th argument.
 * @param   a_Arg3              The name of the 4th argument.
 */
# define IEM_CIMPL_DECL_TYPE_4(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1, a_Type2, a_Arg2, a_Type3, a_Arg3) \
    IEM_DECL_IMPL_TYPE(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0, a_Type1 a_Arg1, a_Type2 a_Arg2, a_Type3 a_Arg3))
/**
 * For defining a C instruction implementation function taking four extra
 * arguments.
 *
 * @param   a_Name              The name of the function.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 * @param   a_Type2             The type of the 3rd argument.
 * @param   a_Arg2              The name of the 3rd argument.
 * @param   a_Type3             The type of the 4th argument.
 * @param   a_Arg3              The name of the 4th argument.
 */
# define IEM_CIMPL_DEF_4(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1, a_Type2, a_Arg2, a_Type3, aArg3) \
    IEM_DECL_IMPL_DEF(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, a_Type0 a_Arg0, a_Type1 a_Arg1, a_Type2 a_Arg2, a_Type3 a_Arg3))
/**
 * For calling a C instruction implementation function taking four extra
 * arguments.
 *
 * This special call macro adds default arguments to the call and allow us to
 * change these later.
 *
 * @param   a_fn                The name of the function.
 * @param   a0                  The name of the 1st argument.
 * @param   a1                  The name of the 2nd argument.
 * @param   a2                  The name of the 3rd argument.
 * @param   a3                  The name of the 4th argument.
 */
# define IEM_CIMPL_CALL_4(a_fn, a0, a1, a2, a3) a_fn(pIemCpu, cbInstr, (a0), (a1), (a2), (a3))


/**
 * For typedef'ing or declaring a C instruction implementation function taking
 * five extra arguments.
 *
 * @param   a_Name              The name of the type.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 * @param   a_Type2             The type of the 3rd argument.
 * @param   a_Arg2              The name of the 3rd argument.
 * @param   a_Type3             The type of the 4th argument.
 * @param   a_Arg3              The name of the 4th argument.
 * @param   a_Type4             The type of the 5th argument.
 * @param   a_Arg4              The name of the 5th argument.
 */
# define IEM_CIMPL_DECL_TYPE_5(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1, a_Type2, a_Arg2, a_Type3, a_Arg3, a_Type4, a_Arg4) \
    IEM_DECL_IMPL_TYPE(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, \
                                               a_Type0 a_Arg0, a_Type1 a_Arg1, a_Type2 a_Arg2, \
                                               a_Type3 a_Arg3, a_Type4 a_Arg4))
/**
 * For defining a C instruction implementation function taking five extra
 * arguments.
 *
 * @param   a_Name              The name of the function.
 * @param   a_Type0             The type of the 1st argument
 * @param   a_Arg0              The name of the 1st argument.
 * @param   a_Type1             The type of the 2nd argument.
 * @param   a_Arg1              The name of the 2nd argument.
 * @param   a_Type2             The type of the 3rd argument.
 * @param   a_Arg2              The name of the 3rd argument.
 * @param   a_Type3             The type of the 4th argument.
 * @param   a_Arg3              The name of the 4th argument.
 * @param   a_Type4             The type of the 5th argument.
 * @param   a_Arg4              The name of the 5th argument.
 */
# define IEM_CIMPL_DEF_5(a_Name, a_Type0, a_Arg0, a_Type1, a_Arg1, a_Type2, a_Arg2, a_Type3, a_Arg3, a_Type4, a_Arg4) \
    IEM_DECL_IMPL_DEF(VBOXSTRICTRC, a_Name, (PIEMCPU pIemCpu, uint8_t cbInstr, \
                                             a_Type0 a_Arg0, a_Type1 a_Arg1, a_Type2 a_Arg2, \
                                             a_Type3 a_Arg3, a_Type4 a_Arg4))
/**
 * For calling a C instruction implementation function taking five extra
 * arguments.
 *
 * This special call macro adds default arguments to the call and allow us to
 * change these later.
 *
 * @param   a_fn                The name of the function.
 * @param   a0                  The name of the 1st argument.
 * @param   a1                  The name of the 2nd argument.
 * @param   a2                  The name of the 3rd argument.
 * @param   a3                  The name of the 4th argument.
 * @param   a4                  The name of the 5th argument.
 */
# define IEM_CIMPL_CALL_5(a_fn, a0, a1, a2, a3, a4) a_fn(pIemCpu, cbInstr, (a0), (a1), (a2), (a3), (a4))

/** @}  */


/** @} */

RT_C_DECLS_END

#endif

