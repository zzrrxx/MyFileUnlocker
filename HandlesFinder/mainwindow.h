#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void unlockClicked(bool checked);
    void unlockAllClicked(bool checked);
    void closeClicked(bool checked);

private:
    void RefreshHandles();
    bool UnlockHandle(QString pid, QString handle);
    QString InvokeCommand(QString cmd, QStringList args);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
