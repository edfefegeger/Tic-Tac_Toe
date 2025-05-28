#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QTimer>
#include <QActionGroup>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class Player { None, Human, Computer };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *computerTimer;
    Player currentPlayer;
    QChar humanSymbol, computerSymbol;
    QColor humanColor, computerColor;
    int humanWins = 0, computerWins = 0;

    void initializeGame();
    void handleCellClick(int row, int col);
    void makeComputerMove();
    void updateTurnIndicator();
    bool checkWin(QChar symbol);
    bool checkDraw();
    void endGame(const QString &winner);
    void loadScore();
    void saveScore();
    void updateScoreDisplay();
    void switchPlayerSymbol(QChar symbol);
};
