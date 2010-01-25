--[[
Copyright (c) 2010, Albertas Vyšniauskas
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]--

update.mpris = {

    new = function(self)
    	local n = {}
		setmetatable(n, self)
        n.label_artist = nil
		n.label_genre = nil
		n.label_album = nil
		n.label_title = nil
		n.container = nil
		return n
	end,

	callback = function (self, data)
		if data.status[1] == 0 or data.status[1] == 1 then
			if self.container then self.container:visible(true) end
       		if self.label_artist then self.label_artist:text(data.artist) end		
    		if self.label_title then self.label_title:text(data.title) end		
    		if self.label_genre then self.label_genre:text(data.genre) end		
    		if self.label_album then self.label_album:text(data.album) end
		else
			if self.container then self.container:visible(false) end
       		if self.label_artist then self.label_artist:text('') end		
    		if self.label_title then self.label_title:text('') end		
    		if self.label_genre then self.label_genre:text('') end		
    		if self.label_album then self.label_album:text('') end  
		end
	end,
}

update.mpris.__index = update.mpris


