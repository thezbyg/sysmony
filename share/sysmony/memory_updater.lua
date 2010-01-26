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

update.memory = {
    new = function(self)
    	local n = {}
		setmetatable(n, self)
        n.label_total_memory = nil
		n.label_free_memory = nil
		n.label_free_memory_percent = nil
        n.label_used_memory = nil
        n.label_used_memory_percent = nil
		return n
	end,
	
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
		
		if self.label_total_memory then	self.label_total_memory:text(format_size(values['MemTotal']*1024)) end
		if self.label_free_memory then self.label_free_memory:text(format_size((values['Buffers'] + values['Cached'] + values['MemFree'])*1024)) end
		if self.label_free_memory_percent then self.label_free_memory_percent:text(format_percentage((values['Buffers'] + values['Cached'] + values['MemFree'])/values['MemTotal'])) end

		if self.label_used_memory then self.label_used_memory:text(format_size((values['MemTotal'] - (values['Buffers'] + values['Cached'] + values['MemFree']))*1024)) end
		if self.label_used_memory_percent then self.label_used_memory_percent:text(format_percentage(1 - (values['Buffers'] + values['Cached'] + values['MemFree'])/values['MemTotal'])) end
	end,
} 

update.memory.__index = update.memory

