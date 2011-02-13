#include "debugform.h"
#include "ui_debugform.h"

#include "registerset.h"
#include "z80.h"


DebugForm::DebugForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugForm)
{
    ui->setupUi(this);
}

DebugForm::~DebugForm()
{
    delete ui;
}

void DebugForm::update()
{
    ui->regAFEdit->setText(QString::number(REGISTER_AF, 16));
    ui->regBCEdit->setText(QString::number(REGISTER_BC, 16));
    ui->regDEEdit->setText(QString::number(REGISTER_DE, 16));
    ui->regHLEdit->setText(QString::number(REGISTER_HL, 16));
    ui->regPCEdit->setText(QString::number(REGISTER_PC, 16));
}

void DebugForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
