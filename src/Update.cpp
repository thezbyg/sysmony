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

#include "Update.h"

#include <time.h>
#include <unistd.h>

Update::Update(){

}

Update::~Update(){
	
}

bool Update::tick(uint32_t now){
	if (updatePending(now)){
		update();
		return true;		
	}
	return false;
}

void Update::update(){
	
}

uint32_t Update::getSleepTime(uint32_t now){
	return 0;
}

bool Update::updatePending(uint32_t now){
	return true;
}



PollingUpdate::PollingUpdate(uint32_t update_interval_, uint32_t now){
	last_update = now - update_interval_;
	update_interval = update_interval_;
}

PollingUpdate::~PollingUpdate(){
	
}

void PollingUpdate::update(){

}

bool PollingUpdate::tick(uint32_t now){
	if (updatePending(now)){
		update();
		last_update = now;
		return true;		
	}
	return false;
}

uint32_t PollingUpdate::getSleepTime(uint32_t now){
	uint32_t t = now - last_update;
	
	if (t >= update_interval){
		return 1;
	}else{
		return update_interval - t;
	}
}

bool PollingUpdate::updatePending(uint32_t now){
	return (now - last_update >= update_interval);
}
