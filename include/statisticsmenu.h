#ifndef STATISTICSMENU_H
#define STATISTICSMENU_H

#include <QDialog>

namespace Ui {
class StatisticsMenu;
}

class StatisticsMenu : public QDialog {
    Q_OBJECT

public:
    explicit StatisticsMenu(QWidget *parent = nullptr);
    ~StatisticsMenu();

    void add_dashboard_link(std::string dash_link);

    void increment_aluminium();
    void increment_plastic();
    void increment_glass();
    void increment_other();

private slots:
    void on_pushButton_clicked();

private:
    Ui::StatisticsMenu *ui;
    std::string _dash_link;

    float overall_counter;
    int aluminium_counter;
    int plastic_counter;
    int glass_counter;
    int other_counter;

    void increment_overall_counter();
};

#endif // STATISTICSMENU_H
