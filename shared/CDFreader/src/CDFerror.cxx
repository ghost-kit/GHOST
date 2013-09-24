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
    this->errorTable[1001]  = QString("One or more of the records are virtual.");
    this->errorTable[0]     = QString("Function completed successfully.");
    this->errorTable[-1001] = QString("Attribute name truncated.");
    this->errorTable[-1002] = QString("CDF pathname truncated.");
    this->errorTable[-1003] = QString("Variable name truncated.");
    this->errorTable[-1004] = QString("A negative floating point zero (-0.0) was detected.");
    this->errorTable[-1006] = QString("A specified parameter was changed to a different value.");
    this->errorTable[-1007] = QString("Operation n/a for the type of variable.");
    this->errorTable[-2001] = QString("Named attribute already exists.");
    this->errorTable[-2002] = QString("CDF identifier is unknown or invalid.");
    this->errorTable[-2003] = QString("Unknown data type specified or encountered.");
    this->errorTable[-2004] = QString("Dimension size specified as zero or less.");
    this->errorTable[-2005] = QString("Dimension index out of range.");
    this->errorTable[-2006] = QString("Unknown or unsupported data encoding.");
    this->errorTable[-2007] = QString("Unknown variable majority specified.");
    this->errorTable[-2008] = QString("Illegal number of dimensions.");
    this->errorTable[-2009] = QString("Record number is out of range.");
    this->errorTable[-2010] = QString("Unrecognized attribute scope.");
    this->errorTable[-2011] = QString("Illegal number of elements (for data type).");
    this->errorTable[-2012] = QString("Open failed - error from file system.");
    this->errorTable[-2013] = QString("The CDF named already exists.");
    this->errorTable[-2014] = QString("Unknown CDF format specified.");
    this->errorTable[-2015] = QString("Illegal number of records to allocate specified.");
    this->errorTable[-2016] = QString("Illegal/missing extension for multi-file CDF.");
    this->errorTable[-2017] = QString("Named attribute not found in this CDF.");
    this->errorTable[-2018] = QString("No such entry for specified attribute.");
    this->errorTable[-2019] = QString("Named variable not found in this CDF.");
    this->errorTable[-2020] = QString("Read failed - error from file system.");
    this->errorTable[-2021] = QString("Write failed - error from file system.");
    this->errorTable[-2022] = QString("Illegal/undefined argument specified.");
    this->errorTable[-2023] = QString("Greater than 64Kb of memory required.");
    this->errorTable[-2024] = QString("Only a limited number of variables may exist in this CDF.");
    this->errorTable[-2025] = QString("Named variable already exists.");
    this->errorTable[-2026] = QString("Unable to allocate dynamic memory.");
    this->errorTable[-2027] = QString("Named CDF is corrupted or not actually a CDF.");
    this->errorTable[-2028] = QString("Version 2 CDF is corrupted.");
    this->errorTable[-2029] = QString("Open failed - error from file system.");
    this->errorTable[-2030] = QString("Illegal number of initial records.");
    this->errorTable[-2031] = QString("Illegal blocking factor specified (less than zero).");
    this->errorTable[-2032] = QString("Current position is at the end of the variable.");
    this->errorTable[-2034] = QString("Unknown CDF status code specified.");
    this->errorTable[-2035] = QString("An internal error has occurred in the CDF library.");
    this->errorTable[-2036] = QString("Illegal number of variables specified.");
    this->errorTable[-2037] = QString("Illegal record count specified.");
    this->errorTable[-2038] = QString("Illegal record interval specified.");
    this->errorTable[-2039] = QString("Illegal dimension count specified.");
    this->errorTable[-2040] = QString("Illegal dimension interval specified.");
    this->errorTable[-2041] = QString("Illegal variable number specified.");
    this->errorTable[-2042] = QString("Illegal attribute number specified.");
    this->errorTable[-2043] = QString("Illegal attribute entry number specified.");
    this->errorTable[-2044] = QString("Illegal attribute name specified.");
    this->errorTable[-2045] = QString("Illegal variable name specified.");
    this->errorTable[-2046] = QString("An attribute has not been selected.");
    this->errorTable[-2047] = QString("An attribute entry has not been selected.");
    this->errorTable[-2048] = QString("A variable has not been selected.");
    this->errorTable[-2049] = QString("Illegal CDF name specified.");
    this->errorTable[-2051] = QString("The parameter/value can't be set/changed.");
    this->errorTable[-2052] = QString("A CDF status code has not been selected.");
    this->errorTable[-2053] = QString("A CDF has not been selected.");
    this->errorTable[-2054] = QString("Writing/deleting is illegal.");
    this->errorTable[-2055] = QString("Close failed - error from file system.");
    this->errorTable[-2056] = QString("Close failed - error from file system.");
    this->errorTable[-2058] = QString("The specified function or item is illegal.");
    this->errorTable[-2060] = QString("Operation not allowed on Version 1 CDFs.");
    this->errorTable[-2063] = QString("Illegal number of cache buffers specified.");
    this->errorTable[-2066] = QString("Creation failed - error from file system.");
    this->errorTable[-2067] = QString("The specified CDF does not exist.");
    this->errorTable[-2068] = QString("Creation failed - error from file system.");
    this->errorTable[-2070] = QString("CDF is in read-only mode.");
    this->errorTable[-2071] = QString("Operation is illegal while in zMode.");
    this->errorTable[-2072] = QString("Illegal zMode specified.");
    this->errorTable[-2073] = QString("Illegal read-only mode specified.");
    this->errorTable[-2074] = QString("Read failed - error from file system.");
    this->errorTable[-2075] = QString("Write failed - error from file system.");
    this->errorTable[-2076] = QString("The operation is illegal for the attribute's scope.");
    this->errorTable[-2077] = QString("No more access to the CDF due to a severe error.");
    this->errorTable[-2079] = QString("Unknown or unsupported data decoding specified.");
    this->errorTable[-2081] = QString("Illegal negative to positive floating point zero mode.");
    this->errorTable[-2082] = QString("The attempted operation isn't supported at this time.");
    this->errorTable[-2086] = QString("Write access is not allowed on the CDF file(s).");
    this->errorTable[-2087] = QString("Deleting is not allowed (read only).");
    this->errorTable[-2088] = QString("Deletion failed - error from file system.");
    this->errorTable[-2089] = QString("Deletion failed - error from file system.");
    this->errorTable[-2090] = QString("An unknown type of compression was encountered.");
    this->errorTable[-2091] = QString("The compression for the CDF/variable can't be set/modified.");
    this->errorTable[-2092] = QString("An error was detected in the compressed data.");
    this->errorTable[-2096] = QString("No internal records in a compressed CDF.");
    this->errorTable[-2097] = QString("An illegal compression parameter was specified.");
    this->errorTable[-2098] = QString("An unknown sparseness was encountered.");
    this->errorTable[-2099] = QString("Sparse records can't be set/modified for the variable.");
    this->errorTable[-2100] = QString("Sparse arrays can't be set/modified for the variable.");
    this->errorTable[-2101] = QString("Too many parameters were encountered.");
    this->errorTable[-2102] = QString("The specified record does not exist.");
    this->errorTable[-2103] = QString("Records can't be allocated at this time.");
    this->errorTable[-2106] = QString("Scratch file deletion failed - error from file system.");
    this->errorTable[-2107] = QString("Scratch file creation failed - error from file system.");
    this->errorTable[-2108] = QString("Scratch file read failed - error from file system.");
    this->errorTable[-2109] = QString("Scratch file write failed - error from file system.");
    this->errorTable[-2110] = QString("An illegal sparse arrays parameter was specified.");
    this->errorTable[-2111] = QString("An illegal scratch directory was specified.");
    this->errorTable[-2113] = QString("Named CDF is corrupted or not supported by the current library version.");
    this->errorTable[-2223] = QString("Version 3 CDF is corrupted.");
    this->errorTable[-2224] = QString("One or more of the date/time fields is out of valid range.");
    this->errorTable[-2225] = QString("The specified checksum method is not currently supported.");
    this->errorTable[-2226] = QString("The data integrity verification through the checksum failed.");
    this->errorTable[-2227] = QString("The checksum is not allowed for old versioned files.");
    this->errorTable[-2228] = QString("Named CDF file is actually a netCDF file.");
    this->errorTable[-2229] = QString("Error handling the date/time for TT2000 or a TT2000 epoch.");

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
