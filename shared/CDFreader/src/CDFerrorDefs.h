#ifndef CDFERRORDEFS_H
#define CDFERRORDEFS_H

//These are the definitions of error codes for the CDF reader

#define CDFr_VIRTUAL_RECORD_DATA             1001
#define CDFr_DID_NOT_COMPRESS                1002
#define CDFr_VAR_ALREADY_CLOSED              1003
#define CDFr_SINGLE_FILE_FORMAT              1004
#define CDFr_NO_PADVALUE_SPECIFIED           1005
#define CDFr_NO_VARS_IN_CDF                  1006
#define CDFr_MULTI_FILE_FORMAT               1007
#define CDFr_SOME_ALREADY_ALLOCATED          1008
#define CDFr_PRECEEDING_RECORDS_ALLOCATED    1009

#define CDFr_OK                               0

#define CDFr_ATTR_NAME_TRUNC                 -1001
#define CDFr_NAME_TRUNC                      -1002
#define CDFr_VAR_NAME_TRUNC                  -1003
#define CDFr_NEGATIVE_FP_ZERO                -1004
                    /* -1005 unused. */
#define CDFr_FORCED_PARAMETER                -1006
#define CDFr_NA_FOR_VARIABLE                 -1007

#define CDFr_WARN                            -2000

#define CDFr_ATTR_EXISTS                     -2001
#define CDFr_BAD_CDF_ID                      -2002
#define CDFr_BAD_DATA_TYPE                   -2003
#define CDFr_BAD_DIM_SIZE                    -2004
#define CDFr_BAD_DIM_INDEX                   -2005
#define CDFr_BAD_ENCODING                    -2006
#define CDFr_BAD_MAJORITY                    -2007
#define CDFr_BAD_NUM_DIMS                    -2008
#define CDFr_BAD_REC_NUM                     -2009
#define CDFr_BAD_SCOPE                       -2010
#define CDFr_BAD_NUM_ELEMS                   -2011
#define CDFr_CDF_OPEN_ERROR                  -2012
#define CDFr_CDF_EXISTS                      -2013
#define CDFr_BAD_FORMAT                      -2014
#define CDFr_BAD_ALLOCATE_RECS               -2015
#define CDFr_BAD_CDF_EXTENSION               -2016
#define CDFr_NO_SUCH_ATTR                    -2017
#define CDFr_NO_SUCH_ENTRY                   -2018
#define CDFr_NO_SUCH_VAR                     -2019
#define CDFr_VAR_READ_ERROR                  -2020
#define CDFr_VAR_WRITE_ERROR                 -2021
#define CDFr_BAD_ARGUMENT                    -2022
#define CDFr_IBM_PC_OVERFLOW                 -2023
#define CDFr_TOO_MANY_VARS                   -2024
#define CDFr_VAR_EXISTS                      -2025
#define CDFr_BAD_MALLOC                      -2026
#define CDFr_NOT_A_CDF                       -2027
#define CDFr_CORRUPTED_V2_CDF                -2028
#define CDFr_VAR_OPEN_ERROR                  -2029
#define CDFr_BAD_INITIAL_RECS                -2030
#define CDFr_BAD_BLOCKING_FACTOR             -2031
#define CDFr_END_OF_VAR                      -2032
                    /* -2033 unused. */
#define CDFr_BAD_CDFSTATUS                   -2034
#define CDFr_INTERNAL_ERROR                  -2035
#define CDFr_BAD_NUM_VARS                    -2036
#define CDFr_BAD_REC_COUNT                   -2037
#define CDFr_BAD_REC_INTERVAL                -2038
#define CDFr_BAD_DIM_COUNT                   -2039
#define CDFr_BAD_DIM_INTERVAL                -2040
#define CDFr_BAD_VAR_NUM                     -2041
#define CDFr_BAD_ATTR_NUM                    -2042
#define CDFr_BAD_ENTRY_NUM                   -2043
#define CDFr_BAD_ATTR_NAME                   -2044
#define CDFr_BAD_VAR_NAME                    -2045
#define CDFr_NO_ATTR_SELECTED                -2046
#define CDFr_NO_ENTRY_SELECTED               -2047
#define CDFr_NO_VAR_SELECTED                 -2048
#define CDFr_BAD_CDF_NAME                    -2049
                    /* -2050 unused. */
#define CDFr_CANNOT_CHANGE                   -2051
#define CDFr_NO_STATUS_SELECTED              -2052
#define CDFr_NO_CDF_SELECTED                 -2053
#define CDFr_READ_ONLY_DISTRIBUTION          -2054
#define CDFr_CLOSE_ERROR                     -2055
#define CDFr_VAR_CLOSE_ERROR                 -2056
                    /* -2057 unused. */
#define CDFr_BAD_FNC_OR_ITEM                 -2058
                    /* -2059 unused. */
#define CDFr_ILLEGAL_ON_V1_CDF               -2060
                    /* -2061 unused. */
                    /* -2062 unused. */
#define CDFr_BAD_CACHE_SIZE                  -2063
                    /* -2064 unused. */
                    /* -2065 unused. */
#define CDFr_CREATE_ERROR                    -2066
#define CDFr_NO_SUCH_CDF                     -2067
#define CDFr_VAR_CREATE_ERROR                -2068
                    /* -2069 unused. */
#define CDFr_READ_ONLY_MODE                  -2070
#define CDFr_ILLEGAL_IN_zMODE                -2071
#define CDFr_BAD_zMODE                       -2072
#define CDFr_BAD_READONLY_MODE               -2073
#define CDFr_READ_ERROR                      -2074
#define CDFr_WRITE_ERROR                     -2075
#define CDFr_ILLEGAL_FOR_SCOPE               -2076
#define CDFr_NO_MORE_ACCESS                  -2077
                    /* -2078 unused. */
#define CDFr_BAD_DECODING                    -2079
                    /* -2080 unused. */
#define CDFr_BAD_NEGtoPOSfp0_MODE            -2081
#define CDFr_UNSUPPORTED_OPERATION           -2082
#define CDFr_SAVE_ERROR                      -2083
#define CDFr_VAR_SAVE_ERROR                  -2084
                    /* -2085 unused. */
#define CDFr_NO_WRITE_ACCESS                 -2086
#define CDFr_NO_DELETE_ACCESS                -2087
#define CDFr_DELETE_ERROR                    -2088
#define CDFr_VAR_DELETE_ERROR                -2089
#define CDFr_UNKNOWN_COMPRESSION             -2090
#define CDFr_CANNOT_COMPRESS                 -2091
#define CDFr_DECOMPRESSION_ERROR             -2092
#define CDFr_COMPRESSION_ERROR               -2093
                    /* -2094 unused. */
                    /* -2095 unused. */
#define CDFr_EMPTY_COMPRESSED_CDF            -2096
#define CDFr_BAD_COMPRESSION_PARM            -2097
#define CDFr_UNKNOWN_SPARSENESS              -2098
#define CDFr_CANNOT_SPARSERECORDS            -2099
#define CDFr_CANNOT_SPARSEARRAYS             -2100
#define CDFr_TOO_MANY_PARMS                  -2101
#define CDFr_NO_SUCH_RECORD                  -2102
#define CDFr_CANNOT_ALLOCATE_RECORDS         -2103
                    /* -2104 unused. */
                    /* -2105 unused. */
#define CDFr_SCRATCH_DELETE_ERROR            -2106
#define CDFr_SCRATCH_CREATE_ERROR            -2107
#define CDFr_SCRATCH_READ_ERROR              -2108
#define CDFr_SCRATCH_WRITE_ERROR             -2109
#define CDFr_BAD_SPARSEARRAYS_PARM           -2110
#define CDFr_BAD_SCRATCH_DIR                 -2111
#define CDFr_NOT_A_CDF_OR_NOT_SUPPORTED      -2113
#define CDFr_CORRUPTED_V3_CDF                -2223
#define CDFr_ILLEGAL_EPOCH_FIELD             -2224
#define CDFr_BAD_CHECKSUM                    -2225
#define CDFr_CHECKSUM_ERROR                  -2226
#define CDFr_CHECKSUM_NOT_ALLOWED            -2227
#define CDFr_IS_A_NETCDF                     -2228
#define CDFr_TT2000_TIME_ERROR               -2229
#define CDFr_UNABLE_TO_PROCESS_CDF           -2230

#endif // CDFERRORDEFS_H
