####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = easy.txt animals.txt hard.txt medium.txt

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/khangman/data/sv
	$(INSTALL_DATA) $(srcdir)/easy.txt $(kde_datadir)/khangman/data/sv/easy.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/sv
	$(INSTALL_DATA) $(srcdir)/animals.txt $(kde_datadir)/khangman/data/sv/animals.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/sv
	$(INSTALL_DATA) $(srcdir)/hard.txt $(kde_datadir)/khangman/data/sv/hard.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/sv
	$(INSTALL_DATA) $(srcdir)/medium.txt $(kde_datadir)/khangman/data/sv/medium.txt

uninstall-local:
	-rm -f $(kde_datadir)/khangman/data/sv/easy.txt
	-rm -f $(kde_datadir)/khangman/data/sv/animals.txt
	-rm -f $(kde_datadir)/khangman/data/sv/hard.txt
	-rm -f $(kde_datadir)/khangman/data/sv/medium.txt

####### kdevelop will overwrite this part!!! (end)############
