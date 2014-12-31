i18nc("NAME OF TRANSLATORS","Your names");
i18nc("EMAIL OF TRANSLATORS","Your emails");
// i18n: file: khangman.kcfg:10
// i18n: ectx: label, entry (CurrentLevel), group (Language)
i18n("Level");
// i18n: file: khangman.kcfg:17
// i18n: ectx: label, entry (ShowCharToolbar), group (Language)
i18n("Show the characters toolbar");
// i18n: file: khangman.kcfg:21
// i18n: ectx: label, entry (SelectedLanguage), group (Language)
i18n("The language selected by the user");
// i18n: file: khangman.kcfg:37
// i18n: ectx: label, entry (Mode), group (Look)
i18n("Background theme");
// i18n: file: khangman.kcfg:44
// i18n: ectx: label, entry (OneLetter), group (General)
i18n("Check this if you do not want to discover each instance of the same letter.");
// i18n: file: khangman.kcfg:48
// i18n: ectx: label, entry (EnableAnimations), group (General)
i18n("Enable Animations");
// i18n: file: khangman.kcfg:52
// i18n: ectx: label, entry (Sound), group (General)
i18n("If checked, sounds will be played for New Game and Win Game");
// i18n: file: khangman.kcfg:57
// i18n: ectx: label, entry (Hint), group (General)
i18n("If checked, hints will always been displayed. If this is not checked (default) \n"
"            hints will be displayed only per word when the user chooses to show the hint.");
// i18n: file: khangman.kcfg:61
// i18n: ectx: label, entry (hintHideTime), group (General)
i18n("This setting allows you to set in seconds how long KHangMan's hint bubble is shown.");
// i18n: file: khangman.kcfg:65
// i18n: ectx: label, entry (resolveTime), group (General)
i18n("This setting allows you to set in seconds how much time is available for resolving the word.");
// i18n: file: khangman.kcfg:69
// i18n: ectx: label, entry (MissedTimer), group (General)
i18n("Time during which the \"Already Guessed\" tooltip is displayed.");
// i18n: file: khangman.kcfg:76
// i18n: ectx: label, entry (AccentedLetters), group (Advanced)
i18n("Type accented letters separately from normal letters");
// i18n: file: khangman.kcfg:83
// i18n: ectx: label, entry (ProvidersUrl), group (KNewStuff)
i18n("The Providers path for KHangMan");

                KSharedConfigPtr globalConf = KSharedConfig::openConfig();
                QString userLanguage=globalConf->group("Locale").readEntry("Language", "en");
                if (userLanguage.isEmpty())
                userLanguage = "en";
                else
                //keep only the first 2 characters
                userLanguage = userLanguage.left(2);
            
