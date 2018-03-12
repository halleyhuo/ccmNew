#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void BindSigSlot(void* sigSrc);

private:
    Ui::MainWindow *ui;
private slots:
    void FluoroModeOn();
    void SubModeOn();
    void RadModeOn();
    void ColHeightIn();
    void ColHeightOut();
    void ColWidthIn();
    void ColWidthOut();
    void ExposureInc();
    void ExposureDec();
    void FPSChange();
public slots:
    void widgetUpdateShow(ushort fun_id, ushort param, uchar status);



};

#endif // MAINWINDOW_H
