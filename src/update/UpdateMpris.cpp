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

#include "UpdateMpris.h"

#define DBUS_MPRIS_NAMESPACE		"org.mpris."
#define DBUS_MPRIS_PLAYER			"org.freedesktop.MediaPlayer"
#define DBUS_MPRIS_PLAYER_PATH		"/Player"
#define DBUS_MPRIS_TRACK_SIGNAL		"TrackChange"
#define DBUS_MPRIS_STATUS_SIGNAL	"StatusChange"

#define DBUS_TYPE_G_STRING_VALUE_HASHTABLE (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE))
#define DBUS_TYPE_G_STRUCT_INT (dbus_g_type_get_struct("GValueArray", G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))

#include <iostream>
using namespace std;

UpdateMpris::UpdateMpris(Instance *instance_, RootWindow *root_window_, const char *player_name):Update(instance_){
	
    g_static_rec_mutex_init(&data_mutex);

	setRootWindow(root_window_);

    string name = DBUS_MPRIS_NAMESPACE;
	name += player_name;

 	player = dbus_g_proxy_new_for_name(instance_->getDBus(), name.c_str(), DBUS_MPRIS_PLAYER_PATH, DBUS_MPRIS_PLAYER);

	dbus_g_proxy_add_signal(player, DBUS_MPRIS_TRACK_SIGNAL, DBUS_TYPE_G_STRING_VALUE_HASHTABLE, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(player, DBUS_MPRIS_TRACK_SIGNAL, G_CALLBACK(track_cb), this, NULL);

	dbus_g_proxy_add_signal(player, DBUS_MPRIS_STATUS_SIGNAL, DBUS_TYPE_G_STRUCT_INT, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(player, DBUS_MPRIS_STATUS_SIGNAL, G_CALLBACK(status_cb), this, NULL);
 
	GError *error = NULL;

	GValueArray *status = NULL;
    if (dbus_g_proxy_call(player, "GetStatus", &error, G_TYPE_INVALID, DBUS_TYPE_G_STRUCT_INT, &status, G_TYPE_INVALID)) {
    	status_cb(NULL, status, this);
	}else{
        cerr << error->message << endl;
		g_error_free(error);
		error = NULL;
	}
	if (status) g_value_array_free(status);

	GHashTable *table = NULL;
	if (dbus_g_proxy_call(player, "GetMetadata", &error, G_TYPE_INVALID, DBUS_TYPE_G_STRING_VALUE_HASHTABLE, &table, G_TYPE_INVALID)) {
		track_cb(NULL, table, this);
	}else{
        cerr << error->message << endl;
		g_error_free(error);
		error = NULL;
	}
	if (table) g_hash_table_destroy(table);


}

UpdateMpris::~UpdateMpris(){
	g_object_unref(player);

	g_static_rec_mutex_free(&data_mutex);
}

bool UpdateMpris::ready(uint32_t now){
	return false;
}

void UpdateMpris::update(uint32_t now){
}

uint32_t UpdateMpris::getSleepTime(uint32_t now){
	return 0;
}

void UpdateMpris::lock(){
	g_static_rec_mutex_lock(&data_mutex);
}

void UpdateMpris::unlock(){
	g_static_rec_mutex_unlock(&data_mutex);
}

void UpdateMpris::status_cb(DBusGProxy *player_proxy, GValueArray *status, UpdateMpris *mpris){
    GValue *value;
	mpris->lock();

	for (int i = 0; i < 4; i++){
		value = g_value_array_get_nth(status, i);
    	if (value != NULL && G_VALUE_HOLDS_INT(value))
			mpris->status[i] = g_value_get_int(value);
		else
			mpris->status[i] = -1;
	}
	
	mpris->unlock();
	mpris->instance->queueUpdate(mpris);
}

void UpdateMpris::track_cb(DBusGProxy *player_proxy, GHashTable *table, UpdateMpris *mpris){
	GValue *value;
	mpris->lock();

	value = (GValue *) g_hash_table_lookup(table, "artist");
	if (value != NULL && G_VALUE_HOLDS_STRING(value))
		mpris->artist = g_value_get_string(value);
	else
		mpris->artist = "";
	value = (GValue *) g_hash_table_lookup(table, "album");
	if (value != NULL && G_VALUE_HOLDS_STRING(value))
		mpris->album = g_value_get_string(value);
	else
		mpris->album = "";
	value = (GValue *) g_hash_table_lookup(table, "genre");
	if (value != NULL && G_VALUE_HOLDS_STRING(value))
		mpris->genre = g_value_get_string(value);
	else
		mpris->genre = "";
	value = (GValue *) g_hash_table_lookup(table, "title");
	if (value != NULL && G_VALUE_HOLDS_STRING(value))
		mpris->title = g_value_get_string(value);
	else
		mpris->title = "";

	mpris->unlock();
	mpris->instance->queueUpdate(mpris);
}


