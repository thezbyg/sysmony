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

#include "Instance.h"
#include "Paths.h"
#include "Time.h"

#include "lua/AllBindings.h"
#include "lua/Instance.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luaconf.h>
}

#include <math.h>
#include <iostream>

using namespace std;
using namespace layout;
using namespace lua;


static void clear_rect(cairo_t *cr, Rect2<double> &cliprect){

	cairo_rectangle(cr, cliprect.x, cliprect.y, cliprect.width, cliprect.height);
	cairo_clip(cr);
	cairo_new_path(cr);
	
	cairo_save(cr);
	cairo_set_source_rgba(cr, 0, 0, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_restore(cr);

#ifndef NDEBUG
	
	cairo_rectangle(cr, cliprect.x, cliprect.y, cliprect.width, cliprect.height);
	cairo_set_source_rgba(cr, (rand()%100)/100.0, (rand()%133)/133.0, (rand()%214)/214.0, 1);
	cairo_stroke(cr);
	
#endif

}


shared_ptr<RootWindow> Instance::getWindow(GdkWindow *gdk_window){
	for (list<shared_ptr<RootWindow> >::iterator i = root_windows.begin(); i != root_windows.end(); i++){
		if ((*i)->gdk_window == gdk_window)
			return *i;
	}
	return shared_ptr<RootWindow>();
}


static void event_func(GdkEvent *e, Instance *instance) {
	
	shared_ptr<RootWindow> root_window = instance->getWindow(e->any.window);

	switch(e->type){
	/*case GDK_MAP:
		
		break;*/
	
	case GDK_DELETE:
		g_main_loop_quit(instance->getMainloop());
		break;
	
	/*case GDK_BUTTON_PRESS:

		break;*/
	
	/*case GDK_KEY_PRESS:

		break;*/
	
	case GDK_EXPOSE:
	
		root_window->lockUpdates();
		{
			gdk_window_begin_paint_rect(e->expose.window, &e->expose.area);
			
			cairo_t *cr = gdk_cairo_create(GDK_DRAWABLE(e->expose.window));
			
			Rect2<double> rect(e->expose.area.x, e->expose.area.y, e->expose.area.width, e->expose.area.height);
			clear_rect(cr, rect);
			
			if (root_window) root_window->draw(rect, cr);

			cairo_destroy(cr);
			
			gdk_window_end_paint(e->expose.window);
		}
		root_window->unlockUpdates();

		break;
	default:
		{
		/*nothing*/
		}
	}

}

Instance::~Instance(){

	g_cond_free(msg_condition);
	g_mutex_free(msg_mutex);

	g_static_rec_mutex_free(&lua_mutex);
    dbus_g_connection_unref(bus);
	lua_close(L);
}


Instance::Instance(){
	
    dbus_g_thread_init();

 	msg_condition = g_cond_new();
	msg_mutex = g_mutex_new();
	msg_worker_finish = false;
	msg_thread = NULL;
 
	
	L = luaL_newstate();
	luaL_openlibs(L);
	
	int status;
	char *tmp;

	gchar* lua_root_path = build_filename("?.lua");
	gchar* lua_user_path = build_config_path("?.lua");
	
	gchar* lua_path = g_strjoin(";", lua_root_path, lua_user_path, (void*)0);
	
	lua_pushstring(L, "package");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "path");
	lua_pushstring(L, lua_path);
	lua_settable(L, -3);
	lua_pop(L, 1);
	
	g_free(lua_path);
	g_free(lua_root_path);
	g_free(lua_user_path);


	g_static_rec_mutex_init(&lua_mutex);
 	
	luaopen_all_sm_bindings(L);
	
	tmp = build_filename("init.lua");
	status = luaL_loadfile(L, tmp) || lua_pcall(L, 0, 0, 0);
	if (status) {
		cerr << lua_tostring(L, -1) << endl;
	}
	g_free(tmp);
	

	struct stat st;
	tmp = build_config_path("user_init.lua");
	if (g_stat(tmp, &st) != 0){
		gchar* config_dir = build_config_path(NULL);
		if (g_stat(config_dir, &st)!=0){
			g_mkdir(config_dir, S_IRWXU);
		}
		g_free(config_dir);
		
		gchar *user_init_template_path = build_filename("user_init_template.lua");
		
		FILE *user_init_template = fopen(user_init_template_path, "rb");
		if (user_init_template){
		
			FILE *user_init = fopen(tmp, "wb");
			if (user_init){
				char temp[4096];
				uint32_t length;
				
				while (!feof(user_init_template)){
					length = fread(temp, 1, 4096, user_init_template);
					if (length > 0){
						fwrite(temp, 1, length, user_init);
						if (length != 4096) break;
					}else break;
				}
				
				fclose(user_init);
			}
		
			fclose(user_init_template);
		}
		
		g_free(user_init_template_path);
	}
	
	if (g_stat(tmp, &st) == 0){
		status = luaL_loadfile(L, tmp) || lua_pcall(L, 0, 0, 0);
		if (status) {
			cerr << lua_tostring(L, -1) << endl;
		}
	}
	g_free(tmp);
	
	
	lua_pushstring(L, "sysmony");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "data_path");
	lua_pushstring(L, tmp = build_filename(NULL));
	lua_settable(L, -3);
	lua_pop(L, 1);
	g_free(tmp);

	
	lua_pushstring(L, "sysmony");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "config_path");
	lua_pushstring(L, tmp = build_config_path(NULL));
	lua_settable(L, -3);
	lua_pop(L, 1);
	g_free(tmp);
	
	loadRenderLibrary("sysmony_render_engine");
	getRenderLib("sysmony_render_engine")->loadLuaLibrary(L);


	gdk_event_handler_set((GdkEventFunc)event_func, this, NULL);
	mainloop = g_main_loop_new(g_main_context_default(), FALSE);
	
	
	bus = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);
	if (!bus) {
		cerr << "Failed to connect to the dbus daemon" << endl;
	}



	
	buildLayout();
	
	
	for (list<shared_ptr<RootWindow> >::iterator i = root_windows.begin(); i != root_windows.end(); i++){
		
		(*i)->window->processEvents();
		(*i)->window->clearInvalidated();
		
		(*i)->startUpdateThread();
		(*i)->show();
	}
	
	gdk_threads_leave();
}


void Instance::buildLayout(){
	
	
	int status;
	int stack_top = lua_gettop(L);
	lua_getglobal(L, "sysmony");
	int sysmony_namespace = lua_gettop(L);
	
	if (lua_type(L, -1)!=LUA_TNIL){
		
		lua_pushstring(L, "build");
		lua_gettable(L, sysmony_namespace);
		if (lua_type(L, -1) != LUA_TNIL){
			
			lua_pushinstance(L, this);
			
			if ((status=lua_pcall(L, 1, 0, 0))==0){

			}else{
				cerr<<"buildLayout: "<<lua_tostring (L, -1)<<endl;
			}
		}
	}
	lua_settop(L, stack_top);
}

void Instance::run(){

 	GError *error;
	msg_thread = g_thread_create((GThreadFunc)msg_worker_thread, this, true, &error);

	
	g_main_loop_run(mainloop);
	
	for (list<shared_ptr<RootWindow> >::iterator i = root_windows.begin(); i != root_windows.end(); i++){
		(*i)->finishUpdateThread();
	}


 	msg_worker_finish = true;
	
	g_mutex_lock(msg_mutex);
	g_cond_signal(msg_condition);
	
	if (msg_thread){
		g_thread_join(msg_thread);
	}
	
	g_mutex_unlock(msg_mutex);

}

GMainLoop* Instance::getMainloop() const{
	return mainloop;	
}

void Instance::lockLua(){
	g_static_rec_mutex_lock(&lua_mutex);	
}

void Instance::unlockLua(){
	g_static_rec_mutex_unlock(&lua_mutex);	
}

	
void Instance::addRootWindow(shared_ptr<RootWindow> root_window){
	root_windows.push_back(root_window);
}

int Instance::loadRenderLibrary(const char *library_name){
	try{
		engine::Render *rend = engine::load_render_engine(library_name);
		if (rend){
			render_libs[library_name] = shared_ptr<engine::Render>(rend);
			return 0;
		}
		return -1;
	}catch(...){
		cerr << "Exception caught while loading " << library_name << " render library" << endl;
		return -1;
	}
}

shared_ptr<engine::Render> Instance::getRenderLib(const char *library_name){
	if (render_libs.find(library_name) != render_libs.end()){
		return render_libs[library_name];
	}else return shared_ptr<engine::Render>();
}


DBusGConnection* Instance::getDBus(){
	return bus;	
}


void Instance::queueUpdate(Update *update){
	g_mutex_lock(msg_mutex);
	queued_updates[update->getRootWindow()].push_back(update);
    g_cond_signal(msg_condition);
	g_mutex_unlock(msg_mutex);
}

gpointer Instance::msg_worker_thread(Instance *instance){
	for (;;){
		g_mutex_lock(instance->msg_mutex);

 		for (UpdateMap::iterator i = instance->queued_updates.begin(); i != instance->queued_updates.end(); i++){
			(*i).first->lockUpdates();
			for (list<Update*>::iterator j = (*i).second.begin(); j != (*i).second.end(); j++){
				(*j)->update(getTime());	
			}
			(*i).first->invalidate();
			(*i).first->unlockUpdates();
		}
		instance->queued_updates.clear();

		g_cond_wait(instance->msg_condition, instance->msg_mutex);	
		g_mutex_unlock(instance->msg_mutex);
		if (instance->msg_worker_finish) break;
	} 		
	return 0;	
}

