####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = easy.txt animals.txt hard.txt medium.txt

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/khangman/data/fr
	$(INSTALL_DATA) $(srcdir)/easy.txt $(kde_datadir)/khangman/data/fr/easy.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/fr
	$(INSTALL_DATA) $(srcdir)/animals.txt $(kde_datadir)/khangman/data/fr/animals.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/fr
	$(INSTALL_DATA) $(srcdir)/hard.txt $(kde_datadir)/khangman/data/fr/hard.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/fr
	$(INSTALL_DATA) $(srcdir)/medium.txt $(kde_datadir)/khangman/data/fr/medium.txt

uninstall-local:
	-rm -f $(kde_datadir)/khangman/data/fr/easy.txt
	-rm -f $(kde_datadir)/khangman/data/fr!animals.txt
	-rm -f $(kde_datadir)/khangman/data/fr/hard.txt
	-rm -f $(kde_datadir)/khangman/data/fr/medium.txt

####### kdevelop will overwrite this part!!! (end)############
