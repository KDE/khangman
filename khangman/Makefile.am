####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = khangman
khangman_SOURCES = info.cpp infodlg.ui mainw.ui khangman.cpp main.cpp khsplash.cpp 
khangman_LDADD   =  $(LIB_KDEUI) 

SUBDIRS = data pics sounds 

EXTRA_DIST = khsplash.h khsplash.cpp main.cpp lo16-app-khangman.png khangman.cpp lo32-app-khangman.png khangman.h khangman.desktop hi16-app-khangman.png hi32-app-khangman.png mainw.ui infodlg.ui info.cpp info.h 

KDE_ICON = khangman

appsdir = $(kde_appsdir)/Edutainment/Languages
apps_DATA = khangman.desktop

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
khangman_LDFLAGS = $(all_libraries) $(KDE_RPATH)

messages:  rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/khangman.pot; \
	fi

