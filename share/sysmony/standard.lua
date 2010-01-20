--[[
Copyright (c) 2010, Albertas Vyšniauskas
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]--

require('helpers')
require('datetimefnc')

update.time = {
	label_time = nil,
	label_date = nil,
	callback = function (self)
		self.label_time:text(os.date('%H:%M'))
		self.label_date:text(os.date('%x'))
	end,
}

update.uptime = {
	label_uptime = nil,
	callback = function (self)
		si = sysinfo:get()
		self.label_uptime:text(format_time(si.uptime))
	end,
}

update.kernel = {
	label_kernel = nil,
	callback = function (self)
		na = uname:get()
		self.label_kernel:text(na.release)
	end,
}

update.cpuinfo = {
	label_cpuinfo = nil,
	
	callback = function (self)

		local f = assert(io.open("/proc/cpuinfo", "r"))

		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			if string.find(line, '^cpu MHz') then
				local f1, f2 = string.find(line, ': ')
				local value = string.sub(line, f2)
				os.setlocale("C", "numeric")
				self.label_cpuinfo:text(string.format("%01.2f GHz", tonumber(value)/1000))
				os.setlocale(nil, "numeric")	
			end
		end		

		f:close()
	end,
}

update.net = {
	label_upload = nil,
	label_download = nil,

	label_upload_speed = nil,
	label_download_speed = nil,
	
	graph_upload_speed = nil,
	graph_download_speed = nil,
	
	prev_download = nil,
	prev_upload = nil,
	prev_download_lp = 0,
	prev_upload_lp = 0,

	interface = "eth0",
	
	callback = function (self)

		local f = assert(io.open("/proc/net/dev", "r"))
		
		f:read("*line")
		f:read("*line")
		
		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			local interface, download, upload = string.match(line, "([%a%d]+):%s*(%d+)%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+(%d+)%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+")
			if interface == self.interface then
				if self.label_upload then self.label_upload:text(format_size(tonumber(upload))) end
				if self.label_download then self.label_download:text(format_size(tonumber(download))) end

                if self.prev_upload then
					self.prev_upload_lp = self.prev_upload_lp + 0.3 * (tonumber(upload) - self.prev_upload - self.prev_upload_lp)
                
					if self.graph_upload_speed then                	
						self.graph_upload_speed:push_value(self.prev_upload_lp / 1024)
					end
				end
				if self.prev_download then
					self.prev_download_lp = self.prev_download_lp + 0.3 * (tonumber(download) - self.prev_download - self.prev_download_lp)
					
					if self.graph_download_speed then
						self.graph_download_speed:push_value(self.prev_download_lp / 1024)
					end				
				end
				
				if self.label_upload_speed then self.label_upload_speed:text(format_speed(tonumber(self.prev_upload_lp), 's')) end
				if self.label_download_speed then self.label_download_speed:text(format_speed(tonumber(self.prev_download_lp), 's')) end

				self.prev_upload = tonumber(upload)
				self.prev_download = tonumber(download)
			end
		end		

		f:close()
	end,
}

update.meminfo = {
	label_total_memory = nil,
	label_free_memory = nil,
	label_free_memory_percent = nil,
	
	callback = function (self)
		local values = {}
		
		local f = assert(io.open("/proc/meminfo", "r"))
		
		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			local name, value = string.match(line, "([%a%d\(\)]+):%s*(%d+)")
			values[name] = value
		end

		f:close()
		
		if self.label_total_memory then
			self.label_total_memory:text(format_size(values['MemTotal']*1024))
		end
		if self.label_free_memory then
			self.label_free_memory:text(format_size((values['Buffers'] + values['Cached'] + values['MemFree'])*1024))
		end
		if self.label_free_memory_percent then
			self.label_free_memory_percent:text(format_percentage((values['Buffers'] + values['Cached'] + values['MemFree'])/values['MemTotal']))
		end
	end,
}

update.calendar = {
	day_widgets = {},
	last_update = nil,

	wrap_week = function (day)
		if day < 1 then
			return day + 7
		elseif day > 7 then
			return day - 7
		else
			return day
		end
	end,
	
	build_calendar = function (self, calendar_style)

		local t = sm_table:new(4, 0, true, false)
		t:padding(30, 0)
		
		local text_color = calendar_style:text_color()
		
		local current_day_style = renderstyle:new({text_color = text_color,
			apply_mask = 2,
			font_weight = 900,
		})
		
		text_color:alpha(0.6)
		
		local day_style = renderstyle:new({text_color = text_color,
			apply_mask = 1,
		})
		
		text_color:alpha(0.3)
		
		local header_style = renderstyle:new({text_color = text_color,
			font_size = 7,
		})
		
		local first_weekday = datetime:get_first_weekday()
		local days = datetime:get_weekday_names()
		
		local l

		self.day_widgets[1] = {}
		
		for i=0,6 do 
			l = label:new(days[self.wrap_week(i+first_weekday)] .. '')
			t:addwidget(right(l), i, 0, i+1, 1, sm_table.Fill + sm_table.Expand, sm_table.Fill, 0)
			l:add_style(header_style)
			self.day_widgets[1][i+1] = l
		end
		
		for j=1,6 do 
			self.day_widgets[j+1] = {}
			for i=0,6 do
				l = label:new('')
				t:addwidget(right(l), i, j, i+1, j+1, sm_table.Fill, sm_table.Fill, 0)
				l:add_style(current_day_style)
				l:add_style(day_style)
				self.day_widgets[j+1][i+1] = l
			end
		end
		
		self.last_update = os.date("*t")
		self.last_update.year = 0
		
		return t
	end,
	
	callback = function (self)

		local now = os.date("*t");
		
		if (self.last_update.year ~= now.year) or (self.last_update.month ~= now.month) or (self.last_update.day ~= now.day) then
			
			local first_weekday = datetime:get_first_weekday()
			local total_days = get_days_in_month(now.year, now.month)
			local empty = self.wrap_week(get_day_of_week(now.year, now.month, 1) - first_weekday + 1)
			local day = 1
			
			local l
			
			for j=1,6 do 
				for i=0,6 do
					l = self.day_widgets[j+1][i+1]
					if (empty>1) or (day > total_days) then
						l:set_style_mask(1)
						l:text('')					
						empty = empty - 1				
					else
						l:text(day .. '')
						if day == now.day then
							l:set_style_mask(2)
						else
							l:set_style_mask(1)
						end
						day = day + 1
					end
				end
			end
			
			self.last_update = os.date("*t")
		end
			
	end,
	
}

function register_updater(root_window, object_class, seconds)
	root_window:add_updater( pollingluaupdate:new(object_class.callback, object_class, seconds*1000) )
end

