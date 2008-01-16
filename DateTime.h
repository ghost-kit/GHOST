#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <algorithm>
#include <cassert>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

static double EPSILON = 1e-8;
/// Days per month
/**  0   1   2   3   4   5   6   7   8   9  10  11  12
 *
 *  dec jan feb mar apr may jun jul aug sep oct nov dec
 */
static int DAYS_PER_MONTH[2][13] = {
  {31,31,28,31,30,31,30,31,31,30,31,30,31},
  {31,31,29,31,30,31,30,31,31,30,31,30,31}  
};
static double epochJulianDate = 2400001; // mjd = jd - 2400001
static size_t epochYear = 1858;
static size_t epochMonth = 11;
static size_t epochDay = 17;
static size_t epochHours = 0;
static size_t epochMinutes = 0;
static double epochSeconds = 0;


/// Date and Time managment
/**
 *  Class to handle Date and Time in two formats:
 *
 *   1.   YYYY/MM/DD HH:MM:SS
 *
 *   2.   Modified Julian Date
 *
 * The Julian day or Julian day number (JDN) is the integer number of
 * days that have elapsed since the initial epoch defined as noon
 * Universal Time (UT) Monday, January 1, 4713 BC in the proleptic
 * Julian calendar.  That noon-to-noon day is counted as Julian day 0.
 *
 * The Modified Julian Day (MJD) is the number of days (with decimal
 * fraction of the day) that have elapsed since midnight at the
 * beginning of Wednesday November 17, 1858.  In terms of the Julian
 * day:
 *
 *  \f[
 *     MJD = JD - 2400000.5
 *  \f]
 *
 * @author Peter Schmitt
 * @version January 3, 2008
 */
class DateTime
{
public:
  // Constructors:    
  DateTime(void);
  DateTime(const size_t &YEAR, const size_t &MONTH, const size_t &DAY, 
	   const size_t &HOURS, const size_t &MINUTES, const double &SECONDS);
  DateTime(const double & MJD);

  // Set Member Data:
  /// Set modified julian date and update YYYY-MM-DD @ HH:MM:SS
  void setMJD(const double &MJD) { mjd = MJD; updateYMDHMS(); }
  /// Set year and update modified julian date
  void setYear(const size_t &YEAR) { year = YEAR; updateMJD(); }
  /// Set month and update modified julian date
  void setMonth(const size_t &MONTH) { month = MONTH; updateMJD(); }
  /// Set day and update modified julian date
  void setDay(const size_t &DAY) { day = DAY; updateMJD(); }
  /// Set hours and update modified julian date
  void setHours(const size_t &HOURS) { hours = HOURS; updateMJD(); }
  /// Set minutes and update modified julian date
  void setMinutes(const size_t &MINUTES) { minutes = MINUTES; updateMJD(); }
  /// Set seconds and update modified julian date
  void setSeconds(const double &SECONDS) { seconds = SECONDS; updateMJD(); }

  // Modify Member Data:
  /// Increment the current MJD by delta_MJD and update YYYY-MM-DD @ HH:MM:SS
  void incrementMJD(const double &delta_MJD) {mjd += delta_MJD; updateYMDHMS(); }
  /// Increment the current year by delta_YEAR and update modified julian date
  void incrementYear(const size_t &delta_YEAR) { year += delta_YEAR; updateMJD(); }
  /// Increment the current month by delta_MONTH and update modified julian date
  void incrementMonth(const size_t &delta_MONTH) { month += delta_MONTH; updateMJD(); }
  /// Increment the current day by delta_DAY and update modified julian date
  void incrementDay(const size_t &delta_DAY) { day += delta_DAY; updateMJD(); }
  /// Increment the current hours by delta_HOURS and update modified julian date
  void incrementHours(const size_t &delta_HOURS) { hours += delta_HOURS; updateMJD(); }
  /// Increment the current minutes by delta_MINUTES and update modified julian date
  void incrementMinutes(const size_t &delta_MINUTES) { minutes += delta_MINUTES; updateMJD(); }
  /// Increment the current seconds by delta_SECONDS and update modified julian date
  void incrementSeconds(const double &delta_SECONDS) { seconds += delta_SECONDS; updateMJD(); }

  // Access Member Data:
  /// Get the current modified julian date
  double getMJD(void) { return mjd; }
  /// Get the current year
  size_t getYear(void) { return year; }
  /// Get the current month
  size_t getMonth(void) { return month; }
  /// Get the current day
  size_t getDay(void) { return day; }
  /// Get the current hours
  size_t getHours(void) { return hours; }
  /// Get the current minutes
  size_t getMinutes(void) { return minutes; }
  /// Get the current seconds
  double getSeconds(void) { return seconds; }  

  /// Get the number of days elapsed since Epoch date
  double getDaySinceEpoch(void) { return mjd; }
  /// Get the number of hours elapsed since Epoch date
  double getHoursSinceEpoch(void) { return 24*mjd; }
  /// Get the number of minutes elapsed since Epoch date
  double getMinutesSinceEpoch(void) { return 1440*mjd; } // 1440 = 24*60
  /// Get the number of seconds elapsed since Epoch date
  double getSecondsSinceEpoch(void) { return 86400*mjd; } // 86400 = 24*60*60

  /// Get the day of the year
  size_t getDayOfYear(void) { return dayOfYear(); }

  // Output:
  friend std::ostream& operator << (std::ostream& output, const DateTime& time);
  std::string getDateTimeString(void);

  // Comparison
  bool operator < (const DateTime & date);
  bool operator <= (const DateTime & date);
  bool operator > (const DateTime & date);
  bool operator >= (const DateTime & date);
  bool operator == (const DateTime & date);

  // Addition & Subtraction
  friend DateTime operator + (const DateTime &date1, const DateTime & date2);
  void operator += (const DateTime & date);
  friend DateTime operator - (const DateTime & date1, const DateTime & date2);
  void operator -= (const DateTime & date);
 
private:  
  size_t dayOfYear(void);
  double secOfDay(void);

  void updateMJD(void);
  void updateYMDHMS(void);
  
  void verifyYMDHMS(void);

  /// Modified Julian Date
  double mjd; 

  /// Year corresponding to mjd
  size_t year;
  /// Month of yearcorresponding to mjd
  size_t month;
  /// Day of month correspdonging to mjd
  size_t day;
  /// Hours of day corresponding to mjd
  size_t hours;
  /// Minutes of hour corresponding to mjd
  size_t minutes;
  /// Seconds of minute corresponding to mjd
  double seconds;
};

#endif
