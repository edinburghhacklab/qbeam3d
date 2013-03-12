/****************************************************************************
** Form interface generated from reading ui file 'ScannerSettingsWidget.ui'
**
** Created: Fr Okt 8 22:09:28 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SCANNERSETTINGSWIDGET_H
#define SCANNERSETTINGSWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QComboBox;
class QGroupBox;
class QToolButton;
class QSpinBox;
class QFrame;
class QCheckBox;

class ScannerSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    ScannerSettingsWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ScannerSettingsWidget();

    QLabel* m_scanModeLabel;
    QLabel* m_contrastModeLabel;
    QComboBox* m_scanModeComboBox;
    QComboBox* m_contrastModeComboBox;
    QGroupBox* m_laserGroup;
    QToolButton* m_actualPosToLowerButton;
    QSpinBox* m_upperBoundSpinBox;
    QSpinBox* m_lowerBoundSpinBox;
    QToolButton* m_actualPosToUpperButton;
    QLabel* m_lowerBoundLabel_2;
    QLabel* m_upperBoundLabel_2;
    QFrame* line4_2;
    QLabel* textLabel3;
    QLabel* m_objectHeightLabel;
    QFrame* line5_3;
    QLabel* m_laserStepLabel_2;
    QSpinBox* m_laserStepSpinBox;
    QSpinBox* m_laserResSpinBox;
    QLabel* m_laserStepLabel;
    QGroupBox* m_tableGroup;
    QFrame* m_tableScanRangeDisplay;
    QSpinBox* m_tableStepSpinBox;
    QSpinBox* m_tableRangeSpinBox;
    QLabel* m_tableStepLabel;
    QLabel* m_bereichLabel;
    QLabel* m_tableResLabel;
    QSpinBox* m_tableResSpinBox;
    QGroupBox* m_meshGroup;
    QCheckBox* m_createCCD1Phantom;
    QCheckBox* m_createCCD2Phantom;
    QCheckBox* m_errorCorrectionCheckBox;
    QCheckBox* m_closeHolesCheckBox;
    QCheckBox* m_createLidCheckBox;
    QCheckBox* m_createBottomCheckBox;
    QSpinBox* m_borderHoleSpinBox;
    QSpinBox* m_closeHoleSpinBox;
    QLabel* textLabel2;
    QCheckBox* m_smoothBorderCheckBox;
    QLabel* textLabel1;

protected:
    QVBoxLayout* ScannerSettingsWidgetLayout;
    QGridLayout* layout16;
    QVBoxLayout* m_laserGroupLayout;
    QGridLayout* layout9;
    QHBoxLayout* layout10;
    QGridLayout* layout13;
    QHBoxLayout* m_tableGroupLayout;
    QGridLayout* layout17;
    QGridLayout* m_meshGroupLayout;

protected slots:
    virtual void languageChange();

};

#endif // SCANNERSETTINGSWIDGET_H
