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



function register_updater(root_window, object_class, seconds)
	root_window:add_updater( pollingluaupdate:new(object_class.callback, object_class, seconds*1000) )
end

