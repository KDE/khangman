####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = khangman
khangman_SOURCES = mainw.ui khangman.cpp main.cpp khangmanview.cpp
khangman_LDADD   =  $(LIB_KDEUI)

SUBDIRS = data pics sounds

EXTRA_DIST = main.cpp lo16-app-khangman.png khangman.cpp lo32-app-khangman.png khangman.h khangman.desktop cr16-app-khangman.png cr32-app-khangman.png mainw.ui khangmanview.h version.h

KDE_ICON = khangman

appsdir = $(kde_appsdir)/Edutainment/Languages
apps_DATA = khangman.desktop

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path.
khangman_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rc_DATA = khangmanui.rc
rcdir = $(kde_datadir)/khangman

messages:  rc.cpp
	$(EXTRACTRC) *.ui *.rc > rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) data.i18n $$LIST -o $(podir)/khangman.pot; \
	fi

