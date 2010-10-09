srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cfg(package='gtk+-2.0', atleast_version='2.8.0', uselib_store='GTK',
		mandatory=True, args='--cflags --libs')
  conf.env.append_value('CXXFLAGS', 
		'-DNDEBUG -DGTK -DGTK2 -DSCI_LEXER -DG_THREADS_IMPL_NONE'.split())

def oldbuild(bld):
  obj = bld.new_task_gen("cxx", "shlib", "pthread", "node_addon", uselib = 'GTK')
  obj.cxxflags = ["-Wall", "-ansi", "-pedantic"]
  obj.packages = 'gtk+-2.0'
  obj.target = "node-gtk"
  obj.source = """
	       src/gtk-window.cc
	       src/gtk-hbox.cc
	       src/gtk-vbox.cc
	       src/gtk-button.cc
	       src/gtk-alert.cc
	       src/gtk-entry.cc
         src/gtk.cc
  	       """

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon", uselib = 'GTK')
  obj.cxxflags = ["-Wall", "-ansi", "-pedantic"]
  obj.packages = 'gtk+-2.0'
  obj.target = "node-gtk"
  obj.source = """
               src/ngtk_widget.cc
               src/ngtk_window.cc
               src/ngtk.cc
               """
