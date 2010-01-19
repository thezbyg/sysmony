--[[
Copyright (c) 2010, Albertas Vyšniauskas
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]--

lsb_release = {

	get_distributor = function ()
		local f = io.popen("lsb_release --id 2>&1")
		if f then
			local l = f:read("*a")
			f:close()
			local i, j, distributor = string.find(l, "Distributor ID:%s*([^\n\r]+)")
			return distributor
		end
	end,

	get_description = function ()
		local f = io.popen("lsb_release --description 2>&1")
		if f then
			local l = f:read("*a")
			f:close()
			local i, j, description = string.find(l, "Description:%s*([^\n\r]+)")
			return description
		end
	end,
	
	get_release = function ()
		local f = io.popen("lsb_release --release 2>&1")
		if f then
			local l = f:read("*a")
			f:close()
			local i, j, release = string.find(l, "Release:%s*([^\n\r]+)")
			return release
		end
	end,
	
	get_codename = function ()
		local f = io.popen("lsb_release --codename 2>&1")
		if f then
			local l = f:read("*a")
			f:close()
			local i, j, codename = string.find(l, "Codename:%s*([^\n\r]+)")
			return codename
		end
	end,

}
