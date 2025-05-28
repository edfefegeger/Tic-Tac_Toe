#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gameTable->setRowCount(3);
    ui->gameTable->setColumnCount(3);
    ui->gameTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->gameTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->gameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->gameTable->setSelectionMode(QAbstractItemView::NoSelection);

    computerTimer = new QTimer(this);
    computerTimer->setSingleShot(true);
    connect(computerTimer, &QTimer::timeout, this, &MainWindow::makeComputerMove);

    connect(ui->gameTable, &QTableWidget::cellClicked, this, &MainWindow::handleCellClick);

    QActionGroup *group = new QActionGroup(this);
    group->addAction(ui->actionPlay_X);
    group->addAction(ui->actionPlay_O);
    ui->actionPlay_X->setCheckable(true);
    ui->actionPlay_O->setCheckable(true);
    connect(group, &QActionGroup::triggered, this, [=](QAction *action){
        switchPlayerSymbol(action == ui->actionPlay_X ? 'X' : 'O');
        initializeGame();
    });

    loadScore();
    switchPlayerSymbol('X'); // по умолчанию
    updateScoreDisplay();
    initializeGame();
}

void MainWindow::initializeGame()
{
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 3; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setFont(QFont("Arial", 24, QFont::Bold));
            ui->gameTable->setItem(row, col, item);
        }

    static bool alternateStart = true;
    currentPlayer = alternateStart ? Player::Human : Player::Computer;
    alternateStart = !alternateStart;

    updateTurnIndicator();

    if (currentPlayer == Player::Computer)
        computerTimer->start(1500);
}

void MainWindow::handleCellClick(int row, int col)
{
    if (currentPlayer != Player::Human || !ui->gameTable->item(row, col)->text().isEmpty()) return;

    ui->gameTable->item(row, col)->setText(humanSymbol);
    ui->gameTable->item(row, col)->setTextColor(humanColor);

    if (checkWin(humanSymbol)) {
        endGame("Вы победили!");
        return;
    } else if (checkDraw()) {
        endGame("Ничья!");
        return;
    }

    currentPlayer = Player::Computer;
    updateTurnIndicator();
    computerTimer->start(1500);
}

void MainWindow::makeComputerMove()
{
    QVector<QPair<int, int>> emptyCells;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (ui->gameTable->item(r, c)->text().isEmpty())
                emptyCells.append({r, c});

    if (!emptyCells.isEmpty()) {
        auto move = emptyCells[qrand() % emptyCells.size()];
        int r = move.first, c = move.second;
        ui->gameTable->item(r, c)->setText(computerSymbol);
        ui->gameTable->item(r, c)->setTextColor(computerColor);

        if (checkWin(computerSymbol)) {
            endGame("Компьютер победил!");
            return;
        } else if (checkDraw()) {
            endGame("Ничья!");
            return;
        }

        currentPlayer = Player::Human;
        updateTurnIndicator();
    }
}

bool MainWindow::checkWin(QChar symbol)
{
    for (int i = 0; i < 3; ++i) {
        if (ui->gameTable->item(i,0)->text() == symbol &&
            ui->gameTable->item(i,1)->text() == symbol &&
            ui->gameTable->item(i,2)->text() == symbol) return true;
        if (ui->gameTable->item(0,i)->text() == symbol &&
            ui->gameTable->item(1,i)->text() == symbol &&
            ui->gameTable->item(2,i)->text() == symbol) return true;
    }
    if (ui->gameTable->item(0,0)->text() == symbol &&
        ui->gameTable->item(1,1)->text() == symbol &&
        ui->gameTable->item(2,2)->text() == symbol) return true;
    if (ui->gameTable->item(0,2)->text() == symbol &&
        ui->gameTable->item(1,1)->text() == symbol &&
        ui->gameTable->item(2,0)->text() == symbol) return true;

    return false;
}

bool MainWindow::checkDraw()
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (ui->gameTable->item(r, c)->text().isEmpty())
                return false;
    return true;
}

void MainWindow::endGame(const QString &winner)
{
    if (winner.contains("Вы победили")) ++humanWins;
    else if (winner.contains("Компьютер")) ++computerWins;

    saveScore();
    updateScoreDisplay();
    QMessageBox::information(this, "Результат", winner);
    initializeGame();
}

void MainWindow::updateTurnIndicator()
{
    QString who = currentPlayer == Player::Human ? QString(humanSymbol) : QString(computerSymbol);
    ui->turnLabel->setText("Ход игрока: " + who);

    QColor color = (currentPlayer == Player::Human) ? humanColor : computerColor;
    QPalette palette = ui->turnLabel->palette();
    palette.setColor(QPalette::Window, color.lighter(150));
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->turnLabel->setAutoFillBackground(true);
    ui->turnLabel->setPalette(palette);
}


void MainWindow::updateScoreDisplay()
{
    ui->scoreLabel->setText(QString("Счет: Игрок %1 : %2 Компьютер").arg(humanWins).arg(computerWins));
}

void MainWindow::switchPlayerSymbol(QChar symbol)
{
    humanSymbol = symbol;
    computerSymbol = (symbol == 'X') ? 'O' : 'X';
    humanColor = Qt::blue;
    computerColor = Qt::red;
}

void MainWindow::loadScore()
{
    QFile file("score.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in >> humanWins >> computerWins;
        file.close();
    }
}

void MainWindow::saveScore()
{
    QFile file("score.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << humanWins << ' ' << computerWins;
        file.close();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
