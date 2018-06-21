#ifndef CUMO_NARRAY_H
#define CUMO_NARRAY_H

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#include <math.h>
#include "cumo/compat.h"
#include "cumo/template.h"
#include "cumo/extconf.h"

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifndef HAVE_U_INT8_T
# ifdef HAVE_UINT8_T
    typedef uint8_t u_int8_t;
# endif
#endif

#ifndef HAVE_U_INT16_T
# ifdef HAVE_UINT16_T
    typedef uint16_t u_int16_t;
# endif
#endif

#ifndef HAVE_U_INT32_T
# ifdef HAVE_UINT32_T
    typedef uint32_t u_int32_t;
# endif
#endif

#ifndef HAVE_U_INT64_T
# ifdef HAVE_UINT64_T
    typedef uint64_t u_int64_t;
# endif
#endif

#define SZF PRI_SIZE_PREFIX // defined in ruby.h

#if   SIZEOF_LONG==8
# define NUM2INT64(x) NUM2LONG(x)
# define INT642NUM(x) LONG2NUM(x)
# define NUM2UINT64(x) NUM2ULONG(x)
# define UINT642NUM(x) ULONG2NUM(x)
# ifndef PRId64
#  define PRId64 "ld"
# endif
# ifndef PRIu64
#  define PRIu64 "lu"
# endif
#elif SIZEOF_LONG_LONG==8
# define NUM2INT64(x) NUM2LL(x)
# define INT642NUM(x) LL2NUM(x)
# define NUM2UINT64(x) NUM2ULL(x)
# define UINT642NUM(x) ULL2NUM(x)
# ifndef PRId64
#  define PRId64 "lld"
# endif
# ifndef PRIu64
#  define PRIu64 "llu"
# endif
#endif

#if   SIZEOF_LONG==4
# define NUM2INT32(x) NUM2LONG(x)
# define INT322NUM(x) LONG2NUM(x)
# define NUM2UINT32(x) NUM2ULONG(x)
# define UINT322NUM(x) ULONG2NUM(x)
# ifndef PRId32
#  define PRId32 "ld"
# endif
# ifndef PRIu32
#  define PRIu32 "lu"
# endif
#elif SIZEOF_INT==4
# define NUM2INT32(x) NUM2INT(x)
# define INT322NUM(x) INT2NUM(x)
# define NUM2UINT32(x) NUM2UINT(x)
# define UINT322NUM(x) UINT2NUM(x)
# ifndef PRId32
#  define PRId32 "d"
# endif
# ifndef PRIu32
#  define PRIu32 "u"
# endif
#endif

#if SIZEOF_VALUE > 4
# undef INT322NUM
# undef UINT322NUM
# define INT322NUM(x) INT2FIX(x)
# define UINT322NUM(x) INT2FIX(x)
#endif

#ifndef HAVE_TYPE_BOOL
  typedef int bool;
#endif
#ifndef FALSE                   /* in case these macros already exist */
# define FALSE   0              /* values of bool */
#endif
#ifndef TRUE
# define TRUE    1
#endif

typedef struct { float dat[2]; }  scomplex;
typedef struct { double dat[2]; } dcomplex;
typedef int fortran_integer;

#define REAL(x) ((x).dat[0])
#define IMAG(x) ((x).dat[1])

extern int cumo_na_debug_flag;

#define mCumo rb_mCumo
extern VALUE rb_mCumo;
#define cNArray cumo_cNArray
extern VALUE cumo_cNArray;
extern VALUE cumo_na_eCastError;
extern VALUE cumo_na_eShapeError;
extern VALUE cumo_na_eOperationError;
extern VALUE cumo_na_eDimensionError;
extern VALUE cumo_na_eValueError;
extern const rb_data_type_t cumo_na_data_type;

//EXTERN const int cumo_na_sizeof[NA_NTYPES+1];

//#define cumo_na_upcast(x,y) cumo_na_upcast(x,y)

/* global variables within this module */
extern VALUE cumo_cBit;
extern VALUE cumo_cDFloat;
extern VALUE cumo_cSFloat;
extern VALUE cumo_cDComplex;
extern VALUE cumo_cSComplex;
extern VALUE cumo_cInt64;
extern VALUE cumo_cInt32;
extern VALUE cumo_cInt16;
extern VALUE cumo_cInt8;
extern VALUE cumo_cUInt64;
extern VALUE cumo_cUInt32;
extern VALUE cumo_cUInt16;
extern VALUE cumo_cUInt8;
extern VALUE cumo_cRObject;
extern VALUE cumo_na_cStep;
#ifndef HAVE_RB_CCOMPLEX
extern VALUE rb_cComplex;
#endif

extern VALUE cumo_sym_reduce;
extern VALUE cumo_sym_option;
extern VALUE cumo_sym_loop_opt;
extern VALUE cumo_sym_init;

#define NARRAY_DATA_T     0x1
#define NARRAY_VIEW_T     0x2
#define NARRAY_FILEMAP_T  0x3

typedef struct RNArray {
    unsigned char ndim;     // # of dimensions
    unsigned char type;
    unsigned char flag[2];  // flags
    unsigned short elmsz;    // element size
    size_t   size;          // # of total elements
    size_t  *shape;         // # of elements for each dimension
    VALUE    reduce;
} cumo_narray_t;


typedef struct RNArrayData {
    cumo_narray_t base;
    char    *ptr;
} cumo_narray_data_t;


typedef union {
    ssize_t stride;
    size_t *index;
} stridx_t;

typedef struct RNArrayView {
    cumo_narray_t base;
    VALUE    data;       // data object
    size_t   offset;     // offset of start point from data pointer
                         // :in units of elm.unit_bits
                         // address_unit  pointer_unit access_unit data_unit
                         // elm.step_unit = elm.bit_size / elm.access_unit
                         // elm.step_unit = elm.size_bits / elm.unit_bits
    stridx_t *stridx;    // stride or indices of data pointer for each dimension
} cumo_narray_view_t;


// filemap is unimplemented
typedef struct RNArrayFileMap {
    cumo_narray_t base;
    char    *ptr;
#ifdef WIN32
    HANDLE hFile;
    HANDLE hMap;
#else // POSIX mmap
    int prot;
    int flag;
#endif
} cumo_narray_filemap_t;


// this will be revised in future.
typedef struct {
    unsigned int element_bits;
    unsigned int element_bytes;
    unsigned int element_stride;
} cumo_narray_type_info_t;


static inline cumo_narray_t *
cumo_na_get_narray_t(VALUE obj)
{
    cumo_narray_t *na;

    Check_TypedStruct(obj,&cumo_na_data_type);
    na = (cumo_narray_t*)DATA_PTR(obj);
    return na;
}

static inline cumo_narray_t *
_cumo_na_get_narray_t(VALUE obj, unsigned char cumo_na_type)
{
    cumo_narray_t *na;

    Check_TypedStruct(obj,&cumo_na_data_type);
    na = (cumo_narray_t*)DATA_PTR(obj);
    if (na->type != cumo_na_type) {
        rb_bug("unknown type 0x%x (0x%x given)", cumo_na_type, na->type);
    }
    return na;
}

#define cumo_na_get_narray_data_t(obj) (cumo_narray_data_t*)_cumo_na_get_narray_t(obj,NARRAY_DATA_T)
#define cumo_na_get_narray_view_t(obj) (cumo_narray_view_t*)_cumo_na_get_narray_t(obj,NARRAY_VIEW_T)
#define cumo_na_get_narray_filemap_t(obj) (cumo_narray_filemap_t*)_cumo_na_get_narray_t(obj,NARRAY_FILEMAP_T)

#define GetNArray(obj,var)      TypedData_Get_Struct(obj, cumo_narray_t, &cumo_na_data_type, var)
#define GetNArrayView(obj,var)  TypedData_Get_Struct(obj, cumo_narray_view_t, &cumo_na_data_type, var)
#define GetNArrayData(obj,var)  TypedData_Get_Struct(obj, cumo_narray_data_t, &cumo_na_data_type, var)

#define SDX_IS_STRIDE(x) ((x).stride&0x1)
#define SDX_IS_INDEX(x)  (!SDX_IS_STRIDE(x))
#define SDX_GET_STRIDE(x) ((x).stride>>1)
#define SDX_GET_INDEX(x)  ((x).index)

#define SDX_SET_STRIDE(x,s) ((x).stride=((s)<<1)|0x1)
#define SDX_SET_INDEX(x,idx) ((x).index=idx)

#define RNARRAY(val)            ((cumo_narray_t*)DATA_PTR(val))
#define RNARRAY_DATA(val)       ((cumo_narray_data_t*)DATA_PTR(val))
#define RNARRAY_VIEW(val)       ((cumo_narray_view_t*)DATA_PTR(val))
#define RNARRAY_FILEMAP(val)    ((cumo_narray_filemap_t*)DATA_PTR(val))

#define RNARRAY_NDIM(val)       (RNARRAY(val)->ndim)
#define RNARRAY_TYPE(val)       (RNARRAY(val)->type)
#define RNARRAY_FLAG(val)       (RNARRAY(val)->flag)
#define RNARRAY_SIZE(val)       (RNARRAY(val)->size)
#define RNARRAY_SHAPE(val)      (RNARRAY(val)->shape)
#define RNARRAY_REDUCE(val)     (RNARRAY(val)->reduce)

#define RNARRAY_DATA_PTR(val)    (RNARRAY_DATA(val)->ptr)
#define RNARRAY_VIEW_DATA(val)   (RNARRAY_VIEW(val)->data)
#define RNARRAY_VIEW_OFFSET(val) (RNARRAY_VIEW(val)->offset)
#define RNARRAY_VIEW_STRIDX(val) (RNARRAY_VIEW(val)->stridx)

#define NA_NDIM(na)     (((cumo_narray_t*)na)->ndim)
#define NA_TYPE(na)     (((cumo_narray_t*)na)->type)
#define NA_SIZE(na)     (((cumo_narray_t*)na)->size)
#define NA_SHAPE(na)    (((cumo_narray_t*)na)->shape)
#define NA_REDUCE(na)   (((cumo_narray_t*)na)->reduce)

#define NA_FLAG(obj)    (cumo_na_get_narray_t(obj)->flag)
#define NA_FLAG0(obj)   (NA_FLAG(obj)[0])
#define NA_FLAG1(obj)   (NA_FLAG(obj)[1])

#define NA_DATA(na)             ((cumo_narray_data_t*)(na))
#define NA_VIEW(na)             ((cumo_narray_view_t*)(na))
#define NA_DATA_PTR(na)         (NA_DATA(na)->ptr)
#define NA_VIEW_DATA(na)        (NA_VIEW(na)->data)
#define NA_VIEW_OFFSET(na)      (NA_VIEW(na)->offset)
#define NA_VIEW_STRIDX(na)      (NA_VIEW(na)->stridx)

#define NA_IS_INDEX_AT(na,i)    (SDX_IS_INDEX(NA_VIEW_STRIDX(na)[i]))
#define NA_IS_STRIDE_AT(na,i)   (SDX_IS_STRIDE(NA_VIEW_STRIDX(na)[i]))
#define NA_INDEX_AT(na,i)       (SDX_GET_INDEX(NA_VIEW_STRIDX(na)[i]))
#define NA_STRIDE_AT(na,i)      (SDX_GET_STRIDE(NA_VIEW_STRIDX(na)[i]))

#define NA_FILEMAP_PTR(na)      (((cumo_narray_filemap_t*)na)->ptr)


#define NA_FL0_TEST(x,f) (NA_FLAG0(x)&(f))
#define NA_FL1_TEST(x,f) (NA_FLAG1(x)&(f))

#define NA_FL0_SET(x,f) do {NA_FLAG0(x) |= (f);} while(0)
#define NA_FL1_SET(x,f) do {NA_FLAG1(x) |= (f);} while(0)

#define NA_FL0_UNSET(x,f) do {NA_FLAG0(x) &= ~(f);} while(0)
#define NA_FL1_UNSET(x,f) do {NA_FLAG1(x) &= ~(f);} while(0)

#define NA_FL0_REVERSE(x,f) do {NA_FLAG0(x) ^= (f);} while(0)
#define NA_FL1_REVERSE(x,f) do {NA_FLAG1(x) ^= (f);} while(0)


/* FLAGS
   - row-major / column-major
   - Overwrite or not
   - byteswapp
   - Extensible?
   - matrix or not
*/

#define NA_FL0_BIG_ENDIAN     (0x1<<0)
#define NA_FL0_COLUMN_MAJOR   (0x1<<1)
#define NA_FL1_LOCK           (0x1<<0)
#define NA_FL1_INPLACE        (0x1<<1)

#define TEST_COLUMN_MAJOR(x)   NA_FL0_TEST(x,NA_FL0_COLUMN_MAJOR)
#define SET_COLUMN_MAJOR(x)    NA_FL0_SET(x,NA_FL0_COLUMN_MAJOR)
#define UNSET_COLUMN_MAJOR(x)  NA_FL0_UNSET(x,NA_FL0_COLUMN_MAJOR)

#define TEST_ROW_MAJOR(x)      (!TEST_COLUMN_MAJOR(x))
#define SET_ROW_MAJOR(x)       UNSET_COLUMN_MAJOR(x)
#define UNSET_ROW_MAJOR(x)     SET_COLUMN_MAJOR(x)

#define TEST_BIG_ENDIAN(x)     NA_FL0_TEST(x,NA_FL0_BIG_ENDIAN)
#define SET_BIG_ENDIAN(x)      NA_FL0_SET(x,NA_FL0_BIG_ENDIAN)
#define UNSET_BIG_ENDIAN(x)    NA_FL0_UNSET(x,NA_FL0_BIG_ENDIAN)

#define TEST_LITTLE_ENDIAN(x)  (!TEST_BIG_ENDIAN(x))
#define SET_LITTLE_ENDIAN(x)   UNSET_BIG_ENDIAN(x)
#define UNSET_LITTLE_ENDIAN(x) SET_BIG_ENDIAN(x)

#define REVERSE_ENDIAN(x)      NA_FL0_REVERSE((x),NA_FL0_BIG_ENDIAN)

#define TEST_LOCK(x)           NA_FL1_TEST(x,NA_FL1_LOCK)
#define SET_LOCK(x)            NA_FL1_SET(x,NA_FL1_LOCK)
#define UNSET_LOCK(x)          NA_FL1_UNSET(x,NA_FL1_LOCK)

#define TEST_INPLACE(x)        NA_FL1_TEST(x,NA_FL1_INPLACE)
#define SET_INPLACE(x)         NA_FL1_SET(x,NA_FL1_INPLACE)
#define UNSET_INPLACE(x)       NA_FL1_UNSET(x,NA_FL1_INPLACE)

#ifdef DYNAMIC_ENDIAN
// not supported
#else
#ifdef WORDS_BIGENDIAN
#define TEST_HOST_ORDER(x)     TEST_BIG_ENDIAN(x)
#define SET_HOST_ORDER(x)      SET_BIG_ENDIAN(x)
#define UNSET_HOST_ORDER(x)    UNSET_BIG_ENDIAN(x)
#define TEST_BYTE_SWAPPED(x)   TEST_LITTLE_ENDIAN(x)
#define SET_BYTE_SWAPPED(x)    SET_LITTLE_ENDIAN(x)
#define UNSET_BYTE_SWAPPED(x)  UNSET_LITTLE_ENDIAN(x)
#define NA_FL0_INIT            NA_FL0_BIG_ENDIAN
#else // LITTLE ENDIAN
#define TEST_HOST_ORDER(x)     TEST_LITTLE_ENDIAN(x)
#define SET_HOST_ORDER(x)      SET_LITTLE_ENDIAN(x)
#define UNSET_HOST_ORDER(x)    UNSET_LITTLE_ENDIAN(x)
#define TEST_BYTE_SWAPPED(x)   TEST_BIG_ENDIAN(x)
#define SET_BYTE_SWAPPED(x)    SET_BIG_ENDIAN(x)
#define UNSET_BYTE_SWAPPED(x)  UNSET_BIG_ENDIAN(x)
#define NA_FL0_INIT            0
#endif
#endif
#define NA_FL1_INIT            0


#define IsNArray(obj) (rb_obj_is_kind_of(obj,cNArray)==Qtrue)

#define DEBUG_PRINT(v) puts(StringValueCStr(rb_funcall(v,rb_intern("inspect"),0)))

#define NA_IsNArray(obj) \
  (rb_obj_is_kind_of(obj,cNArray)==Qtrue)
#define NA_IsArray(obj) \
  (TYPE(obj)==T_ARRAY || rb_obj_is_kind_of(obj,cNArray)==Qtrue)

#define NUM2REAL(v)  NUM2DBL( rb_funcall((v),cumo_na_id_real,0) )
#define NUM2IMAG(v)  NUM2DBL( rb_funcall((v),cumo_na_id_imag,0) )

//#define NA_MAX_DIMENSION (int)(sizeof(VALUE)*8-2)
#define NA_MAX_DIMENSION 12
#define NA_MAX_ELMSZ     65535

typedef unsigned int BIT_DIGIT;
#define BYTE_BIT_DIGIT sizeof(BIT_DIGIT)
#define NB     (sizeof(BIT_DIGIT)*8)
#define BALL   (~(BIT_DIGIT)0)
#define SLB(n) (((n)==NB)?~(BIT_DIGIT)0:(~(~(BIT_DIGIT)0<<(n))))

#define ELEMENT_BIT_SIZE  "ELEMENT_BIT_SIZE"
#define ELEMENT_BYTE_SIZE "ELEMENT_BYTE_SIZE"
#define CONTIGUOUS_STRIDE "CONTIGUOUS_STRIDE"


#ifdef RUBY_INTEGER_UNIFICATION
#define IS_INTEGER_CLASS(c) ((c)==rb_cInteger)
#else
#define IS_INTEGER_CLASS(c) ((c)==rb_cFixnum||(c)==rb_cBignum)
#endif

#include "cumo/ndloop.h"
#include "cumo/intern.h"

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* ifndef CUMO_NARRAY_H */
