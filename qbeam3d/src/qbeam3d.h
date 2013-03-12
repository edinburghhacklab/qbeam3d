#ifndef QBEAM3D_H
#define QBEAM3D_H

#include <qmainwindow.h>

class QTextEdit;

class QBeam3D: public QMainWindow
{
    Q_OBJECT

public:
    QBeam3D();
    ~QBeam3D();

protected:
    void closeEvent( QCloseEvent* );

private slots:
    void newDoc();
    void choose();
    void load( const QString &fileName );
    void save();
    void saveAs();
    void print();

    void about();
    void aboutQt();

private:
    QPrinter *printer;
    QTextEdit *e;
    QString filename;
};


#endif
