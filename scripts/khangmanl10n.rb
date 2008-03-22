#!/usr/bin/env ruby
#
# This script gets all KHangMan data currently in trunk/l10n-kde4 svn and makes
# tarballs of it, removing svn hidden dirs and CMakeLists.txt
#
# (c) 2005 Mark Kretschmann <markey@web.de>
# (c) 2007 - 2008  Anne-Marie Mahfouf <annma@kde.org>
# modified by Carsten Niehaus
# Some parts of this code taken from cvs2dist
# License: GPL V2+

# Each tarball is packaged <iso>.tar.bz2 where <iso> is the language
# iso code used by KDE

# Globally these tarballs should be unpacked under
# $KDEDIRS/share/apps/kvtml
# Locally, these tarballs should be unpacked under
# $KDEHOME/share/apps/kvtml

puts "Connecting to l10n-kde4".center(40, " - ")

Dir.mkdir( "khangman-data")
Dir.chdir( "khangman-data" )

i18nlangs = `svn cat svn://anonsvn.kde.org/home/kde/branches/stable/l10n-kde4/subdirs`

# docs
for lang in i18nlangs
	print "doing #{lang}";
	lang.chomp!
	datadirname = "l10n-kde4/#{lang}/data/kdeedu/khangman"
	Dir.mkdir( "#{lang}" )
	Dir.chdir( "#{lang}" )
    `svn co -q svn://anonsvn.kde.org/home/kde/branches/stable/#{datadirname}`
	Dir.chdir( ".." )
	if !FileTest.exists?( "#{lang}/khangman" )
   		print "in if \n" 
   		Dir.rmdir("#{lang}")
	else
		system `mv #{lang}/khangman/* #{lang}`
		system `rm -Rf #{lang}/khangman/.*`
		system `rmdir #{lang}/khangman`
		system `rm #{lang}/CMakeLists.txt`
		if FileTest.exists?( "#{lang}/IMPROVE.txt" )
			system `rm #{lang}/IMPROVE.txt`
		end
	end
end

Dir.foreach('.') { |dir|
    	next if dir == "."
	next if dir == ".."
	`tar -cf #{dir}.tar #{dir}/`
	`bzip2 #{dir}.tar`
	system `rm -Rf #{dir}/`
}
