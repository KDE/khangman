####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = khangman
khangman_SOURCES = khsplash.cpp khangman.cpp main.cpp 
khangman_LDADD   =  -lkdeui -lkdecore $(LIB_QT) -lX11 $(LIBSOCKET)


EXTRA_DIST = main.cpp khangman.cpp khangman.h khangman.desktop lo32-app-khangman.png lo16-app-khangman.png khsplash.cpp khsplash.h beta.png hg1.gif hg10.gif hg12.gif hg13.gif hg14.gif hg2.gif hg3.gif hg4.gif hg5.gif hg6.gif hg7.gif hg8.gif hg9.gif kh_sm.png 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/khangman.desktop $(kde_appsdir)/Applications/khangman.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-khangman.png $(kde_icondir)/locolor/32x32/apps/khangman.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-khangman.png $(kde_icondir)/locolor/16x16/apps/khangman.png
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/beta.png $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg1.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg10.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg12.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg13.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg14.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg2.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg3.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg4.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg5.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg6.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg7.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg8.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/hg9.gif $(kde_datadir)
	$(mkinstalldirs) 
	$(INSTALL_DATA) $(srcdir)/kh_sm.png $(kde_datadir)

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/khangman.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/khangman.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/khangman.png
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)
	-rm -f $(kde_datadir)

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
khangman_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE´s XML GUI builing
#rcdir = $(kde_datadir)/khangman
#rc_DATA = khangmanui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of 

# messages:

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/khangman.pot; \
	fi

