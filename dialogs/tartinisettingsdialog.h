/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#ifndef TARTINISETTINGSDIALOG_H
#define TARTINISETTINGSDIALOG_H

#include <QColorDialog>
#include "mainwindow.h"
#include <QDialog>
#include "ui_settingsdialog.h"
#include <QSettings>
#include <map>

class GData;

class TartiniSettingsDialog : public QDialog, private Ui_SettingsDialog
{
  Q_OBJECT

  public:
    TartiniSettingsDialog(QWidget * p_parent = 0);

    void loadSetting( QObject * p_object
                    , const std::string & p_group
                    );
    void init();

  public slots:

    QString getPath(const QString p_initial_path);
    void changeTempFolder();
    void fileNameChanged();
    void getBackgroundColor();
    void getShading1Color();
    void getShading2Color();
    void saveSetting( QObject * p_object
                    , const std::string & p_group
                    );
    void saveSettings();
    void checkAnalysisEnabled();
    void onNoteRangeChoice(int p_choice);
    static void setUnknownsToDefault(GData & p_gdata);
    void resetDefaults();

  private:

    /**
     * Store tab names to avoid issues due to memory corruption that change tab
     * names when calling save settings
     */
    static std::map<int, std::string> m_tab_names;
};

#endif // TARTINISETTINGSDIALOG_H
// EOF
