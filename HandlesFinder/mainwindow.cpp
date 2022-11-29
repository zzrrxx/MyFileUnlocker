#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QProcess>
#include <QMessageBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    RefreshHandles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::unlockClicked(bool checked) {
    QItemSelectionModel *selectionModel = ui->tableWidget->selectionModel();
    foreach(QModelIndex index, selectionModel->selectedRows()) {
        QString pid = ((QLabel*)ui->tableWidget->cellWidget(index.row(), 1))->text();
        QString handle = ((QLabel*)ui->tableWidget->cellWidget(index.row(), 2))->text();
        if (!UnlockHandle(pid, handle)) {
            break;
        }
    }
    RefreshHandles();
}
void MainWindow::unlockAllClicked(bool checked) {
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        QString pid = ui->tableWidget->item(row, 1)->text();
        QString handle = ui->tableWidget->item(row, 2)->text();
        if (!UnlockHandle(pid, handle)) {
            break;
        }
    }
    RefreshHandles();
}
void MainWindow::closeClicked(bool checked) {
    this->close();
}


void MainWindow::RefreshHandles() {
    ui->tableWidget->clearContents();

    ui->btnUnlock->setEnabled(false);
    ui->btnUnlockAll->setEnabled(false);

    connect(ui->btnUnlock, SIGNAL(clicked(bool)), this, SLOT(unlockClicked(bool)));
    connect(ui->btnUnlockAll, SIGNAL(clicked(bool)), this, SLOT(unlockAllClicked(bool)));
    connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(closeClicked(bool)));

    QString result = InvokeCommand("E:\\Tools\\Sysinternals\\handle64.exe", QStringList() << "E:\\Tools" << "-v" << "-g");
    QString resultStartMarker = "Process,PID,Type,Handle,Name";
    int idx = result.indexOf(resultStartMarker);
    if (idx > 0) {
        result = result.remove(0, idx + resultStartMarker.length());
        QStringList lines = result.split("\r\n", Qt::SkipEmptyParts);
        ui->tableWidget->setRowCount(lines.count());
        for (int i = 0; i < lines.length(); i++) {
            QStringList comps = lines[i].split(",", Qt::SkipEmptyParts);
            ui->tableWidget->setCellWidget(i, 0, new QLabel(comps[0]));
            ui->tableWidget->setCellWidget(i, 1, new QLabel(comps[1]));
            ui->tableWidget->setCellWidget(i, 2, new QLabel(comps[3]));
            ui->tableWidget->setCellWidget(i, 3, new QLabel(comps[4]));
            ui->tableWidget->setCellWidget(i, 4, new QLabel(comps[5]));
        }
        ui->tableWidget->resizeColumnsToContents();
        if (lines.count() > 0) {
            ui->btnUnlock->setEnabled(true);
            ui->btnUnlockAll->setEnabled(true);
        }
    }
}
bool MainWindow::UnlockHandle(QString pid, QString handle) {
    QStringList args;
    args << " -c " << handle << " -p " << pid << " -y ";
    QString result = InvokeCommand("E:\\Tools\\Sysinternals\\handle64.exe", args);
    if (result.indexOf("Handle closed.") >= 0) {
        return true;
    } else {
        QMessageBox::critical(this, "Error", result);
        return false;
    }
}
QString MainWindow::InvokeCommand(QString cmd, QStringList args) {
    QProcess proc(this);
    proc.start(cmd, args);

    if (!proc.waitForStarted(5000)) {
        QMessageBox::critical(this, "Error", "Faild to invoke handle");
        return "";
    }
    proc.closeWriteChannel();

    if (!proc.waitForFinished()) {
        QMessageBox::critical(this, "Error", "Faild to invoke handle");
        return "";
    }

    return QString(proc.readAll());
}
