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

#ifndef INSTANCE_H_
#define INSTANCE_H_

class Instance;

#include "Update.h"
#include "layout/Window.h"
#include "engine_api/Render.h"

#include <glib.h>
#include <gdk/gdk.h>

#include <list>
#include <map>

extern "C"{
#include <lua.h>
}

class RootWindow{
protected:
	GdkWindow *gdk_window;
	std::shared_ptr<layout::Window> window;
	std::list<std::shared_ptr<Update> > updates;
	std::shared_ptr<engine::Render> render;

	GCond *condition;
	GMutex *mutex;
	GThread *worker_thread;
	bool worker_finish;

public:
	
	RootWindow(std::shared_ptr<layout::Window> window, std::shared_ptr<engine::Render> render_lib);
	~RootWindow();

	void draw(Rect2<double> &rect, cairo_t *cr);

	void show();

	void addUpdater(shared_ptr<Update> update);

	void startUpdateThread();
	void finishUpdateThread();
	
	void lockUpdates();
	void unlockUpdates();

	void updateThread();

	struct RedrawData{
		GdkWindow *window;
		GdkRectangle rectangle;
	};

	static gpointer update_worker_thread(RootWindow *root_window);
	static gboolean redraw_rectangle(struct RedrawData *redraw);

	friend class Instance;
};

class Instance{
protected:
	std::list<shared_ptr<RootWindow > > root_windows;
	std::map<std::string, std::shared_ptr<engine::Render> > render_libs;

	GMainLoop *mainloop;

	lua_State *L;

public:
	
	Instance();
	~Instance();

	std::shared_ptr<RootWindow> getWindow(GdkWindow *gdk_window);

	void addRootWindow(std::shared_ptr<RootWindow> root_window);

	void run();
	void buildLayout();

	int loadRenderLibrary(const char *library_name);
	std::shared_ptr<engine::Render> getRenderLib(const char *library_name);

	GMainLoop* getMainloop() const;
};

#endif /* INSTANCE_H_ */
