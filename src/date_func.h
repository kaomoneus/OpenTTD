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
extern uint32 _tick_counter;

enum struct StandardTimeUnits {
	MINUTES, HOURS, DAYS, length
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
 * Returns ticks or particular time unit in game (non-vanilla) scale
 * @param time_unit
 * @return
 */
constexpr Ticks GetStandardTimeUnitTicks(StandardTimeUnits time_unit) {
	constexpr Ticks ticks[] {DAY_TICKS / 24 / 60, DAY_TICKS / 24, DAY_TICKS};
	return ticks[(int)time_unit];
};


std::tuple<Date, DateFract> GameDateToVanillaDate(Date d, DateFract fract = 0);
std::tuple<Date, DateFract> VanillaDateToGameDate(Date d, DateFract fract = 0);

/**
 * For given date fract returns hours and minutes reminder
 * @param date_fract - source date fracture
 * @return tuple with two items: hour, minute
 */
std::tuple<uint8, uint8> GetHoursAndMinutes(DateFract date_fract);

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
