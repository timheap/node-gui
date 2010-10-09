#include "ngtk_window.h"

#include <v8.h>
#include <gtk/gtk.h>
#include "ngtk.h"

#include <stdio.h>

namespace ngtk {

using namespace v8;

#define NGTK_WINDOW_DEFAULT_WIDTH  640
#define NGTK_WINDOW_DEFAULT_HEIGHT 480

Persistent<FunctionTemplate> Window::constructor_template;

// Public constructor
Window* Window::New (void) {
  HandleScope scope;

  Local<Object> window = constructor_template->GetFunction()->NewInstance();

  return ObjectWrap::Unwrap<Window>(window);
}

// ECMAScript constructor.
Handle<Value> Window::New (const Arguments &args) {
  HandleScope scope;

  Window *window = new Window();
  window->Wrap(args.This());

  // Add the onClose handler
  Persistent<Object> *self = new Persistent<Object>();
  *self = Persistent<Object>::New(args.This());

  g_signal_connect(G_OBJECT(window->widget_), "destroy", G_CALLBACK(Window::onClose), (gpointer) self);

  return args.This();
}

Window::Window (void) {
  widget_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
}

// Add()
// For adding a container to the window.
Handle<Value> Window::Add (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  // TODO: Check arg is an instance of Widget
  Local<Object> arg = args[0]->ToObject();
  GtkWidget *other  = Widget::Gtk(arg);

  gtk_container_add(GTK_CONTAINER(window), other);
  gtk_widget_show(other);

  return args.This();
}

// Show()
// For showing the window.
Handle<Value> Window::Show (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  gtk_widget_show_all(window);

  main_loop_level++;

  if (main_loop_level == 1) {
    Handle<Value> onShow = args.This()->Get(String::New("onShow"));

    if (onShow->IsFunction()) {
      v8::Handle<v8::Function>::Cast(onShow)->Call(args.This(), 0, NULL);
    }
  }

  return args.This();
}

// SetTitle()
// For setting the window title.
Handle<Value> Window::SetTitle (const Arguments &args) {
  HandleScope scope;

  if (args[0]->IsString()) {
    GtkWidget *window = Widget::Gtk(args.This());

    gtk_window_set_title(GTK_WINDOW(window), *String::Utf8Value(args[0]->ToString()));
  }

  return args.This();
}

// GetTitle()
// For getting the window title.
Handle<Value> Window::GetTitle (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  return scope.Close(String::New(gtk_window_get_title(GTK_WINDOW(window))));
}

// SetResizable()
// gtk_window_set_resizable(GtkWindow, bool);
Handle<Value> Window::SetResizable (const Arguments &args) {
  HandleScope scope;

  if (args[0]->IsBoolean()) {
    GtkWidget *window = Widget::Gtk(args.This());

    gtk_window_set_resizable(GTK_WINDOW(window), args[0]->ToBoolean()->Value());
  }

  return args.This();
}

// GetResizable()
// gtk_window_get_resizable(GtkWindow);
Handle<Value> Window::GetResizable (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  return scope.Close(Boolean::New(gtk_window_get_resizable(GTK_WINDOW(window))));
}

// SetDefaultSize()
// gtk_window_set_default_size(GtkWindow, gint height, gint width);
Handle<Value> Window::SetDefaultSize (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  gint width, height;

  if (args[0]->IsNumber()) {
    width = args[0]->Int32Value();
  } else {
    width = NGTK_WINDOW_DEFAULT_WIDTH;
  }

  if (args[1]->IsNumber()) {
    height = args[1]->Int32Value();
  } else {
    height = NGTK_WINDOW_DEFAULT_HEIGHT;
  }

  gtk_window_set_default_size(GTK_WINDOW(window), width, height);

  return args.This();
}

// GetSize()
// gtk_window_get_size(GtkWindow, gint *width, gint *height);
Handle<Value> Window::GetSize (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  gint width, height;
  Local<Array> ret = Array::New(2);

  gtk_window_get_size(GTK_WINDOW(window), &width, &height);

  ret->Set(0, Integer::New(width));
  ret->Set(1, Integer::New(height));

  return scope.Close(ret);
}

// SetPosition()
// gtk_window_set_position(GtkWindow, );
Handle<Value> Window::SetPosition (const Arguments &args) {
  HandleScope scope;

  if (args[0]->IsNumber()) {
    GtkWindowPosition position = (GtkWindowPosition) args[0]->Int32Value();

    switch (position) {
    case GTK_WIN_POS_NONE:
    case GTK_WIN_POS_CENTER:
    case GTK_WIN_POS_MOUSE:
    case GTK_WIN_POS_CENTER_ALWAYS:
    case GTK_WIN_POS_CENTER_ON_PARENT:
      GtkWidget *window = Widget::Gtk(args.This());
      gtk_window_set_position(GTK_WINDOW(window), position);
      break;
    }
  }

  return args.This();
}

// GetPosition()
// gtk_window_get_position(GtkWindow, gint *x, gint *y);
Handle<Value> Window::GetPosition (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  gint x, y;
  Local<Array> ret = Array::New(2);

  gtk_window_get_position(GTK_WINDOW(window), &x, &y);

  ret->Set(0, Integer::New(x));
  ret->Set(1, Integer::New(y));

  return scope.Close(ret);
}

// SetOpacity()
// gtk_window_set_opacity(GtkWindow, gdouble opacity);
Handle<Value> Window::SetOpacity (const Arguments &args) {
  HandleScope scope;

  if (args[0]->IsNumber()) {
    GtkWidget *window = Widget::Gtk(args.This());

    gdouble opacity = args[0]->NumberValue();

    gtk_window_set_opacity(GTK_WINDOW(window), opacity);
  }

  return args.This();
}

// GetOpacity()
// gtk_window_get_opacity(GtkWindow, gint *width, gint *height);
Handle<Value> Window::GetOpacity (const Arguments &args) {
  HandleScope scope;

  GtkWidget *window = Widget::Gtk(args.This());

  return scope.Close(Number::New(gtk_window_get_opacity(GTK_WINDOW(window))));
}

// onClose() - Called when the window is being destoyed.
void Window::onClose (GtkWidget *widget, gpointer dataCast) {
  Persistent<Object>   *self    = reinterpret_cast<Persistent<Object>*>(dataCast);
  v8::Handle<v8::Value> onClose = (*self)->Get(String::New("onClose"));

  if (onClose->IsFunction()) {
    Handle<Function>::Cast(onClose)->Call(*self, 0, NULL);
  }

  main_loop_level--;
}

// Export.
void Window::Initialize (Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(Window::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Window"));

  // Position constants.
  NGTK_DEFINE_CONSTANT(target, "WIN_POS_NONE",             GTK_WIN_POS_NONE);
  NGTK_DEFINE_CONSTANT(target, "WIN_POS_CENTER",           GTK_WIN_POS_CENTER);
  NGTK_DEFINE_CONSTANT(target, "WIN_POS_MOUSE",            GTK_WIN_POS_MOUSE);
  NGTK_DEFINE_CONSTANT(target, "WIN_POS_CENTER_ALWAYS",    GTK_WIN_POS_CENTER_ALWAYS);
  NGTK_DEFINE_CONSTANT(target, "WIN_POS_CENTER_ON_PARENT", GTK_WIN_POS_CENTER_ON_PARENT);

  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "show",           Window::Show);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "setTitle",       Window::SetTitle);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "getTitle",       Window::GetTitle);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "setResizable",   Window::SetResizable);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "getResizable",   Window::GetResizable);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "setDefaultSize", Window::SetDefaultSize);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "getSize",        Window::GetSize);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "setPosition",    Window::SetPosition);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "getPosition",    Window::GetPosition);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "setOpacity",     Window::SetOpacity);
  NGTK_SET_PROTOTYPE_METHOD(constructor_template, "getOpacity",     Window::GetOpacity);

  target->Set(String::NewSymbol("Window"), constructor_template->GetFunction());
}

} // namespace ngtk

