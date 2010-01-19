--[[
Copyright (c) 2010, Albertas Vyšniauskas
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]--

sysmony = {}
update = {}

require('standard')
require('lsb_release')

sysmony.build = function (instance)

	local render_lib = instance:get_render_lib('sysmony_render_engine')

	local window1 = window:new()
	window1:resize(250, 400)
	window1:padding(10, 10)
	
	local main_style = renderstyle:new({
		text_color = color:new(0.3, 0.9, 0.1, 1.0),
		separator_color = color:new(0.3, 0.9, 0.1, 0.5),
		window_color = color:new(0.094118, 0.568627, 0.062745, 0.4),
		secondary_window_color = color:new(0.035294, 0.164706, 0.043137, 0.15),
		window_border_color = color:new(0.18, 0.98, 0.12, 1),	
		font_size = 9,
		font_weight = 400,
		})
	
	window1:add_style(main_style)
	
	local screen_size = screen:get_dimensions()
	window1:set_position(screen_size.width - 270, 20)
	
	local root_window1 = rootwindow:new(window1, render_lib)
	instance:add_root_window(root_window1)
	
	local vbox1 = vbox:new(5, false)
	window1:add_widget(vbox1)
	
	
	local time_style = renderstyle:new({
		font_weight = 600,
		font_size = 16,
	})
	
	update.time.label_time = label:new('')
	update.time.label_time:add_style(time_style)
	update.time.label_date = label:new('')
	register_updater(root_window1, update.time, 1)

	
	update.uptime.label_uptime = label:new('')
	register_updater(root_window1, update.uptime, 5)

	
	update.kernel.label_kernel = label:new('')
	register_updater(root_window1, update.kernel, 60)
	
	
	update.cpuinfo.label_cpuinfo = label:new('')
	register_updater(root_window1, update.cpuinfo, 5)
	
	
	update.net.label_upload = label:new('')
	update.net.label_download = label:new('')
	register_updater(root_window1, update.net, 3)

	
	update.meminfo.label_free_memory_percent = label:new('')
	register_updater(root_window1, update.meminfo, 10)

	
	local calendar = update.calendar:build_calendar(main_style)
	register_updater(root_window1, update.calendar, 1)
	
	
	local distributor = lsb_release.get_distributor()
	if distributor then
		distributor = distributor:lower()
	else
		distributor = 'unknown'
	end
	
	local vbox2 = vbox:new(0, false)
	
	local logo = image:new(sysmony.data_path .. '/logo/' .. distributor .. '.png')
	logo:mask(true)	
	vbox1:add_widget(center(logo), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox1:add_widget(vbox2, true, false)
	
	vbox2:add_widget(center(update.time.label_time), true, false)
	vbox2:add_widget(center(update.time.label_date), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(calendar, true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('Kernel: ', update.kernel.label_kernel), true, false)
	vbox2:add_widget(label_and_value('Uptime: ', update.uptime.label_uptime), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('CPU: ', update.cpuinfo.label_cpuinfo), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('Upload: ', update.net.label_upload), true, false)
	vbox2:add_widget(label_and_value('Download: ', update.net.label_download), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	--vbox2:add_widget(label_and_value('Total Memory: ', update.meminfo.label_total_memory), true, false)
	vbox2:add_widget(label_and_value('Free Memory: ', update.meminfo.label_free_memory_percent), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	

	
end
