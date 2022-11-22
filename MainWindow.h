#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "Board.h"
#include "Net.h"
#include "AIGame.h"
#include "AskNetworkModelDialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void on_pushButton_board_pressed();

    void on_pushButton_ai_pressed();

    void on_pushButton_net_pressed();

private:
    Ui::MainWindow *ui;
    Board* board;

    AskNetworkModelDialog* dialog;

protected slots:
    void dealGameover();
    void dealNetGame(QString ip);
    void dealLinked();
};
#endif //MainWindow.h
