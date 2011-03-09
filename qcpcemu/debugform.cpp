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
    ui->regAFEdit->setText(QString("%1").arg(REGISTER_AF, 4, 16, QLatin1Char('0')));
    ui->regBCEdit->setText(QString("%1").arg(REGISTER_BC, 4, 16, QLatin1Char('0')));
    ui->regDEEdit->setText(QString("%1").arg(REGISTER_DE, 4, 16, QLatin1Char('0')));
    ui->regHLEdit->setText(QString("%1").arg(REGISTER_HL, 4, 16, QLatin1Char('0')));

    ui->regAF1Edit->setText(QString("%1").arg(REGISTER_AF1, 4, 16, QLatin1Char('0')));
    ui->regBC1Edit->setText(QString("%1").arg(REGISTER_BC1, 4, 16, QLatin1Char('0')));
    ui->regDE1Edit->setText(QString("%1").arg(REGISTER_DE1, 4, 16, QLatin1Char('0')));
    ui->regHL1Edit->setText(QString("%1").arg(REGISTER_HL1, 4, 16, QLatin1Char('0')));

    ui->regIEdit->setText(QString("%1").arg(REGISTER_I, 2, 16, QLatin1Char('0')));
    ui->regREdit->setText(QString("%1").arg(REGISTER_R, 2, 16, QLatin1Char('0')));

    ui->regIXEdit->setText(QString("%1").arg(REGISTER_IX, 4, 16, QLatin1Char('0')));
    ui->regIYEdit->setText(QString("%1").arg(REGISTER_IY, 4, 16, QLatin1Char('0')));

    ui->regPCEdit->setText(QString("%1").arg(REGISTER_PC, 4, 16, QLatin1Char('0')));
    ui->regSPEdit->setText(QString("%1").arg(REGISTER_SP, 4, 16, QLatin1Char('0')));

    ui->regIFF1Edit->setText(QString("%1").arg(RegisterSet::IFF1, 2, 16, QLatin1Char('0')));
    ui->regIFF2Edit->setText(QString("%1").arg(RegisterSet::IFF2, 2, 16, QLatin1Char('0')));
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
