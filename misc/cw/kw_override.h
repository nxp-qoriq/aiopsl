

#define _Packed
#define _PackedType

#define __dest_os   6  /* __ppc_eabi */

#define KW_EXCLUDE

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef unsigned long       uintptr_t;

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;
typedef signed long         intptr_t;

typedef unsigned long       size_t;


#define fsl_os_exit(n) abort()

#kw_override ASSERT_COND(_cond)                 \
    do { if (!(_cond)) abort(); } while (0)

#kw_override REPORT_ERROR(_level, _err, _vmsg)  \
    do { XX_Print("%d, %d, ", (_level), (_err)); XX_Print _vmsg; } while (0)

#kw_override DBG(_level, _vmsg)                 \
    do { XX_Print("%d, ", (_level)); XX_Print _vmsg; } while (0)



