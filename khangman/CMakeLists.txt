kde4_header()

add_subdirectory( data ) 
add_subdirectory( pics ) 
add_subdirectory( sounds ) 
add_subdirectory( icons ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdeedu/kdeeducore  )


########### next target ###############

set(khangman_SRCS 
   khangman.cpp 
   main.cpp 
   khangmanview.cpp 
   khnewstuff.cpp 
   timer.cpp )

kde4_automoc(${khangman_SRCS})

kde4_add_ui_files(khangman_SRCS generalui.ui languageui.ui timerui.ui )

kde4_add_kcfg_files(khangman_SRCS prefs.kcfgc )

kde4_add_executable(khangman ${khangman_SRCS})

target_link_libraries(khangman  ${KDE4_KDEUI_LIBS} kdeeducore knewstuff )

install_targets(/bin khangman )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES khangman.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES khangman.kcfg )
install_files( ${CONFIG_INSTALL_DIR} FILES khangmanrc )
install_files( ${DATA_INSTALL_DIR}/khangman FILES khangmanui.rc )

kde4_footer()



#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#bin_PROGRAMS = khangman
#khangman_SOURCES = khangman.cpp main.cpp khangmanview.cpp prefs.kcfgc khnewstuff.cpp generalui.ui languageui.ui timerui.ui timer.cpp
#khangman_LDADD   =  $(LIB_KDEUI) $(top_builddir)/libkdeedu/kdeeducore/libkdeeducore.la -lknewstuff
#
#SUBDIRS = data pics sounds icons
#
#EXTRA_DIST = main.cpp khangman.cpp khangman.h khangman.desktop khangmanview.h version.h khnewstuff.h timer.h timer.cpp
#
#xdg_apps_DATA = khangman.desktop
#kde_kcfg_DATA = khangman.kcfg
#kde_conf_DATA = khangmanrc
#
## set the include path for X, qt and KDE
#INCLUDES= -I$(top_srcdir)/libkdeedu/kdeeducore $(all_includes)
#
#METASOURCES = AUTO
#
## the library search path.
#khangman_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#rc_DATA = khangmanui.rc
#rcdir = $(kde_datadir)/khangman
#
#messages:  rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) data.i18n $$LIST -o $(podir)/khangman.pot; \
#	fi
#
