
function is_leap_year(year)
	return year % 4 == 0 and (year % 100 ~= 0 or year % 400 == 0)
end
  
function get_days_in_month(year, month)
	if month == 2 and is_leap_year(year) then
		return 29
	else
		return ({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })[month]
	end
end

function get_day_of_week(year, month, day)
	return os.date('*t',os.time{year=year,month=month,day=day})['wday']
end
