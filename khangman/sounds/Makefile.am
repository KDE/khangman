####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = new_game.wav splash.wav 

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/khangman/sounds/
	$(INSTALL_DATA) $(srcdir)/new_game.wav $(kde_datadir)/khangman/sounds/new_game.wav
	$(mkinstalldirs) $(kde_datadir)/khangman/sounds/
	$(INSTALL_DATA) $(srcdir)/splash.wav $(kde_datadir)/khangman/sounds/splash.wav

uninstall-local:
	-rm -f $(kde_datadir)/khangman/sounds/new_game.wav
	-rm -f $(kde_datadir)/khangman/sounds/splash.wav

####### kdevelop will overwrite this part!!! (end)############
