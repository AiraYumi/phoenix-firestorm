; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Turkish.nlf"

; Language selection dialog
LangString InstallerLanguageTitle  ${LANG_TURKISH} "Yükleyici Dili"
LangString SelectInstallerLanguage  ${LANG_TURKISH} "Lütfen yükleyicinin dilini seçin"

; subtitle on license text caption
LangString LicenseSubTitleUpdate ${LANG_TURKISH} "Güncelleştir"
LangString LicenseSubTitleSetup ${LANG_TURKISH} "Ayarlar"

LangString MULTIUSER_TEXT_INSTALLMODE_TITLE ${LANG_TURKISH} "Yükleme Modu"
LangString MULTIUSER_TEXT_INSTALLMODE_SUBTITLE ${LANG_TURKISH} "Tüm kullanıcılar (Yönetici gerektirir) için mi yoksa sadece geçerli kullanıcı için mi yüklensin?"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_TOP ${LANG_TURKISH} "Bu yükleyiciyi Yönetici ayrıcalıkları ile çalıştırdığınızda, (ör.) c:\Program Files veya geçerli kullanıcının AppData\Local klasörüne yüklemeyi tercih edebilirsiniz."
LangString MULTIUSER_INNERTEXT_INSTALLMODE_ALLUSERS ${LANG_TURKISH} "Tüm kullanıcılar için yükle"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_CURRENTUSER ${LANG_TURKISH} "Sadece geçerli kullanıcı için yükle"

; installation directory text
LangString DirectoryChooseTitle ${LANG_TURKISH} "Yükleme Dizini" 
LangString DirectoryChooseUpdate ${LANG_TURKISH} "${VERSION_LONG}.(XXX) sürümüne güncelleştirme yapmak için Firestorm dizinini seçin:"
LangString DirectoryChooseSetup ${LANG_TURKISH} "Firestorm'ın yükleneceği dizini seçin:"

LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_TURKISH} "Yükleme Dizini"
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_TURKISH} "Firestorm'ın yükleneceği dizini seçin:"

LangString MUI_TEXT_INSTALLING_TITLE ${LANG_TURKISH} "Firestorm yükleniyor..."
LangString MUI_TEXT_INSTALLING_SUBTITLE ${LANG_TURKISH} "Firestorm görüntüleyici $INSTDIR dizinine yükleniyor"

LangString MUI_TEXT_FINISH_TITLE ${LANG_TURKISH} "Firestorm yükleniyor"
LangString MUI_TEXT_FINISH_SUBTITLE ${LANG_TURKISH} "Firestorm görüntüleyici $INSTDIR dizinine yüklendi"

LangString MUI_TEXT_ABORT_TITLE ${LANG_TURKISH} "Yükleme Durduruldu"
LangString MUI_TEXT_ABORT_SUBTITLE ${LANG_TURKISH} "Firestorm görüntüleyici $INSTDIR dizinine yüklenmiyor"

; CheckStartupParams message box
LangString CheckStartupParamsMB ${LANG_TURKISH} "'$INSTNAME' programı bulunamadı. Sessiz güncelleştirme başarılamadı."

; installation success dialog
LangString InstSuccesssQuestion ${LANG_TURKISH} "Firestorm şimdi başlatılsın mı?"

; remove old NSIS version
LangString RemoveOldNSISVersion ${LANG_TURKISH} "Eski sürüm kontrol ediliyor..."

; check windows version
LangString CheckWindowsVersionDP ${LANG_TURKISH} "Windows sürümü kontrol ediliyor..."
LangString CheckWindowsVersionMB ${LANG_TURKISH} "Firestorm sadece Windows Vista SP2."
LangString CheckWindowsServPackMB ${LANG_TURKISH} "It is recomended to run Firestorm on the latest service pack for your operating system.$\nThis will help with performance and stability of the program."
LangString UseLatestServPackDP ${LANG_TURKISH} "Please use Windows Update to install the latest Service Pack."

; checkifadministrator function (install)
LangString CheckAdministratorInstDP ${LANG_TURKISH} "Yükleme izni kontrol ediliyor..."
LangString CheckAdministratorInstMB ${LANG_TURKISH} "'Sınırlı' bir hesap kullanıyor görünüyorsunuz.$\nFirestorm'ı yüklemek için bir 'yönetici' olmalısınız."

; checkifadministrator function (uninstall)
LangString CheckAdministratorUnInstDP ${LANG_TURKISH} "Kaldırma izni kontrol ediliyor..."
LangString CheckAdministratorUnInstMB ${LANG_TURKISH} "'Sınırlı' bir hesap kullanıyor görünüyorsunuz.$\nFirestorm'ı kaldırmak için bir 'yönetici' olmalısınız."

; checkcpuflags
LangString MissingSSE2 ${LANG_TURKISH} "Bu makinede SSE2 desteğine sahip bir CPU bulunmayabilir, Firestorm ${VERSION_LONG} çalıştırmak için bu gereklidir. Devam etmek istiyor musunuz?"

; Extended cpu checks (AVX2)
LangString MissingAVX2 ${LANG_TURKISH} "Your CPU does not support AVX2 instructions. Please download the legacy CPU version from %DLURL%-legacy-cpus/"
LangString AVX2Available ${LANG_TURKISH} "Your CPU supports AVX2 instructions. You can download the AVX2 optimized version for better performance from %DLURL%/. Do you want to download it now?"
LangString AVX2OverrideConfirmation ${LANG_TURKISH} "If you believe that your PC can support AVX2 optimization, you may install anyway. Do you want to proceed?"
LangString AVX2OverrideNote ${LANG_TURKISH} "By overriding the installer, you are about to install a version that might crash immediately after launching. If this happens, please immediately install the standard CPU version."

; closesecondlife function (install)
LangString CloseSecondLifeInstDP ${LANG_TURKISH} "Firestorm'ın kapatılması bekleniyor..."
LangString CloseSecondLifeInstMB ${LANG_TURKISH} "Firestorm zaten çalışırken kapatılamaz.$\n$\nYaptığınız işi bitirdikten sonra Firestorm'ı kapatmak ve devam etmek için Tamam seçimini yapın.$\nYüklemeyi iptal etmek için İPTAL seçimini yapın."
LangString CloseSecondLifeInstRM ${LANG_TURKISH} "Firestorm failed to remove some files from a previous install."

; closesecondlife function (uninstall)
LangString CloseSecondLifeUnInstDP ${LANG_TURKISH} "Firestorm'ın kapatılması bekleniyor..."
LangString CloseSecondLifeUnInstMB ${LANG_TURKISH} "Firestorm zaten çalışırken kaldırılamaz.$\n$\nYaptığınız işi bitirdikten sonra Firestorm'ı kapatmak ve devam etmek için Tamam seçimini yapın.$\nİptal etmek için İPTAL seçimini yapın."

; CheckNetworkConnection
LangString CheckNetworkConnectionDP ${LANG_TURKISH} "Ağ bağlantısı kontrol ediliyor..."

; error during installation
LangString ErrorSecondLifeInstallRetry ${LANG_TURKISH} "Firestorm installer encountered issues during installation. Some files may not have been copied correctly."
LangString ErrorSecondLifeInstallSupport ${LANG_TURKISH} "Please reinstall viewer from https://www.firestormviewer.org/downloads/ and contact https://www.firestormviewer.org/support/ if issue persists after reinstall."

; ask to remove user's data files
LangString RemoveDataFilesMB ${LANG_TURKISH} "Belgeler ve Ayarlar klasöründeki önbellek dosyaları siliniyor?"

; delete program files
LangString DeleteProgramFilesMB ${LANG_TURKISH} "Firestorm program dizininizde hala dosyalar var.$\n$\nBunlar muhtemelen sizin oluşturduğunuz veya şuraya taşıdığınız dosyalar:$\n$INSTDIR$\n$\nBunları kaldırmak istiyor musunuz?"

; uninstall text
LangString UninstallTextMsg ${LANG_TURKISH} "Bu adımla Firestorm ${VERSION_LONG} sisteminizden kaldırılacaktır."

; ask to remove registry keys that still might be needed by other viewers that are installed
LangString DeleteRegistryKeysMB ${LANG_TURKISH} "Do you want to remove application registry keys?$\n$\nIt is recomended to keep registry keys if you have other versions of Firestorm installed."

; <FS:Ansariel> Ask to create protocol handler registry entries
LangString CreateUrlRegistryEntries ${LANG_TURKISH} "Do you want to register Firestorm as default handler for virtual world protocols?$\n$\nIf you have other versions of Firestorm installed, this will overwrite the existing registry keys."

; <FS:Ansariel> Optional start menu entry
LangString CreateStartMenuEntry ${LANG_TURKISH} "Create an entry in the start menu?"

; <FS:Ansariel> Application name suffix for OpenSim variant
LangString ForOpenSimSuffix ${LANG_TURKISH} "for OpenSimulator"

LangString DeleteDocumentAndSettingsDP ${LANG_TURKISH} "Deleting files in Documents and Settings folder."
LangString UnChatlogsNoticeMB ${LANG_TURKISH} "This uninstall will NOT delete your Firestorm chat logs and other private files. If you want to do that yourself, delete the Firestorm folder within your user Application data folder."
LangString UnRemovePasswordsDP ${LANG_TURKISH} "Removing Firestorm saved passwords."

LangString MUI_TEXT_LICENSE_TITLE ${LANG_TURKISH} "Vivox Voice System License Agreement"
LangString MUI_TEXT_LICENSE_SUBTITLE ${LANG_TURKISH} "Additional license agreement for the Vivox Voice System components."
LangString MUI_INNERTEXT_LICENSE_TOP ${LANG_TURKISH} "Please read the following license agreement carefully before proceeding with the installation:"
LangString MUI_INNERTEXT_LICENSE_BOTTOM ${LANG_TURKISH} "You have to agree to the license terms to continue with the installation."
