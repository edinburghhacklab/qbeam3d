/****************************************************************************
** Form interface generated from reading ui file 'ScanProgressDialog.ui'
**
** Created: Fr Okt 8 22:09:28 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SCANPROGRESSDIALOG_H
#define SCANPROGRESSDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QFrame;
class QProgressBar;
class QPushButton;

class ScanProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ScanProgressDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ScanProgressDialog();

    QGroupBox* groupBox1;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLabel* textLabel1_2;
    QLabel* m_intTimeLabel;
    QLabel* m_thresholdLabel;
    QLabel* m_contrastLabel;
    QFrame* line2;
    QLabel* m_totalScanLineLabel;
    QLabel* m_currentScanLineLabel;
    QLabel* m_currentScanLine;
    QLabel* m_totalScanLines;
    QFrame* line2_2;
    QLabel* m_elapsedTime;
    QLabel* textLabel1;
    QLabel* m_estimatedTimeLabel;
    QLabel* m_timeLabel;
    QLabel* m_endTime;
    QLabel* m_startTime;
    QLabel* m_totalTime;
    QLabel* m_timeElapsedLabel;
    QProgressBar* m_totalProgressBar;
    QPushButton* m_cancelButton;

protected:
    QVBoxLayout* ScanProgressDialogLayout;
    QGridLayout* groupBox1Layout;
    QGridLayout* layout4;
    QGridLayout* layout1;

protected slots:
    virtual void languageChange();

};

#endif // SCANPROGRESSDIALOG_H
