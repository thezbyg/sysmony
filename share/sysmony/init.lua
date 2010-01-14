

require('helpers')
require('datetimefnc')

sysmony = {}
update = {}

update.time = {
	label_time = nil,
	label_date = nil,
	callback = function ()
		local this = update.time;
		this.label_time:text(os.date('%H:%M'))
		this.label_date:text(os.date('%x'))
	end,
}

update.uptime = {
	label_uptime = nil,
	callback = function ()
		si = sysinfo:get()
		local this = update.uptime;
		this.label_uptime:text(format_time(si.uptime))
	end,
}

update.kernel = {
	label_kernel = nil,
	callback = function ()
		local this = update.kernel;
		na = uname:get()
		this.label_kernel:text(na.release)
	end,
}

update.cpuinfo = {
	label_cpuinfo = nil,
	
	callback = function ()
		local this = update.cpuinfo;
		
		local f = assert(io.open("/proc/cpuinfo", "r"))

		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			if string.find(line, '^cpu MHz') then
				local f1, f2 = string.find(line, ': ')
				local value = string.sub(line, f2)
				os.setlocale("C", "numeric")
				this.label_cpuinfo:text(string.format("%01.2f GHz", tonumber(value)/1000))
				os.setlocale(nil, "numeric")	
			end
		end		

		f:close()
	end,
}

update.net = {
	label_upload = nil,
	label_download = nil,
	
	callback = function ()
		local this = update.net;
		
		local f = assert(io.open("/proc/net/dev", "r"))
		
		f:read("*line")
		f:read("*line")
		
		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			local interface, download, upload = string.match(line, "([%a%d]+):%s*(%d+)%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+(%d+)%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+")
			if interface=='eth0' then
				this.label_upload:text(format_size(tonumber(upload)))
				this.label_download:text(format_size(tonumber(download)))
			end
		end		

		f:close()
	end,
}

update.meminfo = {
	label_total_memory = nil,
	label_free_memory = nil,
	
	callback = function ()
		local this = update.meminfo;
		local values = {}
		
		local f = assert(io.open("/proc/meminfo", "r"))
		
		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			local name, value = string.match(line, "([%a%d\(\)]+):%s*(%d+)")
			values[name] = value
		end

		f:close()
		
		if this.label_total_memory then
			this.label_total_memory:text(format_size(values['MemTotal']*1024))
		end
		--this.label_free_memory:text(format_size((values['Buffers'] + values['Cached'] + values['MemFree'])*1024))	
		
		this.label_free_memory:text(format_percentage((values['Buffers'] + values['Cached'] + values['MemFree'])/values['MemTotal']))
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
	
	build_callendar = function (self)

		local t = sm_table:new(0, 0, true, false)
		t:padding(30, 0)
		
		local header_style = renderstyle:new({text_color = color:new(0.3, 0.9, 0.1, 0.3),
			font_size = 7,
		})
		
		local day_style = renderstyle:new({text_color = color:new(0.3, 0.9, 0.1, 0.6),
			apply_mask = 1,
		})
		
		local current_day_style = renderstyle:new({text_color = color:new(0.3, 0.9, 0.1, 1.0),
			apply_mask = 2,
			font_weight = 900,
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
	
	callback = function ()
		local this = update.calendar;
		
		local now = os.date("*t");
		
		if (this.last_update.year ~= now.year) or (this.last_update.month ~= now.month) or (this.last_update.day ~= now.day) then
			
			local first_weekday = datetime:get_first_weekday()
			local total_days = get_days_in_month(now.year, now.month)
			local empty = this.wrap_week(get_day_of_week(now.year, now.month, 1) - first_weekday + 1)
			local day = 1
			
			local l
			
			for j=1,6 do 
				for i=0,6 do
					l = this.day_widgets[j+1][i+1]
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
			
			this.last_update = os.date("*t")
		end
		
	end,
	
	

}


sysmony.build = function (instance)

	local render_lib = instance:get_render_lib('sysmony_render_engine')

	local window1 = window:new()
	window1:resize(250, 400)
	window1:padding(10, 10)
	window1:set_position(1600 - 270, 20);
	
	local root_window1 = rootwindow:new(window1, render_lib)
	instance:add_root_window(root_window1)
	
	
	local vbox1 = vbox:new(5, false)
	window1:add_widget(vbox1)
	
	local main_style = renderstyle:new({text_color = color:new(0.3, 0.9, 0.1, 1.0),
		separator_color = color:new(0.3, 0.9, 0.1, 0.5),
		window_color = color:new(0.094118, 0.568627, 0.062745, 0.4),
		secondary_window_color = color:new(0.035294, 0.164706, 0.043137, 0.15),
		window_border_color = color:new(0.094118, 0.568627, 0.062745, 1),	
		font_size = 9,
		font_weight = 400,
		font_name = "Sans",
		})
	
	window1:add_style(main_style)
	
	
	local time_style = renderstyle:new({
		font_name = 'Bitfont',
		font_weight = 400,
		font_size = 30,
	})
	
	
	local label_time = label:new('')
	label_time:add_style(time_style)
	
	local label_date = label:new('')
	local label_uptime = label:new('')
	local label_kernel = label:new('')
	local label_cpu_speed = label:new('')
	local label_upload = label:new('')
	local label_download = label:new('')
	local label_free_memory = label:new('')
	local label_total_memory = label:new('')
	
	local vbox2 = vbox:new(0, false)
	
	vbox1:add_widget(center(image:new(sysmony.data_path .. '/debian.png')), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox1:add_widget(vbox2, true, false)
	
	vbox2:add_widget(center(label_time), true, false)
	vbox2:add_widget(center(label_date), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(update.calendar.build_callendar(update.calendar), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('Kernel: ', label_kernel), true, false)
	vbox2:add_widget(label_and_value('Uptime: ', label_uptime), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('CPU: ', label_cpu_speed), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('Upload: ', label_upload), true, false)
	vbox2:add_widget(label_and_value('Download: ', label_download), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	--vbox2:add_widget(label_and_value('Total Memory: ', label_total_memory), true, false)
	vbox2:add_widget(label_and_value('Free Memory: ', label_free_memory), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	

	
	
	update.time.label_time = label_time
	update.time.label_date = label_date
	root_window1:add_updater( pollingluaupdate:new(update.time.callback, 1000) )
	update.time.callback()
	
	update.uptime.label_uptime = label_uptime
	root_window1:add_updater( pollingluaupdate:new(update.uptime.callback, 5*1000) )
	update.uptime.callback()
	
	update.kernel.label_kernel = label_kernel
	root_window1:add_updater( pollingluaupdate:new(update.kernel.callback, 60*1000) )
	update.kernel.callback()
	
	update.cpuinfo.label_cpuinfo = label_cpu_speed
	root_window1:add_updater( pollingluaupdate:new(update.cpuinfo.callback, 5*1000) )
	update.cpuinfo.callback()
	
	update.net.label_upload = label_upload
	update.net.label_download = label_download
	root_window1:add_updater( pollingluaupdate:new(update.net.callback, 3*1000) )
	update.net.callback()
	
	update.meminfo.label_free_memory = label_free_memory
	update.meminfo.label_total_memory = label_total_memory
	root_window1:add_updater( pollingluaupdate:new(update.meminfo.callback, 10*1000) )
	update.meminfo.callback()
	
	root_window1:add_updater( pollingluaupdate:new(update.calendar.callback, 1000) )
	update.calendar.callback()

end
