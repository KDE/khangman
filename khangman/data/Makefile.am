####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = level2.txt level3.txt level4.txt easy.txt 

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/khangman/data/
	$(INSTALL_DATA) $(srcdir)/level2.txt $(kde_datadir)/khangman/data/level2.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/
	$(INSTALL_DATA) $(srcdir)/level3.txt $(kde_datadir)/khangman/data/level3.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/
	$(INSTALL_DATA) $(srcdir)/level4.txt $(kde_datadir)/khangman/data/level4.txt
	$(mkinstalldirs) $(kde_datadir)/khangman/data/
	$(INSTALL_DATA) $(srcdir)/easy.txt $(kde_datadir)/khangman/data/easy.txt

uninstall-local:
	-rm -f $(kde_datadir)/khangman/data/level2.txt
	-rm -f $(kde_datadir)/khangman/data/level3.txt
	-rm -f $(kde_datadir)/khangman/data/level4.txt
	-rm -f $(kde_datadir)/khangman/data/easy.txt

####### kdevelop will overwrite this part!!! (end)############
