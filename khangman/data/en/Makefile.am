####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = easy.txt animals.txt hard.txt medium.txt

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/khangman/data/en
	$(INSTALL_DATA) $(srcdir)/easy.txt $(kde_datadir)/khangman/data/en/easy.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/en
	$(INSTALL_DATA) $(srcdir)/animals.txt $(kde_datadir)/khangman/data/en/animals.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/en
	$(INSTALL_DATA) $(srcdir)/hard.txt $(kde_datadir)/khangman/data/en/hard.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/en
	$(INSTALL_DATA) $(srcdir)/medium.txt $(kde_datadir)/khangman/data/en/medium.txt

uninstall-local:
	-rm -f $(kde_datadir)/khangman/data/en/easy.txt
	-rm -f $(kde_datadir)/khangman/data/en/animals.txt
	-rm -f $(kde_datadir)/khangman/data/en/hard.txt
	-rm -f $(kde_datadir)/khangman/data/en/medium.txt

####### kdevelop will overwrite this part!!! (end)############
