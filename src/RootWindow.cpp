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
		window->draw(rect, &dc);
	}else{
		cerr << "RootWindow is NULL" << endl;
	}
}




