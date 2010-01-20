--[[
Copyright (c) 2010, Albertas Vyšniauskas
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]--

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

format_speed = function (size, per)
	local units = {
		'B/',
		'KiB/',
		'MiB/',	
		'GiB/',
		'TiB/',
		'PiB/',	
	}
	local unit = 1
	while size>=1000 do
		size = size / 1024
		unit = unit + 1
	end
	return string.format("%01.2f ", size) .. units[unit] .. per
end




