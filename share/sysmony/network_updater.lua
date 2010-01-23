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

update.network = {
    
	new = function(self, interface)
    	local n = {}
		setmetatable(n, self)
		n.label_upload = nil
		n.label_download = nil

		n.label_upload_speed = nil
		n.label_download_speed = nil
		
		n.graph_upload_speed = nil
		n.graph_download_speed = nil
		
		n.prev_download = nil
		n.prev_upload = nil
		n.prev_download_lp = 0
		n.prev_upload_lp = 0

		n.interface = interface or "eth0"
		
		n.last_update = timer:now()
		return n
	end,
     
	callback = function (self)

		local f = assert(io.open("/proc/net/dev", "r"))
		
		f:read("*line")
		f:read("*line")
		
		while true do
			local line = f:read("*line")
			if line == nil then break end
			
			local interface, download, upload = string.match(line, "([%a%d]+):%s*(%d+)%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+(%d+)%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+%s+%d+")
			if interface == self.interface then
				local now = timer:now()
				local seconds = 1 / (self.last_update:difference(now) * 0.001)
				self.last_update = now
				
				if self.label_upload then self.label_upload:text(format_size(tonumber(upload))) end
				if self.label_download then self.label_download:text(format_size(tonumber(download))) end

                if self.prev_upload then
					self.prev_upload_lp = self.prev_upload_lp + 0.3 * ((tonumber(upload) - self.prev_upload) * seconds - self.prev_upload_lp)
                
					if self.graph_upload_speed then                	
						self.graph_upload_speed:push_value(self.prev_upload_lp / 1024)
					end
				end
				if self.prev_download then
					self.prev_download_lp = self.prev_download_lp + 0.3 * ((tonumber(download) - self.prev_download) * seconds - self.prev_download_lp)
					
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

update.network.__index = update.network

