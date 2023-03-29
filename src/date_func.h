/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file date_func.h Functions related to dates. */

#ifndef DATE_FUNC_H
#define DATE_FUNC_H

#include "date_type.h"

extern Year      _cur_year;
extern Month     _cur_month;
extern Date      _date;
extern DateFract _date_fract;
extern uint64 _tick_counter;

enum struct StandardTimeUnits {
	MINUTES, HOURS, DAYS, length, VANILLA_DAY_MAX_UNITS = length
};

void SetDate(Date date, DateFract fract);
void ConvertDateToYMD(Date date, YearMonthDay *ymd);
Date ConvertYMDToDate(Year year, Month month, Day day);

/**
 * Returns maximal standard time unit which is smaller than span
 * @param span A given time span in ticks
 * @return standard time unit
 */
StandardTimeUnits GetStandardTimeUnitFor(Ticks span);

/**
 * Converts ticks to time units
 * @param ticks ticks amount to be converted
 * @param time_unit type of time unit you want to convert ticks into
 * @return time units amount
 */
int TicksToTimeUnits(Ticks ticks, StandardTimeUnits time_unit = StandardTimeUnits::VANILLA_DAY_MAX_UNITS);

/**
 * Converts time units to ticks
 * @param units  units to be converted
 * @param time_unit type of time unit you want into ticks
 * @return  ticks amount
 */
Ticks TimeUnitsToTicks(int units, StandardTimeUnits time_unit = StandardTimeUnits::VANILLA_DAY_MAX_UNITS);

/**
 * Converts game date into vanilla date.
 * First it converts date + date_fract to ticks
 * And then converted ticks to vanilla day (which has amount of
 * ticks equal to VANILLA_DAY_TICKS) and corresponding vanilla day fract
 * @param d game date to be converted
 * @param fract game date fract to be converted
 * @return vanilla date and vanilla date fract
 */
std::tuple<Date, DateFract> GameDateToVanillaDate(Date d, DateFract fract = 0);

/**
 * Converts vanilla date into game date
 * First it converts vanilla date + vanilla date fract to ticks
 * And then converted ticks game day (which has amount of
 * ticks equal to GetDayTicks result) and corresponding game day fract
 * @param d vanilla date to be converted
 * @param fract vanilla date fract to be converted
 * @return game date and game date fract
 */
std::tuple<Date, DateFract> VanillaDateToGameDate(Date d, DateFract fract = 0);


/**
 * Converts hour and minute into ticks
 */
Ticks HourMinuteToTicks(uint8 hour, uint8 minute);


/**
 * Converts ticks into hour and minute
 * @param ticks - source date fracture
 * @return tuple with two items: hour, minute
 */
std::tuple<uint8, uint8> TicksToHourMinute(Ticks ticks);

/**
 * Checks whether the given year is a leap year or not.
 * @param yr The year to check.
 * @return True if \c yr is a leap year, otherwise false.
 */
static inline bool IsLeapYear(Year yr)
{
	return yr % 4 == 0 && (yr % 100 != 0 || yr % 400 == 0);
}

#endif /* DATE_FUNC_H */
