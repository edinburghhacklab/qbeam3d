/****************************************************************************
** Form interface generated from reading ui file 'ScannerControlWidget.ui'
**
** Created: Fr Okt 8 22:09:28 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SCANNERCONTROLWIDGET_H
#define SCANNERCONTROLWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QSpinBox;
class QPushButton;

class ScannerControlWidget : public QWidget
{
    Q_OBJECT

public:
    ScannerControlWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ScannerControlWidget();

    QGroupBox* m_laserControlBox;
    QLabel* m_layerLabel;
    QSpinBox* m_layerSpinBox;
    QPushButton* m_laserOnOffButton;
    QLabel* m_laserPositionLabel;
    QSpinBox* m_laserPositionSpinBox;
    QPushButton* m_repositionButton;
    QPushButton* m_laserResetButton;
    QGroupBox* m_tableControlBox;
    QSpinBox* m_tablePositionSpinBox;
    QLabel* m_tablePositionLabel;
    QPushButton* m_tableOnOffButton;
    QPushButton* m_tableResetButton;

protected:
    QVBoxLayout* ScannerControlWidgetLayout;
    QGridLayout* m_laserControlBoxLayout;
    QGridLayout* m_tableControlBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // SCANNERCONTROLWIDGET_H
