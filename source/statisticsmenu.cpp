#include "statisticsmenu.h"
#include "ui_statisticsmenu.h"

#include <string>

StatisticsMenu::StatisticsMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatisticsMenu)
{
    ui->setupUi(this);

    overall_counter = 0;
    aluminium_counter = 0;
    plastic_counter = 0;
    glass_counter = 0;
    other_counter = 0;
}

StatisticsMenu::~StatisticsMenu() {
    delete ui;
}

void StatisticsMenu::add_dashboard_link(std::string dash_link) {
    _dash_link = std::move(dash_link);
}

void StatisticsMenu::increment_overall_counter() {
    ui->scan_count_label->setText(std::to_string(int(++overall_counter)).c_str());

    ui->aluminium_progressbar->setValue(aluminium_counter/overall_counter * 100 + 0.5);
    ui->plastic_progressbar->setValue(plastic_counter/overall_counter * 100 + 0.5);
    ui->glass_progressbar->setValue(glass_counter/overall_counter * 100 + 0.5);
    ui->other_progressbar->setValue(other_counter/overall_counter * 100 + 0.5);
}
void StatisticsMenu::increment_aluminium() {
    ++aluminium_counter;
    increment_overall_counter();
}
void StatisticsMenu::increment_plastic() {
    ++plastic_counter;
    increment_overall_counter();
}
void StatisticsMenu::increment_glass() {
    ++glass_counter;
    increment_overall_counter();
}
void StatisticsMenu::increment_other() {
    ++other_counter;
    increment_overall_counter();
}

void StatisticsMenu::on_pushButton_clicked() {
    if(_dash_link.empty()) {
        return;
    }

    #if defined(__linux__)
        const std::string open_dash("xdg-open \"" + _dash_link);
        system(open_dash.c_str());
    #endif
}
