#include "debugform.h"
#include "ui_debugform.h"

#include "registerset.h"
#include "z80.h"


DebugForm::DebugForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugForm)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT(setButtonClicked()));
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
    ui->regSPEdit->setText(QString::number(REGISTER_SP, 16));

    ui->regAF1Edit->setText(QString::number(REGISTER_AF1, 16));
    ui->regBC1Edit->setText(QString::number(REGISTER_BC1, 16));
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

void DebugForm::setButtonClicked()
{
    if (!ui->breakpointEdit->text().isEmpty())
    {
        quint16 address = ui->breakpointEdit->text().toInt(0, 16);
        emit setBreakpoint(address);
        ui->breakpointEdit->clear();
    }
}
