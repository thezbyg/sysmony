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
require('calendar_updater')
require('network_updater')
require('datetime_updater')
require('memory_updater')
require('uptime_updater')
require('kernel_updater')
require('cpuinfo_updater')
require('mpris_updater')

sysmony.build = function (instance)

	local render_lib = instance:get_render_lib('sysmony_render_engine')

	local window1 = window:new()
	window1:resize(250, 500)
	window1:padding(10, 10)
	
	local main_style = renderstyle:new({
		text_color = color:new(0.3, 0.9, 0.1, 1.0),
		separator_color = color:new(0.3, 0.9, 0.1, 0.5),
		window_color = color:new(0.094118, 0.568627, 0.062745, 0.4),
		secondary_window_color = color:new(0.035294, 0.164706, 0.043137, 0.15),
		window_border_color = color:new(0.18, 0.98, 0.12, 1),	
		font_name = "Liberation Sans",
		font_size = 10,
		font_weight = 400,
		})
	
	window1:add_style(main_style)
	
	local screen_size = screen:get_dimensions()
	window1:set_position(screen_size.width - 270, 20)
	
	local root_window1 = rootwindow:new(window1, render_lib)
	instance:add_root_window(root_window1)
	


    local window2 = window:new()
    window2:resize(300, 100)
	window2:padding(10, 10)
	window2:add_style(main_style)
	window2:set_position(screen_size.width - 420 - 270, 20)
    local root_window2 = rootwindow:new(window2, render_lib)
	instance:add_root_window(root_window2)

    local vbox1 = vbox:new(5, false)
    window2:add_widget(vbox1)
	
    local vbox2 = vbox:new(0, false)
    vbox1:add_widget(vbox2, true, false)

    local mpris = update.mpris:new()
	mpris.label_artist = label:new('')
	mpris.label_title = label:new('')
	mpris.label_genre = label:new('')
	mpris.label_album = label:new('')
	mpris_updater = luaupdatempris:new(instance, root_window2, mpris.callback, mpris, 'xmms2')

	mpris.container = vbox2

    vbox2:add_widget(label:new('Playing:'), false, false)
	vbox2:add_widget(mpris.label_artist, false, false)
	vbox2:add_widget(mpris.label_title, false, false)
	vbox2:add_widget(mpris.label_album, false, false)

	vbox1:add_widget(separator:new(true), true, false)

 
	local vbox1 = vbox:new(5, false)
	window1:add_widget(vbox1)
	
	
	local time_style = renderstyle:new({
		font_weight = 600,
		font_size = 16,
	})
	
	local datetime = update.datetime:new()
	datetime.label_time = label:new('')
	datetime.label_time:add_style(time_style)
	datetime.label_date = label:new('')
	register_updater(instance, root_window1, datetime, 1)

	local uptime = update.uptime:new()
	uptime.label_uptime = label:new('')
	register_updater(instance, root_window1, uptime, 5)

	local kernel = update.kernel:new()
	kernel.label_kernel = label:new('')
	register_updater(instance, root_window1, kernel, 60)
	
	local cpuinfo = update.cpuinfo:new()
	cpuinfo.label_cpuinfo = label:new('')
	register_updater(instance, root_window1, cpuinfo, 5)
	
	local network = update.network:new('eth0')
	network.label_upload = label:new('')
	network.label_download = label:new('')
	network.label_upload_speed = label:new('')
	network.label_download_speed = label:new('')
	network.graph_upload_speed = graph:new(1, 2.5 * 1024, 60)
	network.graph_upload_speed:logarithmic_scale(0.01, 10)
	network.graph_download_speed = graph:new(1, 5 * 1024, 60)
	network.graph_download_speed:logarithmic_scale(0.01, 10)
	register_updater(instance, root_window1, network, 1.5)
	
	local memory = update.memory:new()
	memory.label_used_memory_percent = label:new('')
	register_updater(instance, root_window1, memory, 10)

	local calendar = update.calendar:new()
	local calendar_widget = calendar:build_calendar(main_style)
	register_updater(instance, root_window1, calendar, 1)
	
	
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
	
	vbox2:add_widget(center(datetime.label_time), true, false)
	vbox2:add_widget(center(datetime.label_date), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(calendar_widget, true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('Kernel: ', kernel.label_kernel), true, false)
	vbox2:add_widget(label_and_value('Uptime: ', uptime.label_uptime), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('CPU: ', cpuinfo.label_cpuinfo), true, false)
	vbox2:add_widget(label_and_value('RAM usage: ', memory.label_used_memory_percent), true, false)
	
	vbox2:add_widget(separator:new(true), true, false)
	
	vbox2:add_widget(label_and_value('Upload: ', network.label_upload), true, false)
	vbox2:add_widget(label_and_value('Upload speed: ', network.label_upload_speed), true, false)
	network.graph_upload_speed:min_size(1, 40)
	vbox2:add_widget(network.graph_upload_speed, true, false)

	vbox2:add_widget(label_and_value('Download: ', network.label_download), true, false)
	vbox2:add_widget(label_and_value('Download speed: ', network.label_download_speed), true, false)
	network.graph_download_speed:min_size(1, 40)
	vbox2:add_widget(network.graph_download_speed, true, false)
	
	
	vbox2:add_widget(separator:new(true), true, false)
	

	
end
