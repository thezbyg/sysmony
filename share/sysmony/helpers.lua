

center = function (widget)
	local align = alignment:new(0.5, 0)
	align:add_widget(widget)
	return align
end

right = function (widget)
	local align = alignment:new(1, 0)
	align:add_widget(widget)
	return align
end

label_and_value = function (label_text, value_label)
	
	local hbox1 = hbox:new(0, false)

	hbox1:add_widget(label:new(label_text), true, false)
	hbox1:add_widget(right(value_label), true, true)
	
	return hbox1
end

format_time = function (total_seconds)

	local days, hours, minutes, seconds
	
	seconds = total_seconds
	days = math.floor(seconds / 86400)
	seconds = seconds - days * 86400
	hours = math.floor(seconds / 3600)
	seconds = seconds - hours * 3600
	minutes = math.floor(seconds / 60)
	seconds = seconds - minutes * 60
	
	if days > 0 then
		return string.format("%.fd %.fh", days, hours)
	elseif hours > 0 then
		return string.format("%.fh %.fm", hours, minutes)
	elseif minutes > 0 then
		return string.format("%.fm %.fs", minutes, seconds)
	else
		return string.format("%.fs", seconds)
	end

	
end

format_percentage = function (percentage)
	return string.format("%.0f%%", percentage*100)
end

format_size = function (size)
	local units = {
		'B',
		'KiB',
		'MiB',	
		'GiB',
		'TiB',
		'PiB',	
	}
	local unit = 1
	while size>=1000 do
		size = size / 1024
		unit = unit + 1
	end
	return string.format("%01.2f ", size) .. units[unit]
end

