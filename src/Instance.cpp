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

#include "lua/Bindings.h"
#include "lua/Instance.h"
#include "lua/Sysinfo.h"
#include "lua/Uname.h"
#include "lua/DateTime.h"
#include "lua/Style.h"
#include "lua/Color.h"

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
	
	/*cairo_rectangle(cr, cliprect.x, cliprect.y, cliprect.width, cliprect.height);
	cairo_set_source_rgba(cr, (rand()%100)/100.0, (rand()%133)/133.0, (rand()%214)/214.0, 1);
	cairo_stroke(cr);*/
	
#endif

}



RootWindow::RootWindow(shared_ptr<layout::Window> window_, shared_ptr<engine::Render> render_lib_){
	
	window = window_;
	render = render_lib_;

	GdkWindow* rootwindow = gdk_get_default_root_window();
	
	GdkVisual* visual = gdk_visual_get_best_with_both(32, GDK_VISUAL_TRUE_COLOR);
	
	GdkWindowAttr attr;
	attr.title = (gchar*)"sysmony";
	attr.event_mask = GdkEventMask(GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_STRUCTURE_MASK);
	attr.x = window->getPosition().x;
	attr.y = window->getPosition().y;
	attr.width = window->getAllocation().width;
	attr.height = window->getAllocation().height;
	
	attr.window_type = GDK_WINDOW_TOPLEVEL;
	attr.visual = visual;
	attr.colormap = gdk_colormap_new(visual, true);
	attr.override_redirect = false;
	attr.wclass = GDK_INPUT_OUTPUT;
	
	gdk_window = gdk_window_new(rootwindow, &attr, 
		GdkWindowAttributesType(GDK_WA_NOREDIR | GDK_WA_VISUAL | GDK_WA_COLORMAP 
		| GDK_WA_X | GDK_WA_Y | GDK_WA_TITLE));
	
	gdk_window_stick(gdk_window);
	gdk_window_set_keep_below(gdk_window, true);
	
	gdk_window_set_type_hint(gdk_window, GDK_WINDOW_TYPE_HINT_DESKTOP);
	
	
	condition = g_cond_new();
	mutex = g_mutex_new();
	worker_finish = false;
	worker_thread = NULL;
	
}

void RootWindow::show(){
	gdk_window_show_unraised(gdk_window);
}

RootWindow::~RootWindow(){
	gdk_window_destroy(gdk_window);
	
	g_cond_free(condition);
	g_mutex_free(mutex);
}

void RootWindow::addUpdater(shared_ptr<Update> update){
	updates.push_back(update);
}

void RootWindow::startUpdateThread(){
	GError *error;
	worker_thread = g_thread_create((GThreadFunc)update_worker_thread, this, true, &error);
}

void RootWindow::finishUpdateThread(){
	
	worker_finish = true;
	
	g_mutex_lock(mutex);
	g_cond_signal(condition);
	
	if (worker_thread){
		g_thread_join(worker_thread);
	}
	
	g_mutex_unlock(mutex);
}

void RootWindow::lockUpdates(){
	g_mutex_lock(mutex);
}

void RootWindow::unlockUpdates(){
	g_mutex_unlock(mutex);
}

gpointer RootWindow::update_worker_thread(RootWindow *root_window){
	root_window->updateThread();
	return 0;
}

gboolean RootWindow::redraw_rectangle(struct RedrawData *redraw){
	gdk_threads_enter();
	gdk_window_invalidate_rect(redraw->window, &redraw->rectangle, true);
	delete redraw;
	gdk_threads_leave();
	return false;
}

uint32_t getTime(){
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	
	return ts.tv_nsec/1000000 + ts.tv_sec*1000;
}

void RootWindow::updateThread(){
	for (;;){
		GTimeVal timeval;

		g_mutex_lock(mutex);
		
		uint32_t min_sleep = 0;
		uint32_t sleep_time;
		uint32_t now = getTime();
		
		for (list<shared_ptr<Update> >::iterator i = updates.begin(); i != updates.end(); i++){
			
			(*i)->tick(now);
			
			if ((sleep_time = (*i)->getSleepTime(now))){
				if ((!min_sleep) || (sleep_time < min_sleep))
					min_sleep = sleep_time;
			}
			
		}
		
		
		Rect2<double> dirty_rect;
		

		if (window){
			window->processEvents();
			dirty_rect = window->getInvalidated();
			window->clearInvalidated();
		
		
			if (!dirty_rect.isEmpty()){
				
				struct RedrawData *redraw = new struct RedrawData;
				
				redraw->window = gdk_window;
				redraw->rectangle.x = floor(dirty_rect.x)-1;
				redraw->rectangle.y = floor(dirty_rect.y)-1;
				redraw->rectangle.width = ceil(dirty_rect.width)+1;
				redraw->rectangle.height = ceil(dirty_rect.height)+1;	
				
				GMainContext *main_context = g_main_context_get_thread_default();
				GSource *source = g_idle_source_new();
				g_source_set_callback(source, (GSourceFunc)redraw_rectangle, redraw, 0);
				g_source_attach(source, main_context);
				
			}
			
		}
			


		g_get_current_time(&timeval);
		if (min_sleep){
			min_sleep &= 0xffffff00;
			min_sleep += 300;

			g_time_val_add(&timeval, min_sleep*1000);
		}else{
			g_time_val_add(&timeval, 30*60*1000*1000);
		}
		
#ifndef NDEBUG
		printf("Tick %d\n", min_sleep);
#endif
		
		g_cond_timed_wait(condition, mutex, &timeval);
		g_mutex_unlock(mutex);
		
		if (worker_finish) break;
	}
}

void RootWindow::draw(Rect2<double> &rect, cairo_t *cr){
	DrawContext dc;
	dc.setCairo(cr);

	if (window) {
		
		//shared_ptr<engine::StyleState> style_state = shared_ptr<engine::StyleState>(render->getDefaultStyleState());
		//dc.pushStyleState(style_state);
		
		//dc.pushStyleStateClone();
		//window->applyStyles(&dc);
		
		window->draw(rect, &dc);
		
		//dc.popStyleState();
		
	}else{
		cerr << "RootWindow is NULL" << endl;
	}
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
	
	case GDK_BUTTON_PRESS:
		/*printf("button pressed\n");
		gdk_window_invalidate_rect(e->button.window, 0, true);*/
		break;
	
	case GDK_KEY_PRESS:
		//printf("key pressed\n");
		//gdk_window_invalidate_rect(e->key.window, 0, true);
		break;
	
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
	lua_close(L);
}

Instance::Instance(){
	
	
	L= luaL_newstate();
	luaL_openlibs(L);
	
	luaopen_all_bindingswindow(L);
	luaopen_instance(L);
	luaopen_rootwindow(L);
	luaopen_renderlib(L);
	luaopen_sysinfo(L);
	luaopen_uname(L);
	luaopen_datetime(L);
	luaopen_style(L);
	luaopen_color(L);

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
	
	g_free(lua_path);
	g_free(lua_root_path);
	g_free(lua_user_path);
	
	tmp = build_filename("init.lua");
	status = luaL_loadfile(L, tmp) || lua_pcall(L, 0, 0, 0);
	if (status) {
		cerr << lua_tostring(L, -1) << endl;
	}
	g_free(tmp);
	
	
	
	lua_pushstring(L, "sysmony");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "data_path");
	lua_pushstring(L, tmp = build_filename(NULL));
	lua_settable(L, -3);
	g_free(tmp);
	
	loadRenderLibrary("sysmony_render_engine");
	getRenderLib("sysmony_render_engine")->loadLuaLibrary(L);
	
	buildLayout();
	

	
	
	
	
	

	
	//layout = shared_ptr<LayoutMain>(new LayoutMain());
	//window = getWindow(this);
	
	gdk_event_handler_set((GdkEventFunc)event_func, this, NULL);
	mainloop = g_main_loop_new(g_main_context_default(), FALSE);
	
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
				cerr<<"getWindow: "<<lua_tostring (L, -1)<<endl;
			}
		}
	}
	lua_settop(L, stack_top);
}

void Instance::run(){
	g_main_loop_run(mainloop);
	
	for (list<shared_ptr<RootWindow> >::iterator i = root_windows.begin(); i != root_windows.end(); i++){
		(*i)->finishUpdateThread();
	}

}

GMainLoop* Instance::getMainloop() const{
	return mainloop;	
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
		cerr << "Exception caught while loading " << library_name << endl;
		return -1;
	}
}

shared_ptr<engine::Render> Instance::getRenderLib(const char *library_name){
	if (render_libs.find(library_name) != render_libs.end()){
		return render_libs[library_name];
	}else return shared_ptr<engine::Render>();
}

