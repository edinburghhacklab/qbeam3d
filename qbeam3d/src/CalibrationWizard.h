/****************************************************************************
** Form interface generated from reading ui file 'CalibrationWizard.ui'
**
** Created: Fr Okt 8 22:09:28 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CALIBRATIONWIZARD_H
#define CALIBRATIONWIZARD_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwizard.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class Plane2DView;
class QWidget;
class QLabel;
class QCheckBox;
class QWidgetStack;
class QLCDNumber;
class QToolButton;
class QButtonGroup;
class QRadioButton;
class QGroupBox;
class QPushButton;
class QLineEdit;

class CalibrationWizard : public QWizard
{
    Q_OBJECT

public:
    CalibrationWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CalibrationWizard();

    QWidget* m_introductionPage;
    QLabel* m_introductionText;
    QCheckBox* m_precalibCheckBox;
    QLabel* m_calibrateOriginText;
    QWidget* m_preCalibrationPage;
    QWidgetStack* m_originTextStack;
    QWidget* m_page1_1;
    QLabel* m_originStepText1;
    QWidget* m_page2_1;
    QLabel* m_originStepText2;
    QWidget* m_page3_1;
    QLabel* m_originStepText3;
    QWidget* m_page4_1;
    QLabel* m_originStepText4;
    QLCDNumber* m_laserPositionLCD;
    QToolButton* m_down;
    QToolButton* m_fineDown;
    QToolButton* m_up;
    QToolButton* m_fineUp;
    QToolButton* m_5down;
    QToolButton* m_5up;
    QButtonGroup* m_layerGroup;
    QCheckBox* m_use7CheckBox;
    QRadioButton* m_radio2;
    QCheckBox* m_use5CheckBox;
    QRadioButton* m_radio1;
    QRadioButton* m_radio3;
    QRadioButton* m_radio6;
    QCheckBox* m_use4CheckBox;
    QRadioButton* m_radio5;
    QRadioButton* m_radio4;
    QRadioButton* m_radio7;
    QCheckBox* m_use2CheckBox;
    QCheckBox* m_use3CheckBox;
    QCheckBox* m_use6CheckBox;
    QCheckBox* m_use1CheckBox;
    QLabel* m_lcd1;
    QLabel* m_lcd2;
    QLabel* m_lcd3;
    QLabel* m_lcd4;
    QLabel* m_lcd5;
    QLabel* m_lcd6;
    QLabel* m_lcd7;
    QToolButton* m_trackLayerButton;
    QToolButton* m_previousLayerButton;
    QGroupBox* groupBox1;
    QLabel* m_distanceLabel;
    QLabel* m_distanceIntLabel;
    QGroupBox* groupBox1_2;
    QLabel* m_angleLabel;
    QLabel* m_angleIntLabel;
    QWidget* m_ccdCalibrationPage;
    Plane2DView* m_calibrationPlotter;
    QWidgetStack* m_ccdStack;
    QWidget* m_ccdText1;
    QLabel* m_ccdText1_2;
    QWidget* m_ccdText2;
    QLabel* m_ccdText2_2;
    QPushButton* m_calibrateButton;
    QPushButton* m_optimizeButton;
    QGroupBox* m_rightGroupBox;
    QLabel* m_averageLabelRight;
    QLineEdit* m_rightAverage;
    QLabel* m_maxLabelRight;
    QLineEdit* m_rightMax;
    QGroupBox* m_leftGroupBox;
    QLabel* m_averageLabelLeft;
    QLineEdit* m_leftAverage;
    QLabel* m_maxLabelLeft;
    QLineEdit* m_leftMax;

protected:
    QHBoxLayout* m_introductionPageLayout;
    QVBoxLayout* layout6;
    QHBoxLayout* m_preCalibrationPageLayout;
    QVBoxLayout* m_page1_1Layout;
    QVBoxLayout* m_page2_1Layout;
    QVBoxLayout* m_page3_1Layout;
    QVBoxLayout* m_page4_1Layout;
    QVBoxLayout* layout15;
    QSpacerItem* spacer3;
    QHBoxLayout* layout10;
    QGridLayout* buttonLayout;
    QHBoxLayout* layout14;
    QGridLayout* m_layerGroupLayout;
    QVBoxLayout* layout13;
    QVBoxLayout* groupBox1Layout;
    QVBoxLayout* groupBox1_2Layout;
    QVBoxLayout* m_ccdCalibrationPageLayout;
    QHBoxLayout* layout11;
    QVBoxLayout* m_ccdText1Layout;
    QVBoxLayout* m_ccdText2Layout;
    QVBoxLayout* layout10_2;
    QHBoxLayout* layout9;
    QVBoxLayout* m_rightGroupBoxLayout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // CALIBRATIONWIZARD_H
