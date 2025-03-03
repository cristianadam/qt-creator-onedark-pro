#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <extensionsystem/iplugin.h>
#include <utils/filepath.h>
#include <utils/fileutils.h>

using namespace Core;
using namespace Utils;

namespace OneDarkPro::Internal {

class OneDarkProPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "OneDarkPro.json")

public:
    void initialize() final
    {
        FilePath oneDarkProXml = FilePath::fromString(":/onedark-pro.xml");
        FilePath oneDarkProTheme = FilePath::fromString(":/onedark-pro.creatortheme");
        FilePath oneDarkProTokens = FilePath::fromString(":/onedark-pro.figmatokens");

        FilePath stylesDir = ICore::userResourcePath("styles");
        FilePath themesDir = ICore::userResourcePath("themes");

        struct FileToDir
        {
            FilePath file;
            FilePath dir;
        };
        QVector<FileToDir> fileToDir{{oneDarkProXml, stylesDir},
                                     {oneDarkProTheme, themesDir},
                                     {oneDarkProTokens, themesDir}};

        // "Install" the theme and text color files
        for (const auto &pair : fileToDir) {
            if (!pair.dir.exists())
                pair.dir.createDir();
            auto destFile = pair.dir.pathAppended(pair.file.fileName());
            FileUtils::copyIfDifferent(pair.file, destFile);
        }

        auto settings = ICore::settings();
        const QString themeName = oneDarkProTheme.baseName();
        if (settings->value(Constants::SETTINGS_THEME) != themeName) {
            // Set the theme and text color scheme as default
            settings->setValue(Constants::SETTINGS_THEME, themeName);

            const char schemeFileNamesKey[] = "TextEditor/ColorSchemes";
            auto schemeFileNames = settings->value(schemeFileNamesKey).toMap();
            schemeFileNames.insert(themeName, oneDarkProXml.toSettings());
            settings->setValue(schemeFileNamesKey, schemeFileNames);

            ICore::askForRestart(tr("The OneDark Pro theme change will take effect after restart."));
        }
    }
};

} // namespace OneDarkPro::Internal

#include <onedark-pro.moc>
