/*
 * Copyright (c) 2010, Albertas Vyšniauskas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of the software author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Render.h"
#include <dlfcn.h>

#include <iostream>
#include <string>
using namespace std;

namespace engine {

Render* load_render_engine(const char* engine_name){
	
	string dl_name = "lib";
	dl_name += engine_name;
	dl_name +=".so";

	void* handle = dlopen(dl_name.c_str(), RTLD_LAZY);
	if (!handle) {
		cerr << dlerror() << endl;
		return 0;
	}

	sm_get_render_engine_t sm_get_render_engine = reinterpret_cast<sm_get_render_engine_t>(dlsym(handle, "sm_get_render_engine"));
	if (!sm_get_render_engine) {
		cerr << dlerror() << endl;
		dlclose(handle);
		return 0;
	}
	
	Render* r = sm_get_render_engine();
	if (r) return r;

	dlclose(handle);
	return 0;
}

Render::Render(){
}

Render::~Render(){
}

int Render::loadLuaLibrary(lua_State *L){
	return -1;
}

	
}
