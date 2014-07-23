
#ifndef __RTA_RTA_H__
#define __RTA_RTA_H__

#include "rta/sec_run_time_asm.h"
#include "rta/fifo_load_store_cmd.h"
#include "rta/header_cmd.h"
#include "rta/jump_cmd.h"
#include "rta/key_cmd.h"
#include "rta/load_cmd.h"
#include "rta/math_cmd.h"
#include "rta/move_cmd.h"
#include "rta/nfifo_cmd.h"
#include "rta/operation_cmd.h"
#include "rta/protocol_cmd.h"
#include "rta/seq_in_out_ptr_cmd.h"
#include "rta/signature_cmd.h"
#include "rta/store_cmd.h"

/**
 * @file                 rta.h
 * @brief                RTA API definition.
 */

/**
 * @defgroup rta_api_group RTA API
 * Contains RTA API details.
 *
 * @defgroup program_group Descriptor Buffer Management Routines
 * @ingroup rta_api_group
 * Contains details of RTA descriptor buffer management and SEC Era
 * management routines.
 *
 * @defgroup cmd_group SEC Commands Routines
 * @ingroup rta_api_group
 * Contains details of RTA wrapper routines over SEC engine commands.
 *
 * @defgroup refcode_group Self Referential Code Management Routines
 * @ingroup rta_api_group
 * Contains details of RTA self referential code routines.
 */

/**
 * @mainpage
 *
 * Runtime Assembler Library is an easy and flexible runtime method for
 * writing SEC descriptors. Its implements a thin abstraction layer above
 * SEC commands set; the resulting code is compact and similar to a
 * descriptor sequence.
 *
 * RTA library improves comprehension of the SEC code, adds flexibility for
 * writing complex descriptors and keeps the code lightweight. Should be used
 * by whom needs to encode descriptors at runtime, with comprehensible flow
 * control in descriptor.
 *
 * The \subpage userman "User Manual" page contains more details about RTA.
 *
 * \image html rta_arch.png "RTA Integration Overview" width=10cm
 *
 * In SDK package, an example of RTA usage in user space is included. The
 * inclusion of RTA in kernel space in the diagram above is only demonstrative.
 */

/** @addtogroup program_group
 *  @{
 */

/**
 * @def                PROGRAM_CNTXT_INIT
 * @details            @b PROGRAM_CNTXT_INIT must be called before any
 *                     descriptor run-time assembly call type field carry
 *                     info i.e. whether descriptor is shared or Job
 *                     descriptor.
 *
 * @param[in] buffer   Input buffer where the descriptor will be placed
 *                     (@c uint32_t *).
 * @param[in] offset   Offset in input buffer from where the data will be
 *                     written (@c unsigned).
 */
#define PROGRAM_CNTXT_INIT(buffer, offset) \
	rta_program_cntxt_init(program, buffer, offset)

/**
 * @def                PROGRAM_FINALIZE
 * @details            @b PROGRAM_FINALIZE must be called to mark completion
 *                     of @b RTA call.
 * @return             Total size of the descriptor in words (@c unsigned).
 */
#define PROGRAM_FINALIZE() rta_program_finalize(program)

/**
 * @def                PROGRAM_SET_36BIT_ADDR
 * @details            @b PROGRAM_SET_36BIT_ADDR must be called to set pointer
 *                     size to 36 bits.
 * @return             Current size of the descriptor in words (@c unsigned).
 */
#define PROGRAM_SET_36BIT_ADDR() rta_program_set_36bit_addr(program)

/**
 * @def                PROGRAM_SET_BSWAP
 * @details            @b PROGRAM_SET_BSWAP must be called to enable byte
 *                     swapping. Byte swapping on a 4-byte boundary will be
 *                     performed at the end - when calling PROGRAM_FINALIZE().
 * @return             Current size of the descriptor in words (@c unsigned).
 */
#define PROGRAM_SET_BSWAP() rta_program_set_bswap(program)

/**
 * @def                WORD
 * @details            @b WORD must be called to insert in descriptor buffer a
 *                     32bits value.
 *
 * @param[in] val      Input value to be written in descriptor buffer
 *                     (@c uint32_t).
 * @return             Descriptor buffer offset where this command is inserted
 *                     (@c unsigned).
 */
#define WORD(val) rta_word(program, val)

/**
 * @def                DWORD
 * @details            @b DWORD must be called to insert in descriptor buffer a
 *                     64bits value.
 *
 * @param[in] val      Input value to be written in descriptor buffer
 *                     (@c uint64_t).
 * @return             Descriptor buffer offset where this command is inserted
 *                     (@c unsigned).
 */
#define DWORD(val) rta_dword(program, val)

/**
 * @def                COPY_DATA
 * @details            @b COPY_DATA must be called to insert in descriptor buffer
 *                     data larger than 64bits.
 *
 * @param[in] data     Input data to be written in descriptor buffer
 *                     (@c uint8_t *).
 * @param[in] len      Length of input data (@c unsigned).
 * @return             Descriptor buffer offset where this command is inserted
 *                     (@c unsigned).
 */
#define COPY_DATA(data, len) rta_copy_data(program, (data), (len))

/**
 * @def                SHR_DESC_LEN
 * @details            @b SHR_DESC_LEN determines shared descriptor buffer
 *                     length in words.
 *
 * @param[in] buffer   Shared descriptor buffer (@c uint32_t *).
 * @return             Shared descriptor buffer length (@c unsigned).
 */
#define SHR_DESC_LEN(buffer) rta_desc_len(buffer, HDR_DESCLEN_SHR_MASK)

/**
 * @def                SHR_DESC_BYTES
 * @details            @b SHR_DESC_BYTES determines shared descriptor buffer
 *                     length in bytes.
 *
 * @param[in] buffer   Shared descriptor buffer (@c uint32_t *).
 * @return             Shared descriptor buffer length (@c unsigned).
 */
#define SHR_DESC_BYTES(buffer) rta_desc_bytes(buffer, HDR_DESCLEN_SHR_MASK)

/**
 * @def                JOB_DESC_LEN
 * @details            @b JOB_DESC_LEN determines job descriptor buffer length
 *                     in words.
 *
 * @param[in] buffer   Job descriptor buffer (@c uint32_t *).
 * @return             Job descriptor buffer length (@c unsigned).
 */
#define JOB_DESC_LEN(buffer) rta_desc_len(buffer, HDR_DESCLEN_MASK)

/**
 * @def                JOB_DESC_BYTES
 * @details            @b JOB_DESC_BYTES determines job descriptor buffer length
 *                     in bytes.
 *
 * @param[in] buffer   Job descriptor buffer (@c uint32_t *).
 * @return             Job descriptor buffer length (@c unsigned).
 */
#define JOB_DESC_BYTES(buffer) rta_desc_bytes(buffer, HDR_DESCLEN_MASK)

/**
 * @brief              SEC HW block revision.
 *
 * This *must not be confused with SEC version*:
 * - SEC HW block revision format is "v"
 * - SEC revision format is "x.y"
 */
extern enum rta_sec_era rta_sec_era;

/**
 * @brief              Set SEC Era HW block revision for which the RTA library
 *                     will generate the descriptors.
 * @warning            Must be called *only once*, *before* using any other
 *                     RTA API routine.
 * @warning            *Not thread safe*.
 *
 * @param[in] era      SEC Era (<c> enum rta_sec_era</c>).
 * @return             0 if the ERA was set successfully, -1 otherwise
 *                     (@c int).
 */
static inline int rta_set_sec_era(enum rta_sec_era era)
{
	if (era > MAX_SEC_ERA) {
		rta_sec_era = DEFAULT_SEC_ERA;
		pr_err("Unsupported SEC ERA. Defaulting to ERA %d\n",
		       DEFAULT_SEC_ERA + 1);
		return -1;
	}

	rta_sec_era = era;
	return 0;
}

/**
 * @brief              Get SEC Era HW block revision for which the RTA library
 *                     will generate the descriptors.
 *
 * @return             SEC Era (@c unsigned).
 */
static inline unsigned rta_get_sec_era(void)
{
	 return rta_sec_era;
}

/** @} */ /* end of program_group */


/** @addtogroup cmd_group
 *  @{
 */

/**
 * @def                SHR_HDR
 * @details            Configures Shared Descriptor @b HEADER command
 *
 * @param[in] share    Descriptor share state:@n @li <em>SHR_ALWAYS, SHR_SERIAL,
 *                     SHR_NEVER, SHR_WAIT</em>.
 * @param[in] start_idx Index in descriptor buffer where the execution of the
 *                     Shared Descriptor should start (@c unsigned).
 * @param[in] flags    Operational flags:@n @li <em>RIF, DNR, CIF, SC, PD</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SHR_HDR(share, start_idx, flags) \
	rta_shr_header(program, share, start_idx, flags)

/**
 * @def                JOB_HDR
 * @details            Configures JOB Descriptor @b HEADER command.
 *
 * @param[in] share    Descriptor share state:@n @li <em>SHR_ALWAYS,
 *                     SHR_SERIAL, SHR_NEVER, SHR_WAIT, SHR_DEFER</em>.
 * @param[in] start_idx Index in descriptor buffer where the execution of the
 *                     Job Descriptor should start (@c unsigned).
 *                     In case SHR bit is present in flags, this will be the
 *                     shared descriptor length.
 * @param[in] share_desc Pointer to shared descriptor, in case @em SHR bit is
 *                     set (@c uint64_t).
 * @param[in] flags    Operational flags:@n @li <em>RSMS, DNR, TD, MTD, REO,
 *                     SHR</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JOB_HDR(share, start_idx, share_desc, flags) \
	rta_job_header(program, share, start_idx, share_desc, flags, 0)

/**
 * @def                JOB_HDR_EXT
 * @details            Configures JOB Descriptor @b HEADER command.
 *
 * @param[in] share    Descriptor share state:@n @li <em>SHR_ALWAYS,
 *                     SHR_SERIAL, SHR_NEVER, SHR_WAIT, SHR_DEFER</em>.
 * @param[in] start_idx Index in descriptor buffer where the execution of the
 *                     Job Descriptor should start (@c unsigned).
 *                     In case SHR bit is present in flags, this will be the
 *                     shared descriptor length.
 * @param[in] share_desc Pointer to shared descriptor, in case @em SHR bit is
 *                     set (@c uint64_t).
 * @param[in] flags    Operational flags:@n @li <em>RSMS, DNR, TD, MTD, REO,
 *                     SHR</em>.
 * @param[in] ext_flags Extended header flags: @n @li <em>DSV (DECO Select
 *                     Valid), DECO Id (limited by DSEL_MASK)</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JOB_HDR_EXT(share, start_idx, share_desc, flags, ext_flags) \
	rta_job_header(program, share, start_idx, share_desc, flags | EXT, \
		       ext_flags)

/**
 * @def                MOVE
 * @details            Configures @b MOVE and @b MOVE_LEN commands.
 *
 * @param[in] src      Internal source of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOABD, IFIFOAB1, IFIFOAB2, AB1, AB2, ABD</em>.
 * @param[in] src_offset Offset in source data (@c uint16_t).
 * @param[in] dst      Internal destination of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOAB1, IFIFOAB2, IFIFO, PKA, KEY1, KEY2,
 *                     ALTSOURCE</em>.
 * @param[in] dst_offset Offset in destination data (@c uint16_t).
 * @param[in] length   Size of data to be moved:@n @li for @b MOVE should be
 *                     specified using @b IMM macro; @li for @b MOVE_LEN
 *                     should be specified using @e MATH0-MATH3.
 * @param[in] opt      Operational flags:@n @li <em>WAITCOMP, FLUSH1, FLUSH2,
 *                     LAST1, LAST2, SIZE_WORD, SIZE_BYTE, SIZE_DWORD</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on error;
 *                     In debug mode, a log message will be shown at output.
 */
#define MOVE(src, src_offset, dst, dst_offset, length, opt) \
	rta_move(program, __MOVE, src, src_offset, dst, dst_offset, length, opt)

/**
 * @def                MOVEB
 * @details            Configures @b MOVEB command. Identical with @b MOVE
 *                     command if byte swapping not enabled; else - when src/dst
 *                     is descriptor buffer or MATH registers, data type is
 *                     byte array when MOVE data type is 4-byte array and
 *                     vice versa.
 *
 * @param[in] src      Internal source of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOABD, IFIFOAB1, IFIFOAB2, AB1, AB2, ABD</em>.
 * @param[in] src_offset Offset in source data (@c uint16_t).
 * @param[in] dst      Internal destination of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOAB1, IFIFOAB2, IFIFO, PKA, KEY1, KEY2,
 *                     ALTSOURCE</em>.
 * @param[in] dst_offset Offset in destination data (@c uint16_t).
 * @param[in] length   Size of data to be moved:@n @li for @b MOVE should be
 *                     specified using @b IMM macro; @li for @b MOVE_LEN
 *                     should be specified using @e MATH0-MATH3.
 * @param[in] opt      Operational flags:@n @li <em>WAITCOMP, FLUSH1, FLUSH2,
 *                     LAST1, LAST2, SIZE_WORD, SIZE_BYTE, SIZE_DWORD</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on error;
 *                     In debug mode, a log message will be shown at output.
 */
#define MOVEB(src, src_offset, dst, dst_offset, length, opt) \
	rta_move(program, __MOVEB, src, src_offset, dst, dst_offset, length, \
		 opt)

/**
 * @def                MOVEDW
 * @details            Configures @b MOVEDW command. Identical with @b MOVE
 *                     command, with the following differences:@n @li data type
 *                     is 8-byte array. @li word swapping is performed when SEC
 *                     is programmed in little endian mode.
 *
 * @param[in] src      Internal source of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOABD, IFIFOAB1, IFIFOAB2, AB1, AB2, ABD</em>.
 * @param[in] src_offset Offset in source data (@c uint16_t).
 * @param[in] dst      Internal destination of data that will be moved:@n @li
 *                     <em>CONTEXT1, CONTEXT2, OFIFO, DESCBUF, MATH0-MATH3,
 *                     IFIFOAB1, IFIFOAB2, IFIFO, PKA, KEY1, KEY2,
 *                     ALTSOURCE</em>.
 * @param[in] dst_offset Offset in destination data (@c uint16_t).
 * @param[in] length   Size of data to be moved:@n @li for @b MOVE should be
 *                     specified using @b IMM macro; @li for @b MOVE_LEN
 *                     should be specified using @e MATH0-MATH3.
 * @param[in] opt      Operational flags:@n @li <em>WAITCOMP, FLUSH1, FLUSH2,
 *                     LAST1, LAST2, SIZE_WORD, SIZE_BYTE, SIZE_DWORD</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on error;
 *                     In debug mode, a log message will be shown at output.
 */
#define MOVEDW(src, src_offset, dst, dst_offset, length, opt) \
	rta_move(program, __MOVEDW, src, src_offset, dst, dst_offset, length, \
		 opt)

/**
 * @def                FIFOLOAD
 * @details            Configures SEC @b FIFOLOAD command to load message data,
 *                     PKHA data, IV, ICV, AAD and bit length message data into
 *                     Input Data FIFO.
 *
 * @param[in] data     Input data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, MSG1, MSG2, MSGOUTSNOOP, MSGINSNOOP, IV1, IV2,
 *                     AAD1, ICV1, ICV2, BIT_DATA, SKIP</em>.
 * @param[in] src      Pointer or actual data in case of immediate load; @b IMM
 *                     or @b PTR macros must be used to indicate type.
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, IMMED, EXT, CLASS1,
 *                     CLASS2, BOTH, FLUSH1, LAST1, LAST2</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define FIFOLOAD(data, src, length, flags) \
	rta_fifo_load(program, data, src, length, flags)

/**
 * @def                SEQFIFOLOAD
 * @details            Configures SEC <b>SEQ FIFOLOAD</b> command to load
 *                     message data, PKHA data, IV, ICV, AAD and bit length
 *                     message data into Input Data FIFO.
 *
 * @param[in] data     Input data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, MSG1, MSG2, MSGOUTSNOOP, MSGINSNOOP, IV1, IV2,
 *                     AAD1, ICV1, ICV2, BIT_DATA, SKIP</em>.
 * @param[in] length   Number of bytes to load; can be set to 0 for SEQ command
 *                     w/ @e VLF set (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>VLF, CLASS1, CLASS2, BOTH,
 *                     FLUSH1, LAST1, LAST2, AIDF</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQFIFOLOAD(data, length, flags) \
	rta_fifo_load(program, data, NONE, length, WITH(flags|SEQ))

/**
 * @def                FIFOSTORE
 * @details            Configures SEC @b FIFOSTORE command, to move data from
 *                     Output Data FIFO to external memory via DMA.
 *
 * @param[in] data     Output data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, OFIFO, RNG, RNGOFIFO, AFHA_SBOX, MDHA_SPLIT_KEY,
 *                     MSG, KEY1, KEY2, SKIP</em>.
 * @param[in] encrypt_flags  Store data encryption mode:@n @li <em>EKT, NRM,
 *                     TK, JDK</em>.
 * @param[in] dst      Pointer to store location (@c uint64_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, CONT, EXT, CLASS1,
 *                     CLASS2, BOTH</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     ommand is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define FIFOSTORE(data, encrypt_flags, dst, length, flags) \
	rta_fifo_store(program, data, encrypt_flags, dst, length, flags)

/**
 * @def                SEQFIFOSTORE
 * @details            Configures SEC <b>SEQ FIFOSTORE</b> command, to move
 *                     data from Output Data FIFO to external memory via DMA.
 *
 * @param[in] data     Output data type to store:@n @li <em>PKHA registers,
 *                     IFIFO, OFIFO, RNG, RNGOFIFO, AFHA_SBOX, MDHA_SPLIT_KEY,
 *                     MSG, KEY1, KEY2, METADATA, SKIP</em>.
 * @param[in] encrypt_flags  Store data encryption mode:@n @li <em>EKT, NRM,
 *                     TK, JDK</em>.
 * @param[in] length   Number of bytes to load; can be set to 0 for SEQ command
 *                     w/ @e VLF set (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>VLF, CONT, EXT, CLASS1,
 *                     CLASS2, BOTH</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQFIFOSTORE(data, encrypt_flags, length, flags) \
	rta_fifo_store(program, data, encrypt_flags, 0, length, WITH(flags|SEQ))

/**
 * @def                KEY
 * @details            Configures SEC @b KEY and <b>SEQ KEY</b> commands.
 *
 * @param[in] key_dst  Key store location:@n @li <em>KEY1, KEY2, PKE,
 *                     AFHA_SBOX, MDHA_SPLIT_KEY</em>.
 * @param[in] encrypt_flags  Key encryption mode:@n @li <em>ENC, EKT, TK,
 *                     NWB, PTS</em>.
 * @param[in] src      Pointer or actual data in case of immediate load
 *                     (@c uint64_t).
 * @param[in] length   Number of bytes to load; can be set to 0 for SEQ command
 *                     w/ @e VLF set (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li for KEY: <em>SGF, IMMED</em>.
 *                     @li for SEQKEY: <em>SEQ, VLF, AIDF</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define KEY(key_dst, encrypt_flags, src, length, flags) \
	rta_key(program, key_dst, encrypt_flags, src, length, flags)

/**
 * @def                SEQINPTR
 * @details            Configures SEC <b>SEQ IN PTR</b> command.
 *
 * @param[in] src      Starting address for Input Sequence (@c uint64_t).
 * @param[in] length   Number of bytes in (or to be added to) Input Sequence
 *                     (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>RBS, INL, SGF, PRE, EXT,
 *                     RTO, RJD, SOP</em> (when @e PRE, @e RTO or @e SOP are
 *                     set, @e src parameter must be 0).
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQINPTR(src, length, flags) \
	rta_seq_in_ptr(program, src, length, flags)

/**
 * @def                SEQOUTPTR
 * @details            Configures SEC <b>SEQ OUT PTR</b> command.
 *
 * @param[in] dst      Starting address for Output Sequence (@c uint64_t).
 * @param[in] length   Number of bytes in (or to be added to) Output Sequence
 *                     (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>SGF, PRE, EXT, RTO, RST, EWS
 *                     </em> (when @e PRE or @e RTO are set, @e dst parameter
 *                     must be 0).
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQOUTPTR(dst, length, flags) \
	rta_seq_out_ptr(program, dst, length, flags)

/**
 * @def                  ALG_OPERATION
 * @details              Configures <b>ALGORITHM OPERATION</b> command.
 *
 * @param[in] cipher_alg Algorithm to be used.
 * @param[in] aai        Additional algorithm information; contains mode
 *                       information that is associated with the algorithm
 *                       (check desc.h for specific values).
 * @param[in] algo_state Algorithm state; defines the state of the algorithm
 *                       that is being executed (check desc.h file for specific
 *                       values).
 * @param[in] icv_check  ICV checking; selects whether the algorithm should
 *                       check calculated ICV with known ICV:@n @li
 *                       <em>ICV_CHECK_ENABLE, ICV_CHECK_DISABLE</em>.
 * @param[in] enc        Selects between encryption and decryption:@n @li
 *                       <em>OP_ALG_ENCRYPT, OP_ALG_DECRYPT</em>.
 * @return               @li On success, descriptor buffer offset where this
 *                       command is inserted (@c unsigned).
 *                       @li First error program counter will be incremented on
 *                       error; in debug mode, a log message will be shown at
 *                       output.
 */
#define ALG_OPERATION(cipher_alg, aai, algo_state, icv_check, enc) \
	rta_operation(program, cipher_alg, aai, algo_state, icv_check, enc)

/**
 * @def                PROTOCOL
 * @details            Configures <b>PROTOCOL OPERATION</b> command.
 *
 * @param[in] optype   Operation type:@n @li <em>OP_TYPE_UNI_PROTOCOL/
 *                     OP_TYPE_DECAP_PROTOCOL/OP_TYPE_ENCAP_PROTOCOL</em>.
 * @param[in] protid   Protocol identifier value (check desc.h file for
 *                     specific values).
 * @param[in] protoinfo  Protocol dependent value (check desc.h file for
 *                     specific values).
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define PROTOCOL(optype, protid, protoinfo) \
	rta_proto_operation(program, optype, protid, protoinfo)

/**
 * @def                PKHA_OPERATION
 * @details            Configures <b>PKHA OPERATION</b> command.
 *
 * @param[in] op_pkha  PKHA operation; indicates the modular arithmetic
 *                     function to execute (check desc.h file for specific
 *                     values).
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define PKHA_OPERATION(op_pkha)   rta_pkha_operation(program, op_pkha)

/**
 * @def                JUMP
 * @details            Configures @b JUMP command.
 *
 * @param[in] addr     Local offset for local jumps or address pointer for
 *                     non-local jumps; @b IMM or @b PTR macros must be used to
 *                     indicate type.
 * @param[in] jump_type Type of action taken by jump:@n @li <em>LOCAL_JUMP,
 *                     GOSUB, RETURN, HALT, HALT_STATUS, FAR_JUMP</em>.
 * @param[in] test_type Defines how jump conditions are evaluated:@n @li
 *                     <em>ALL_TRUE, ALL_FALSE, ANY_TRUE, ANY_FALSE</em>.
 * @param[in] cond     Jump conditions:@n @li operational flags <em>DONE1,
 *                     DONE2, BOTH</em>.
 *                     @li various sharing and wait conditions (@e JSL = 1):
 *                     <em>NIFP, NIP, NOP, NCP, CALM, SELF, SHARED, JQP</em>.
 *                     @li Math and PKHA status conditions (@e JSL = 0):
 *                     <em>Z, N, NV, C, PK0, PK1, PKP</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JUMP(addr, jump_type, test_type, cond) \
	rta_jump(program, addr, jump_type, test_type, cond, _NONE, 0)

/**
 * @def                JUMP_INC
 * @details            Configures @b JUMP_INC command.
 *
 * @param[in] addr     Local offset; @b IMM or @b PTR macros must be used to
 *                     indicate type.
 * @param[in] test_type Defines how jump conditions are evaluated:@n @li
 *                     <em>ALL_TRUE, ALL_FALSE, ANY_TRUE, ANY_FALSE</em>.
 * @param[in] cond     Jump conditions:@n @li Math status conditions
 *                     (@e JSL = 0): <em>Z, N, NV, C</em>.
 * @param[in] src_dst  Register to increment / decrement:@n @li <em>MATH0-MATH3,
 *                     DPOVRD, SEQINSZ, SEQOUTSZ, VSEQINSZ, VSEQOUTSZ</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JUMP_INC(addr, test_type, cond, src_dst) \
	rta_jump(program, addr, LOCAL_JUMP_INC, test_type, cond, src_dst)

/**
 * @def                JUMP_DEC
 * @details            Configures @b JUMP_DEC command.
 *
 * @param[in] addr     Local offset; @b IMM or @b PTR macros must be used to
 *                     indicate type.
 * @param[in] test_type Defines how jump conditions are evaluated:@n @li
 *                     <em>ALL_TRUE, ALL_FALSE, ANY_TRUE, ANY_FALSE</em>.
 * @param[in] cond     Jump conditions:@n @li Math status conditions
 *                     (@e JSL = 0): <em>Z, N, NV, C</em>.
 * @param[in] src_dst  Register to increment / decrement:@n @li <em>MATH0-MATH3,
 *                     DPOVRD, SEQINSZ, SEQOUTSZ, VSEQINSZ, VSEQOUTSZ</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define JUMP_DEC(addr, test_type, cond, src_dst) \
	rta_jump(program, addr, LOCAL_JUMP_DEC, test_type, cond, src_dst)

/**
 * @def                LOAD
 * @details            Configures SEC @b LOAD command to load data registers
 *                     from descriptor or from a memory location.
 *
 * @param[in] addr     Immediate value or pointer to the data to be loaded;
 *                     @b IMM or @b PTR macros must be used to indicate type.
 * @param[in] dst      Destination register (@c uint64_t).
 * @param[in] offset   Start point to write data in destination register
 *                     (@c uint32_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags: @e VLF.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define LOAD(addr, dst, offset, length, flags) \
	rta_load(program, addr, dst, offset, length, flags)

/**
 * @def                SEQLOAD
 * @details            Configures SEC <b>SEQ LOAD</b> command to load data
 *                     registers from descriptor or from a memory location.
 *
 * @param[in] dst      Destination register (@c uint64_t).
 * @param[in] offset   Start point to write data in destination register
 *                     (@c uint32_t).
 * @param[in] length   Number of bytes to load (@c uint32_t).
 * @param[in] flags    Operational flags: @e SGF.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQLOAD(dst, offset, length, flags) \
	rta_load(program, NONE, dst, offset, length, WITH(flags|SEQ))

/**
 * @def                STORE
 * @details            Configures SEC @b STORE command to read data from
 *                     registers and write them to a memory location.
 *
 * @param[in] src      Immediate value or source register for data to be
 *                     stored:@n @li <em>KEY1SZ, KEY2SZ, DJQDA, MODE1, MODE2,
 *                     DJQCTRL, DATA1SZ, DATA2SZ, DSTAT, ICV1SZ, ICV2SZ, DPID,
 *                     CCTRL, ICTRL, CLRW, CSTAT, MATH0-MATH3, PKHA registers,
 *                     CONTEXT1, CONTEXT2, DESCBUF, JOBDESCBUF,
 *                     SHAREDESCBUF</em>.
 *                     @b IMM must be used to indicate immediate value.
 * @param[in] offset   Start point for reading from source register
 *                     (@c uint16_t).
 * @param[in] dst      Pointer to store location; @b PTR must be used to
 *                     indicate pointer value (@c uint64_t).
 * @param[in] length   Number of bytes to store (@c uint32_t).
 * @param[in] flags    Operational flags:@n @li <em>VLF, IMM</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define STORE(src, offset, dst, length, flags) \
	rta_store(program, src, offset, dst, length, flags)

/**
 * @def                SEQSTORE
 * @details            Configures SEC <b>SEQ STORE</b> command to read data
 *                     from registers and write them to a memory location.
 *
 * @param[in] src      Immediate value or source register for data to be
 *                     stored:@n @li <em>KEY1SZ, KEY2SZ, DJQDA, MODE1, MODE2,
 *                     DJQCTRL, DATA1SZ, DATA2SZ, DSTAT, ICV1SZ, ICV2SZ, DPID,
 *                     CCTRL, ICTRL, CLRW, CSTAT, MATH0-MATH3, PKHA registers,
 *                     CONTEXT1, CONTEXT2, DESCBUF, JOBDESCBUF,
 *                     SHAREDESCBUF</em>.
 *                     @n @b IMM must be used to indicate immediate value.
 * @param[in] offset   Start point for reading from source register
 *                     (@c uint16_t).
 * @param[in] length   Number of bytes to store (@c uint32_t).
 * @param[in] flags    Operational flags: @e SGF.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SEQSTORE(src, offset, length, flags) \
	rta_store(program, src, offset, NONE, length, WITH(flags|SEQ))

/**
 * @def                MATHB
 * @details            Configures SEC @b MATH command to perform binary
 *                     operations.
 *
 * @param[in] operand1 First operand:@n @li <em>MATH0-MATH3, DPOVRD, SEQINSZ,
 *                     SEQOUTSZ, VSEQINSZ, VSEQOUTSZ, ZERO, ONE, NONE,
 *                     Immediate value</em>.
 *                     @n @b IMM must be used to indicate immediate value.
 * @param[in] operator Function to be performed:@n @li <em>ADD, ADDC, SUB,
 *                     SUBB, OR, AND, XOR, LSHIFT, RSHIFT, SHLD</em>.
 * @param[in] operand2 Second operand:@n @li <em>MATH0-MATH3, DPOVRD, VSEQINSZ,
 *                     VSEQOUTSZ, ABD, OFIFO, JOBSRC, ZERO, ONE, SEQINSZ,
 *                     Immediate value</em>.@n @b IMM must be used to indicate
 *                     immediate value.
 * @param[in] result   Destination for the result:@n @li <em>MATH0-MATH3,
 *                     DPOVRD, SEQINSZ, SEQOUTSZ, NONE, VSEQINSZ,
 *                     VSEQOUTSZ</em>.
 * @param[in] length   Length in bytes of the operation and the immediate
 *                     value, if there is one (@c int).
 * @param[in] opt      Operational flags: @e IFB, NFU, STL, SWP.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define MATHB(operand1, operator, operand2, result, length, opt) \
	rta_math(program, operand1, operator, operand2, result, length, opt)

/**
 * @def                MATHU
 * @details            Configures SEC @b MATH command to perform unary
 *                     operations.
 *
 * @param[in] operand1 Operand:@n @li <em>MATH0-MATH3, DPOVRD, SEQINSZ,
 *                     SEQOUTSZ, VSEQINSZ, VSEQOUTSZ, ZERO, ONE, NONE,
 *                     Immediate value</em>. @n @b IMM must be used to
 *                     indicate immediate value.
 * @param[in] operator Function to be performed:@n @li <em>ZBYTE, BSWAP</em>.
 * @param[in] result   Destination for the result:@n @li <em>MATH0-MATH3,
 *                     DPOVRD, SEQINSZ, SEQOUTSZ, NONE, VSEQINSZ,
 *                     VSEQOUTSZ</em>.
 * @param[in] length   Length in bytes of the operation and the immediate
 *                     value, if there is one (@c int).
 * @param[in] opt      Operational flags: @e NFU, STL, SWP.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define MATHU(operand1, operator, result, length, opt) \
	rta_math(program, operand1, operator, _NONE, 0, result, length, opt)

/**
 * @def                SIGNATURE
 * @details            Configures @b SIGNATURE command.
 *
 * @param[in] sign_type Signature type:@n @li <em>SIGN_TYPE_FINAL,
 *                     SIGN_TYPE_FINAL_RESTORE, SIGN_TYPE_FINAL_NONZERO,
 *                     SIGN_TYPE_IMM_2, SIGN_TYPE_IMM_3, SIGN_TYPE_IMM_4</em>.
 *                     @n After @b SIGNATURE command, @b DWORD or
 *                     @b WORD must be used to insert signature in descriptor
 *                     buffer.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define SIGNATURE(sign_type)   rta_signature(program, sign_type)

/**
 * @def                NFIFOADD
 * @details            Configures @b NFIFO command, a shortcut of RTA Load
 *                     command to write to iNfo FIFO.
 *
 * @param[in] src      Source for the input data in Alignment Block:@n @li
 *                     <em>IFIFO, OFIFO, PAD, MSGOUTSNOOP, ALTSOURCE,
 *                     OFIFO_SYNC, MSGOUTSNOOP_ALT</em>.
 * @param[in] data     Type of data that is going through the Input Data FIFO:
 *                     @n @li <em>MSG, MSG1, MSG2, IV1, IV2, ICV1, ICV2, SAD1,
 *                     AAD1, AAD2, AFHA_SBOX, SKIP, PKHA registers, AB1, AB2,
 *                     ABD</em>.
 * @param[in] length   Length of the data copied in FIFO registers
 *                     (@c uint32_t).
 * @param[in] flags    Select options between:@n @li operational flags:
 *                     <em>LAST1, LAST2, FLUSH1, FLUSH2, OC, BP</em>.
 *                     @li when PAD is selected as source: <em>BM, PR, PS</em>.
 *		       @li padding type: <em>PAD_ZERO, PAD_NONZERO,
 *		       PAD_INCREMENT, PAD_RANDOM, PAD_ZERO_N1, PAD_NONZERO_0,
 *		       PAD_N1, PAD_NONZERO_N</em>.
 * @return             @li On success, descriptor buffer offset where this
 *                     command is inserted (@c unsigned).
 *                     @li First error program counter will be incremented on
 *                     error; in debug mode, a log message will be shown at
 *                     output.
 */
#define NFIFOADD(src, data, length, flags) \
	rta_nfifo_load(program, src, data, length, flags)

/** @} */ /* end of cmd_group */

/** @addtogroup refcode_group
 *  @{
 */

/**
 * @def                REFERENCE
 * @details            @b REFERENCE initialize a variable used for storing an
 *                     index inside a descriptor buffer.
 *
 * @param[in] ref      Reference to a descriptor buffer's index where an update
 *                     is required with a value that will be known latter in
 *                     the program flow.
 */
#define REFERENCE(ref)    unsigned ref = 0xFFFFFFFF

/**
 * @def                LABEL
 * @details            @b LABEL initialize a variable used for storing an index
 *                     inside a descriptor buffer.
 *
 * @param[in] label    Label stores the value with what should be updated the
 *                     REFERENCE line in the descriptor buffer.
 */
#define LABEL(label)      unsigned label = 0

/**
 * @def                SET_LABEL
 * @details            @b SET_LABEL routine sets a @b LABEL value.
 *
 * @param[in] label    Value that will be inserted in a line previously written
 *                     in the descriptor buffer.
 */
#define SET_LABEL(label)  label = rta_set_label(program)

/**
 * @def                PATCH_JUMP
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b JUMP command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_JUMP(line, new_ref) rta_patch_jmp(program, line, new_ref, 0)

/**
 * @def                PATCH_JUMP_NON_LOCAL
 * @details            Auxiliary command to resolve referential code between
 *                     two program buffers.
 *
 * @param[in] src_program Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in source descriptor buffer where the update
 *                     will be done; this value is previously retained in
 *                     program flow using a reference near the sequence to be
 *                     modified (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in source descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b JUMP command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_JUMP_NON_LOCAL(src_program, line, new_ref) \
	rta_patch_jmp(src_program, line, new_ref, 1)

/**
 * @def                PATCH_MOVE
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b MOVE command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_MOVE(line, new_ref) rta_patch_move(program, line, new_ref, 0)

/**
 * @def                PATCH_MOVE_NON_LOCAL
 * @details            Auxiliary command to resolve referential code between
 *                     two program buffers.
 *
 * @param[in] src_program Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in source descriptor buffer where the update
 *                     will be done; this value is previously retained in
 *                     program flow using a reference near the sequence to be
 *                     modified (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in source descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b MOVE command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_MOVE_NON_LOCAL(src_program, line, new_ref) \
	rta_patch_move(src_program, line, new_ref, 1)

/**
 * @def                PATCH_LOAD
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b LOAD command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_LOAD(line, new_ref) rta_patch_load(program, line, new_ref)

/**
 * @def                PATCH_STORE
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b STORE command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_STORE(line, new_ref) rta_patch_store(program, line, new_ref, 0)

/**
 * @def                PATCH_STORE_NON_LOCAL
 * @details            Auxiliary command to resolve referential code between
 *                     two program buffers.
 *
 * @param[in] src_program Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in source descriptor buffer where the update
 *                     will be done; this value is previously retained in
 *                     program flow using a reference near the sequence to be
 *                     modified (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in source descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b STORE command, the value represents the offset
 *                     field (in words).
 */
#define PATCH_STORE_NON_LOCAL(src_program, line, new_ref) \
	rta_patch_store(src_program, line, new_ref, 1)

/**
 * @def                PATCH_HDR
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b HEADER command, the value represents the start
 *                     index field.
 */
#define PATCH_HDR(line, new_ref) rta_patch_header(program, line, new_ref, 0)

/**
 * @def                PATCH_HDR_NON_LOCAL
 * @details            Auxiliary command to resolve referential code between
 *                     two program buffers.
 *
 * @param[in] src_program Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in source descriptor buffer where the update
 *                     will be done; this value is previously retained in
 *                     program flow using a reference near the sequence to be
 *                     modified (@c unsigned).
 * @param[in] new_ref  Updated value that will be inserted in source descriptor
 *                     buffer at the specified line; this value is previously
 *                     obtained using @b SET_LABEL macro near the line that
 *                     will be used as reference (@c unsigned).
 *                     For @b HEADER command, the value represents the start
 *                     index field.
 *
 */
#define PATCH_HDR_NON_LOCAL(src_program, line, new_ref) \
	rta_patch_header(src_program, line, new_ref, 1)

/**
 * @def                PATCH_RAW
 * @details            Auxiliary command to resolve self referential code.
 *
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] mask     Mask to be used for applying the new value (@c unsigned).
 *                     The mask selects which bits from the provided @new_val
 *                     are taken into consideration when overwriting the
 *                     existing value.
 * @param[in] new_val  Updated value that will be masked using the provided
 *                     mask value and inserted in descriptor buffer at the
 *                     specified line.
 *
 */
#define PATCH_RAW(line, mask, new_val) \
	rta_patch_raw(program, line, mask, new_val, 0)

/**
 * @def                PATCH_RAW_NON_LOCAL
 * @details            Auxiliary command to resolve referential code between
 *                     two program buffers.
 *
 * @param[in] src_program Buffer to be updated (<c>struct program *</c>).
 * @param[in] line     Position in descriptor buffer where the update will be
 *                     done; this value is previously retained in program flow
 *                     using a reference near the sequence to be modified
 *                     (@c unsigned).
 * @param[in] mask     Mask to be used for applying the new value (@c unsigned).
 *                     The mask selects which bits from the provided @new_val
 *                     are taken into consideration when overwriting the
 *                     existing value.
 * @param[in] new_val  Updated value that will be masked using the provided
 *                     mask value and inserted in descriptor buffer at the
 *                     specified line.
 *
 */
#define PATCH_RAW_NON_LOCAL(src_program, line, mask, new_val) \
	rta_patch_raw(src_program, line, mask, new_val, 1)
/** @} */ /* end of refcode_group */


#endif /* __RTA_RTA_H__ */
