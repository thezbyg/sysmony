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

#ifndef UPDATE_MPRIS_H_
#define UPDATE_MPRIS_H_

#include "../Update.h"

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include <stdint.h>
#include <stdbool.h>

#include <string>

class UpdateMpris:public Update{
protected:
    GStaticRecMutex data_mutex;
	DBusGProxy *player;

	std::string artist;
	std::string album;
	std::string genre;
	std::string title;

	int32_t status[4];
public:
	UpdateMpris(Instance *instance, RootWindow *root_window, const char* player_name);
	virtual ~UpdateMpris();

	virtual bool ready(uint32_t now);
	virtual void update(uint32_t now);
	virtual uint32_t getSleepTime(uint32_t now);

    void lock();
	void unlock();

	static void status_cb(DBusGProxy *player_proxy, GValueArray *status, UpdateMpris *mpris);
	static void track_cb(DBusGProxy *player_proxy, GHashTable *table, UpdateMpris *mpris);
};


#endif /* UPDATE_MPRIS_H_ */


