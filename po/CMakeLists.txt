find_package(Gettext REQUIRED)
file(GLOB _po_files *.po)
GETTEXT_CREATE_TRANSLATIONS(blade-menu-plugin.pot ALL ${_po_files})
