####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = new_game.wav splash.wav new_game.ogg splash.ogg 

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/khangman/sounds/
	$(INSTALL_DATA) $(srcdir)/new_game.wav $(kde_datadir)/khangman/sounds/new_game.wav
	$(mkinstalldirs) $(kde_datadir)/khangman/sounds/
	$(INSTALL_DATA) $(srcdir)/splash.wav $(kde_datadir)/khangman/sounds/splash.wav
	$(mkinstalldirs) $(kde_datadir)/khangman/sounds/
	$(INSTALL_DATA) $(srcdir)/new_game.ogg $(kde_datadir)/khangman/sounds/new_game.ogg
	$(mkinstalldirs) $(kde_datadir)/khangman/sounds/
	$(INSTALL_DATA) $(srcdir)/splash.ogg $(kde_datadir)/khangman/sounds/splash.ogg

uninstall-local:
	-rm -f $(kde_datadir)/khangman/sounds/new_game.wav
	-rm -f $(kde_datadir)/khangman/sounds/splash.wav
	-rm -f $(kde_datadir)/khangman/sounds/new_game.ogg
	-rm -f $(kde_datadir)/khangman/sounds/splash.ogg

####### kdevelop will overwrite this part!!! (end)############
