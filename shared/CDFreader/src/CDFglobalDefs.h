#ifndef CDFGLOBALDEFS_H
#define CDFGLOBALDEFS_H

#include "CDFattribute.h"
#include "CDFvariable.h"
#include "QMap"

/******************************************************************************
* types
******************************************************************************/

namespace CDFr
{
typedef float       float_t;
typedef float       real4_t;
typedef double      double_t;
typedef double      real8_t;
typedef double      epoch_t;
typedef int8_t      int1_t;
typedef int8_t      byte_t;
typedef u_int8_t    uint1_t;
typedef int16_t     int2_t;
typedef u_int16_t   uint2_t;
typedef int32_t     int4_t;
typedef u_int32_t   uint4_t;
typedef int64_t     int8_t;
typedef int64_t     time_tt2000_t;
typedef char        char_t;
typedef uchar       uchar_t;

//TODO: EPOCH16 needs to be worked on
}


/******************************************************************************
* TypeDefs.
******************************************************************************/
namespace CDFr {

class CDFattribute;
class CDFvariable;

typedef  QMap<QString, CDFr::CDFattribute*> attributeList;
typedef  QMap<QString, CDFr::CDFvariable*> variableList;
}
/******************************************************************************
* Encoding (for data only, everything else is network encoding).
******************************************************************************/

#define CDFr_NETWORK_ENCODING        1L
#define CDFr_SUN_ENCODING            2L
#define CDFr_VAX_ENCODING            3L
#define CDFr_DECSTATION_ENCODING     4L
#define CDFr_SGi_ENCODING            5L
#define CDFr_IBMPC_ENCODING          6L
#define CDFr_IBMRS_ENCODING          7L
#define CDFr_HOST_ENCODING           8L
#define CDFr_PPC_ENCODING            9L
#define CDFr_HP_ENCODING             11L
#define CDFr_NeXT_ENCODING           12L
#define CDFr_ALPHAOSF1_ENCODING      13L
#define CDFr_ALPHAVMSd_ENCODING      14L
#define CDFr_ALPHAVMSg_ENCODING      15L
#define CDFr_ALPHAVMSi_ENCODING      16L


/******************************************************************************
* Variance flags
******************************************************************************/

#define CDFr_VARY   (-1L)        /* TRUE record or dimension variance flag */
#define CDFr_NOVARY 0L           /* FALSE record or dimension variance flag */

/******************************************************************************
* Majorities
******************************************************************************/

#define CDFr_ROW_MAJOR       1L
#define CDFr_COLUMN_MAJOR    2L

/******************************************************************************
* Formats.
******************************************************************************/

#define CDFr_SINGLE_FILE     1L
#define CDFr_MULTI_FILE      2L

/******************************************************************************
* Checksum
******************************************************************************/

#define CDFr_NO_CHECKSUM     0L
#define CDFr_MD5_CHECKSUM    1L
#define CDFr_OTHER_CHECKSUM  2L

/******************************************************************************
* Attribute scopes
******************************************************************************/

#define CDFr_GLOBAL_SCOPE            1L
#define CDFr_VARIABLE_SCOPE          2L

/******************************************************************************
* Readonly modes.
******************************************************************************/

#define CDFr_READONLYon              (-1L)
#define CDFr_READONLYoff             0L

/******************************************************************************
* Validate data modes.
******************************************************************************/

#define CDFr_VALIDATEFILEon          (-1L)
#define CDFr_VALIDATEFILEoff         0L

/******************************************************************************
* zModes.
******************************************************************************/

#define CDFr_zMODEoff                0L
#define CDFr_zMODEon1                1L
#define CDFr_zMODEon2                2L

/******************************************************************************
* Negative to positive floating point zero modes.
******************************************************************************/

#define CDFr_NEGtoPOSfp0on           (-1L)
#define CDFr_NEGtoPOSfp0off          0L

/******************************************************************************
* Backward file mode.
******************************************************************************/

#define CDFr_BACKWARDFILEon          1
#define CDFr_BACKWARDFILEoff         0

/******************************************************************************
* Compression/sparseness constants.
******************************************************************************/

#define CDFr_CDF_MAX_PARMS			5
#define CDFr_NO_COMPRESSION			0L
#define CDFr_RLE_COMPRESSION			1L
#define CDFr_HUFF_COMPRESSION		2L
#define CDFr_AHUFF_COMPRESSION		3L
/**************************************************
* Compression `4' used to be RICE.  Do not reuse! *
**************************************************/
#define CDFr_GZIP_COMPRESSION		5L
/* #define ZLIB_COMPRESSION                6L */

#define CDFr_RLE_OF_ZEROs			0L
#define CDFr_OPTIMAL_ENCODING_TREES		0L
/* #define ZLIB_OF_ZEROs                   0L */
#define CDFr_NO_SPARSEARRAYS			0L
#define CDFr_NO_SPARSERECORDS		0L
#define CDFr_PAD_SPARSERECORDS		1L
#define CDFr_PREV_SPARSERECORDS		2L




#endif // CDFGLOBALDEFS_H
