/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include <qcolordialog.h>
#include "mainwindow.h"

void SettingsDialog::loadSetting(QObject *obj, const QString &group)
{
  QString key = obj->name();
  if(obj->isA("QGroupBox")) {
    //Iterate over the groupBox's children
    const QObjectList *widgets = obj->children();
    if(!widgets) return;
    QObject *childObj;
    for(QObjectListIt it(*widgets); (childObj = it.current()) != NULL; ++it) {
      loadSetting(childObj, group);
    }
  } else if(obj->isA("QLineEdit")) {
    ((QLineEdit*)obj)->setText((gdata->settings).getString(group, key));
  } else if(obj->isA("QComboBox")) {
    ((QComboBox*)obj)->setCurrentText((gdata->settings).getString(group, key));
  } else if(obj->isA("QPushButton") && ((QPushButton*)obj)->isToggleButton()) {
    ((QPushButton*)obj)->setOn((gdata->settings).getBool(group, key));
  } else if(obj->isA("QCheckBox")) {
    ((QCheckBox*)obj)->setChecked((gdata->settings).getBool(group, key));
  } else if(obj->isA("QSpinBox")) {
    ((QSpinBox*)obj)->setValue((gdata->settings).getInt(group, key));
  } else if(obj->isA("QFrame")) {
    QColor color;
    color.setNamedColor((gdata->settings).getString(group, key));
    ((QFrame*)obj)->setPaletteBackgroundColor(color);
  }
}

void SettingsDialog::init()
{
  // Go through all the categories on the left, and insert all the preferences we can into the fields.
  // Combo boxes must be done separately.
  soundInput->insertStringList(AudioStream::getInputDeviceNames());
  soundOutput->insertStringList(AudioStream::getOutputDeviceNames());
  
  QString group;
  QObject *obj;
  const QObjectList *widgets;
  //Iterate over all groups
  for (uint i = 0; i < listBox->count(); i++) {
    //Iterate over all widgets in the current group and load their settings
    group = listBox->text(i);
    widgets = stack->widget(i)->children();
    if(widgets) {
      for(QObjectListIt it(*widgets); (obj = it.current()) != NULL; ++it) {
        loadSetting(obj, group);
      }
    }
  }

  #ifdef LINUX
  soundInput->setEditable(true);
  soundOutput->setEditable(true);
  #endif

  checkAnalysisEnabled();
}

QString SettingsDialog::getPath(const QString initialPath)
{
  return QFileDialog::getExistingDirectory(initialPath, this, "getDirectory", "Choose a directory", true, false);
}

/*
void SettingsDialog::changeOpenFolder()
{
  if(openFilesFolder) {
    QString path = getPath(openFilesFolder->text());
    if (path != "") openFilesFolder->setText(QDir::convertSeparators(path));
  }
}

void SettingsDialog::changeSaveFolder()
{
  if(saveFilesFolder) {
    QString path = getPath(saveFilesFolder->text());
    if (path != "") saveFilesFolder->setText(QDir::convertSeparators(path));
  }
}
*/
void SettingsDialog::changeTempFolder()
{
  if(tempFilesFolder) {
    QString path = getPath(tempFilesFolder->text());
    if (path != "") tempFilesFolder->setText(QDir::convertSeparators(path));
  }
}

/*
void SettingsDialog::openFilesTextChanged(const QString &s)
{
  if(bindOpenSaveFolders->isOn() && s != saveFilesFolder->text())
    saveFilesFolder->setText(s);
}

void SettingsDialog::saveFilesTextChanged(const QString &s)
{
  if(bindOpenSaveFolders->isOn() && openFilesFolder->text() != s)
    openFilesFolder->setText(s);
}

void SettingsDialog::toggleBindFolder(bool state)
{
  if(state) {
    bindOpenSaveFolders->setPixmap(QPixmap::fromMimeSource("chain_closed.png"));
    if(openFilesFolder->text().isEmpty()) {
      openFilesFolder->setText(saveFilesFolder->text());
    } else {
      saveFilesFolder->setText(openFilesFolder->text());
    }
  } else {
    bindOpenSaveFolders->setPixmap(QPixmap::fromMimeSource("chain_open.png"));
  }
}
*/
void SettingsDialog::fileNameChanged()
{
  QString filename;
  int digits = fileNumberOfDigits->value();
  if(digits == 0) {
    filename.sprintf("%s.wav", filenameGeneratingString->text().latin1());
  } else {
    filename.sprintf("%s%0*d.wav", filenameGeneratingString->text().latin1(), digits, fileGeneratingNumber->value());
  }
  filenameExample->setText(filename);
}

void SettingsDialog::getBackgroundColor()
{
  QColor color = QColorDialog::getColor(backgroundColor->paletteBackgroundColor());
  if(color.isValid()) {
    backgroundColor->setPaletteBackgroundColor(color);
  }
}

void SettingsDialog::getShading1Color()
{
  QColor color = QColorDialog::getColor(shading1Color->paletteBackgroundColor());
  if(color.isValid()) {
    shading1Color->setPaletteBackgroundColor(color);
  }
}

void SettingsDialog::getShading2Color()
{
  QColor color = QColorDialog::getColor(shading2Color->paletteBackgroundColor());
  if(color.isValid()) {
    shading2Color->setPaletteBackgroundColor(color);
  }
}

void SettingsDialog::saveSetting(QObject *obj, const QString group)
{
  QString key = obj->name();
  if(obj->isA("QGroupBox")) {
    //Iterate over the groupBox's children
    const QObjectList *widgets = obj->children();
    if(!widgets) return;
    QObject *childObj;
    for(QObjectListIt it(*widgets); (childObj = it.current()) != NULL; ++it) {
      saveSetting(childObj, group);
    }
  } else if(obj->isA("QLineEdit")) {
    (gdata->settings).setString(group, key, ((QLineEdit*)obj)->text());
  } else if(obj->isA("QComboBox")) {
    (gdata->settings).setString(group, key, ((QComboBox*)obj)->currentText());
  } else if(obj->isA("QPushButton") && ((QPushButton*)obj)->isToggleButton()) {
    (gdata->settings).setBool(group, key, ((QPushButton*)obj)->isOn());
  } else if(obj->isA("QCheckBox")) {
    (gdata->settings).setBool(group, key, ((QCheckBox*)obj)->isChecked());
  } else if(obj->isA("QSpinBox")) {
    (gdata->settings).setInt(group, key, ((QSpinBox*)obj)->value());
  } else if(obj->isA("QFrame")) {
    (gdata->settings).setString(group, key, ((QFrame*)obj)->paletteBackgroundColor().name());
  }
}

void SettingsDialog::saveSettings()
{
  // Go through all the categories on the left, and save all the preferences we can from the fields.
  // Combo boxes must be done separately.
  QString group;
  QObject *obj;
  const QObjectList *widgets;
  //Iterate over all the groups
  for(uint i = 0; i < listBox->count(); i++) {
    //Iterate over all widgets in the current group and save their settings
    group = listBox->text(i);
    widgets = stack->widget(i)->children();
    if(widgets) {
      for(QObjectListIt it(*widgets); (obj = it.current()) != NULL; ++it) {
        saveSetting(obj, group);
      }
    }
  }
  
  (gdata->settings).save();
	
  QApplication::postEvent(mainWindow, new QCustomEvent(SETTINGS_CHANGED));
	
  SettingsDialog::accept();

}

void SettingsDialog::checkAnalysisEnabled()
{
  QObjectList *widgets = stack->widget(0)->queryList("QComboBox", "noteRangeChoice", false, true);
  QComboBox *noteRangeChoice = (QComboBox*)widgets->first();
  myassert(noteRangeChoice);
  
  int choice = noteRangeChoice->currentItem();
  
  widgets = stack->widget(2)->queryList("QGroupBox", "bufferSizeGroupBox", false, true);
  QGroupBox *bufferSizeGroupBox = (QGroupBox*)widgets->first();
  myassert(bufferSizeGroupBox);
  widgets = stack->widget(2)->queryList("QGroupBox", "stepSizeGroupBox", false, true);
  QGroupBox *stepSizeGroupBox = (QGroupBox*)widgets->first();
  myassert(stepSizeGroupBox);
  
  if(choice == 0) {
    bufferSizeGroupBox->setEnabled(true);
    stepSizeGroupBox->setEnabled(true);
  } else {
    bufferSizeGroupBox->setEnabled(false);
    stepSizeGroupBox->setEnabled(false);
  }
}

void SettingsDialog::onNoteRangeChoice(int choice)
{
  QObjectList *widgets = stack->widget(2)->queryList("QSpinBox", "bufferSizeValue", false, true);
  QSpinBox *bufferSizeValue = (QSpinBox*)widgets->first();
  myassert(bufferSizeValue);
  widgets = stack->widget(2)->queryList("QComboBox", "bufferSizeUnit", false, true);
  QComboBox *bufferSizeUnit = (QComboBox*)widgets->first();
  myassert(bufferSizeUnit);
  widgets = stack->widget(2)->queryList("QCheckBox", "bufferSizeRound", false, true);
  QCheckBox *bufferSizeRound = (QCheckBox*)widgets->first();
  myassert(bufferSizeRound);
  widgets = stack->widget(2)->queryList("QSpinBox", "stepSizeValue", false, true);
  QSpinBox *stepSizeValue = (QSpinBox*)widgets->first();
  myassert(stepSizeValue);
  widgets = stack->widget(2)->queryList("QComboBox", "stepSizeUnit", false, true);
  QComboBox *stepSizeUnit = (QComboBox*)widgets->first();
  myassert(stepSizeUnit);
  widgets = stack->widget(2)->queryList("QCheckBox", "stepSizeRound", false, true);
  QCheckBox *stepSizeRound = (QCheckBox*)widgets->first();
  myassert(stepSizeRound);
  
  switch(choice) {
    case 1:
        bufferSizeValue->setValue(96);
        stepSizeValue->setValue(48);
      break;
    case 2:
        bufferSizeValue->setValue(48);
        stepSizeValue->setValue(24);
      break;
    case 3:
        bufferSizeValue->setValue(24);
        stepSizeValue->setValue(12);
      break;
    case 4:
        bufferSizeValue->setValue(12);
        stepSizeValue->setValue(6);
      break;
  }
  if(choice > 0) {
    bufferSizeUnit->setCurrentText("milli-seconds");
    stepSizeUnit->setCurrentText("milli-seconds");
    bufferSizeRound->setChecked(true);
    stepSizeRound->setChecked(true);
  }
  checkAnalysisEnabled();
}

void SettingsDialog::setDefaults(Settings *settings)
{
  settings->setBool("General", "bindOpenSaveFolders", true);
  settings->setString("General", "tempFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  settings->setString("General", "filenameGeneratingString", "Untitled");
  settings->setInt("General", "fileGeneratingNumber", 1);
  settings->setInt("General", "fileNumberOfDigits", 2);
  settings->setInt("General", "fastUpdateSpeed", 75);
  settings->setInt("General", "slowUpdateSpeed", 150);
  settings->setString("General", "noteRangeChoice", "Notes F1 and higher - Cello, Guitar, Bass Clarinet, General sounds ...");
  
  settings->setBool("View", "autoFollow", true);
  settings->setBool("View", "backgroundShading", true);

#ifndef LINUX
  settings->setString("Sound", "soundInput", "Default Input Device");   // Use default device
  settings->setString("Sound", "soundOutput", "Default Output Device"); // Use default device
#else
  settings->setString("Sound", "soundInput", "/dev/dsp");
  settings->setString("Sound", "soundOutput", "/dev/dsp");
#endif
  settings->setString("Sound", "numberOfChannels", "Mono");
  settings->setInt("Sound", "sampleRate", 44100);
  settings->setInt("Sound", "bitsPerSample", 16);
  settings->setBool("Sound", "muteOutput", true);
  
  settings->setInt("Analysis", "bufferSizeValue", 48);
  settings->setString("Analysis", "bufferSizeUnit", "milli-seconds");
  settings->setBool("Analysis", "bufferSizeRound", true);
  
  settings->setInt("Analysis", "stepSizeValue", 24);
  settings->setString("Analysis", "stepSizeUnit", "milli-seconds");
  settings->setBool("Analysis", "stepSizeRound", true);
  settings->setBool("Analysis", "doingHarmonicAnalysis", true);
  settings->setBool("Analysis", "doingFreqAnalysis", true);
  settings->setBool("Analysis", "doingEqualLoudness", true);
  settings->setString("Analysis", "analysisType", "MPM");
  settings->setInt("Analysis", "thresholdValue", 97);

  settings->setString("Colors", "backgroundColor", "#BBCDE2");
  settings->setString("Colors", "shading1Color", "#BBCDEF");
  settings->setString("Colors", "shading2Color", "#CBCDE2");
  settings->setBool("Colors", "displayBackgroundShading", true);
  
  settings->setBool("Dialogs", "rememberOpenFolder", true);
  settings->setString("Dialogs", "openFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  settings->setBool("Dialogs", "rememberSaveFolder", true);
  settings->setString("Dialogs", "saveFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  settings->setBool("Dialogs", "appendWav", true);
}
