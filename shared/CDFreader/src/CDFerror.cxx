#include "CDFerror.h"
#include <iomanip>
#include <iostream>

//==================================================================//
CDFr::CDFerrorHandler::CDFerrorHandler()
{
    //set up the error code and table
    this->errorCode = CDFr_OK;
    this->populateErrorTable();
}

//==================================================================//
CDFr::CDFerrorHandler::~CDFerrorHandler()
{
    this->errorTable.clear();
}


//==================================================================//
void CDFr::CDFerrorHandler::populateErrorTable()
{
    //these records are extracted from the CDF software and may need to be updated if CDF version changes.
    this->errorTable[1001] = QString("1001 VIRTUAL_RECORD_DATA: One or more of the records are virtual.");
    this->errorTable[0] = QString("0 CDF_OK: Function completed successfully.");
    this->errorTable[-1001] = QString("-1001 ATTR_NAME_TRUNC: Attribute name truncated.");
    this->errorTable[-1002] = QString("-1002 CDF_NAME_TRUNC: CDF pathname truncated.");
    this->errorTable[-1003] = QString("-1003 VAR_NAME_TRUNC: Variable name truncated.");
    this->errorTable[-1004] = QString("-1004 NEGATIVE_FP_ZERO: A negative floating point zero (-0.0) was detected.");
    this->errorTable[-1006] = QString("-1006 FORCED_PARAMETER: A specified parameter was changed to a different value.");
    this->errorTable[-1007] = QString("-1007 NA_FOR_VARIABLE: Operation n/a for the type of variable.");
    this->errorTable[-2001] = QString("-2001 ATTR_EXISTS: Named attribute already exists.");
    this->errorTable[-2002] = QString("-2002 BAD_CDF_ID: CDF identifier is unknown or invalid.");
    this->errorTable[-2003] = QString("-2003 BAD_DATA_TYPE: Unknown data type specified or encountered.");
    this->errorTable[-2004] = QString("-2004 BAD_DIM_SIZE: Dimension size specified as zero or less.");
    this->errorTable[-2005] = QString("-2005 BAD_DIM_INDEX: Dimension index out of range.");
    this->errorTable[-2006] = QString("-2006 BAD_ENCODING: Unknown or unsupported data encoding.");
    this->errorTable[-2007] = QString("-2007 BAD_MAJORITY: Unknown variable majority specified.");
    this->errorTable[-2008] = QString("-2008 BAD_NUM_DIMS: Illegal number of dimensions.");
    this->errorTable[-2009] = QString("-2009 BAD_REC_NUM: Record number is out of range.");
    this->errorTable[-2010] = QString("-2010 BAD_SCOPE: Unrecognized attribute scope.");
    this->errorTable[-2011] = QString("-2011 BAD_NUM_ELEMS: Illegal number of elements (for data type).");
    this->errorTable[-2012] = QString("-2012 CDF_OPEN_ERROR: Open failed - error from file system.");
    this->errorTable[-2013] = QString("-2013 CDF_EXISTS: The CDF named already exists.");
    this->errorTable[-2014] = QString("-2014 BAD_FORMAT: Unknown CDF format specified.");
    this->errorTable[-2015] = QString("-2015 BAD_ALLOCATE_RECS: Illegal number of records to allocate specified.");
    this->errorTable[-2016] = QString("-2016 BAD_CDF_EXTENSION: Illegal/missing extension for multi-file CDF.");
    this->errorTable[-2017] = QString("-2017 NO_SUCH_ATTR: Named attribute not found in this CDF.");
    this->errorTable[-2018] = QString("-2018 NO_SUCH_ENTRY: No such entry for specified attribute.");
    this->errorTable[-2019] = QString("-2019 NO_SUCH_VAR: Named variable not found in this CDF.");
    this->errorTable[-2020] = QString("-2020 VAR_READ_ERROR: Read failed - error from file system.");
    this->errorTable[-2021] = QString("-2021 VAR_WRITE_ERROR: Write failed - error from file system.");
    this->errorTable[-2022] = QString("-2022 BAD_ARGUMENT: Illegal/undefined argument specified.");
    this->errorTable[-2023] = QString("-2023 PC_OVERFLOW: Greater than 64Kb of memory required.");
    this->errorTable[-2024] = QString("-2024 TOO_MANY_VARS: Only a limited number of variables may exist in this CDF.");
    this->errorTable[-2025] = QString("-2025 VAR_EXISTS: Named variable already exists.");
    this->errorTable[-2026] = QString("-2026 BAD_MALLOC: Unable to allocate dynamic memory.");
    this->errorTable[-2027] = QString("-2027 NOT_A_CDF: Named CDF is corrupted or not actually a CDF.");
    this->errorTable[-2028] = QString("-2028 CORRUPTED_V2_CDF: Version 2 CDF is corrupted.");
    this->errorTable[-2029] = QString("-2029 VAR_OPEN_ERROR: Open failed - error from file system.");
    this->errorTable[-2030] = QString("-2030 BAD_INITIAL_RECS: Illegal number of initial records.");
    this->errorTable[-2031] = QString("-2031 BAD_BLOCKING_FACTOR: Illegal blocking factor specified (less than zero).");
    this->errorTable[-2032] = QString("-2032 END_OF_VAR: Current position is at the end of the variable.");
    this->errorTable[-2034] = QString("-2034 BAD_CDFSTATUS: Unknown CDF status code specified.");
    this->errorTable[-2035] = QString("-2035 CDF_INTERNAL_ERROR: An internal error has occurred in the CDF library.");
    this->errorTable[-2036] = QString("-2036 BAD_NUM_VARS: Illegal number of variables specified.");
    this->errorTable[-2037] = QString("-2037 BAD_REC_COUNT: Illegal record count specified.");
    this->errorTable[-2038] = QString("-2038 BAD_REC_INTERVAL: Illegal record interval specified.");
    this->errorTable[-2039] = QString("-2039 BAD_DIM_COUNT: Illegal dimension count specified.");
    this->errorTable[-2040] = QString("-2040 BAD_DIM_INTERVAL: Illegal dimension interval specified.");
    this->errorTable[-2041] = QString("-2041 BAD_VAR_NUM: Illegal variable number specified.");
    this->errorTable[-2042] = QString("-2042 BAD_ATTR_NUM: Illegal attribute number specified.");
    this->errorTable[-2043] = QString("-2043 BAD_ENTRY_NUM: Illegal attribute entry number specified.");
    this->errorTable[-2044] = QString("-2044 BAD_ATTR_NAME: Illegal attribute name specified.");
    this->errorTable[-2045] = QString("-2045 BAD_VAR_NAME: Illegal variable name specified.");
    this->errorTable[-2046] = QString("-2046 NO_ATTR_SELECTED: An attribute has not been selected.");
    this->errorTable[-2047] = QString("-2047 NO_ENTRY_SELECTED: An attribute entry has not been selected.");
    this->errorTable[-2048] = QString("-2048 NO_VAR_SELECTED: A variable has not been selected.");
    this->errorTable[-2049] = QString("-2049 BAD_CDF_NAME: Illegal CDF name specified.");
    this->errorTable[-2051] = QString("-2051 CANNOT_CHANGE: The parameter/value can't be set/changed.");
    this->errorTable[-2052] = QString("-2052 NO_STATUS_SELECTED: A CDF status code has not been selected.");
    this->errorTable[-2053] = QString("-2053 NO_CDF_SELECTED: A CDF has not been selected.");
    this->errorTable[-2054] = QString("-2054 READ_ONLY_DISTRIBUTION: Writing/deleting is illegal.");
    this->errorTable[-2055] = QString("-2055 CDF_CLOSE_ERROR: Close failed - error from file system.");
    this->errorTable[-2056] = QString("-2056 VAR_CLOSE_ERROR: Close failed - error from file system.");
    this->errorTable[-2058] = QString("-2058 BAD_FNC_OR_ITEM: The specified function or item is illegal.");
    this->errorTable[-2060] = QString("-2060 ILLEGAL_ON_V1_CDF: Operation not allowed on Version 1 CDFs.");
    this->errorTable[-2063] = QString("-2063 BAD_CACHE_SIZE: Illegal number of cache buffers specified.");
    this->errorTable[-2066] = QString("-2066 CDF_CREATE_ERROR: Creation failed - error from file system.");
    this->errorTable[-2067] = QString("-2067 NO_SUCH_CDF: The specified CDF does not exist.");
    this->errorTable[-2068] = QString("-2068 VAR_CREATE_ERROR: Creation failed - error from file system.");
    this->errorTable[-2070] = QString("-2070 READ_ONLY_MODE: CDF is in read-only mode.");
    this->errorTable[-2071] = QString("-2071 ILLEGAL_IN_zMODE: Operation is illegal while in zMode.");
    this->errorTable[-2072] = QString("-2072 BAD_zMODE: Illegal zMode specified.");
    this->errorTable[-2073] = QString("-2073 BAD_READONLY_MODE: Illegal read-only mode specified.");
    this->errorTable[-2074] = QString("-2074 CDF_READ_ERROR: Read failed - error from file system.");
    this->errorTable[-2075] = QString("-2075 CDF_WRITE_ERROR: Write failed - error from file system.");
    this->errorTable[-2076] = QString("-2076 ILLEGAL_FOR_SCOPE: The operation is illegal for the attribute's scope.");
    this->errorTable[-2077] = QString("-2077 NO_MORE_ACCESS: No more access to the CDF due to a severe error.");
    this->errorTable[-2079] = QString("-2079 BAD_DECODING: Unknown or unsupported data decoding specified.");
    this->errorTable[-2081] = QString("-2081 BAD_NEGtoPOSfp0_MODE: Illegal negative to positive floating point zero mode.");
    this->errorTable[-2082] = QString("-2082 UNSUPPORTED_OPERATION: The attempted operation isn't supported at this time.");
    this->errorTable[-2086] = QString("-2086 NO_WRITE_ACCESS: Write access is not allowed on the CDF file(s).");
    this->errorTable[-2087] = QString("-2087 NO_DELETE_ACCESS: Deleting is not allowed (read only).");
    this->errorTable[-2088] = QString("-2088 CDF_DELETE_ERROR: Deletion failed - error from file system.");
    this->errorTable[-2089] = QString("-2089 VAR_DELETE_ERROR: Deletion failed - error from file system.");
    this->errorTable[-2090] = QString("-2090 UNKNOWN_COMPRESSION: An unknown type of compression was encountered.");
    this->errorTable[-2091] = QString("-2091 CANNOT_COMPRESS: The compression for the CDF/variable can't be set/modified.");
    this->errorTable[-2092] = QString("-2092 DECOMPRESSION_ERROR: An error was detected in the compressed data.");
    this->errorTable[-2096] = QString("-2096 EMPTY_COMPRESSED_CDF: No internal records in a compressed CDF.");
    this->errorTable[-2097] = QString("-2097 BAD_COMPRESSION_PARM: An illegal compression parameter was specified.");
    this->errorTable[-2098] = QString("-2098 UNKNOWN_SPARSENESS: An unknown sparseness was encountered.");
    this->errorTable[-2099] = QString("-2099 CANNOT_SPARSERECORDS: Sparse records can't be set/modified for the variable.");
    this->errorTable[-2100] = QString("-2100 CANNOT_SPARSEARRAYS: Sparse arrays can't be set/modified for the variable.");
    this->errorTable[-2101] = QString("-2101 TOO_MANY_PARMS: Too many parameters were encountered.");
    this->errorTable[-2102] = QString("-2102 NO_SUCH_RECORD: The specified record does not exist.");
    this->errorTable[-2103] = QString("-2103 CANNOT_ALLOCATE_RECORDS: Records can't be allocated at this time.");
    this->errorTable[-2106] = QString("-2106 SCRATCH_DELETE_ERROR: Scratch file deletion failed - error from file system.");
    this->errorTable[-2107] = QString("-2107 SCRATCH_CREATE_ERROR: Scratch file creation failed - error from file system.");
    this->errorTable[-2108] = QString("-2108 SCRATCH_READ_ERROR: Scratch file read failed - error from file system.");
    this->errorTable[-2109] = QString("-2109 SCRATCH_WRITE_ERROR: Scratch file write failed - error from file system.");
    this->errorTable[-2110] = QString("-2110 BAD_SPARSEARRAYS_PARM: An illegal sparse arrays parameter was specified.");
    this->errorTable[-2111] = QString("-2111 BAD_SCRATCH_DIR: An illegal scratch directory was specified.");
    this->errorTable[-2113] = QString("-2113 NOT_A_CDF_OR_NOT_SUPPORTED: Named CDF is corrupted or not supported by the current library version.");
    this->errorTable[-2223] = QString("-2223 CORRUPTED_V3_CDF: Version 3 CDF is corrupted.");
    this->errorTable[-2224] = QString("-2224 ILLEGAL_EPOCH_FIELD: One or more of the date/time fields is out of valid range.");
    this->errorTable[-2225] = QString("-2225 BAD_CHECKSUM: The specified checksum method is not currently supported.");
    this->errorTable[-2226] = QString("-2226 CHECKSUM_ERROR: The data integrity verification through the checksum failed.");
    this->errorTable[-2227] = QString("-2227 CHECKSUM_NOT_ALLOWED: The checksum is not allowed for old versioned files.");
    this->errorTable[-2228] = QString("-2228 IS_A_NETCDF: Named CDF file is actually a netCDF file.");
    this->errorTable[-2229] = QString("-2229 TT2000_TIME_ERROR: Error handling the date/time for TT2000 or a TT2000 epoch.");


    //custom error messages
    this->errorTable[-4000] = QString("-4000 NO_PARENT_DEFINED: No Parent has been defined for Attribute.");
}


//==================================================================//
bool CDFr::CDFerrorHandler::hasError()
{
    //this is pretty simple.  If the code is not NO_ERROR, the there is an error
    if(this->errorCode != CDFr_OK) return true;
    else return false;
}

//==================================================================//
QString CDFr::CDFerrorHandler::getErrorString()
{

    if(this->errorTable.contains(errorCode))
    {
        return this->errorTable[errorCode];
    }
    else
    {
        //this will be the case if we didn't put the error code in the lookup table
        return QString("INVALID ERROR CODE - CODE NOT FOUND IN LOOKUP TABLE");
    }
}

//==================================================================//
long CDFr::CDFerrorHandler::getError()
{

    return this->errorCode;
}

//==================================================================//
void CDFr::CDFerrorHandler::setError(long errorCode)
{
    this->errorCode = errorCode;
}
