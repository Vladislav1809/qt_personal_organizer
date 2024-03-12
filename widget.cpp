#include "widget.h"
#include "./ui_widget.h"
#include <QListWidgetItem>
#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QDate>
#include <QMessageBox>
#include <QFile>
#include <QUuid>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QFile file("data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this,tr("Ошибка"),tr("Возникла ошибка при чтении файла"));
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(line);
        ui->taskListWidget->addItem(item);
    }
    file.close();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_createTaskButton_clicked()
{
    QDialog *d = new QDialog();
    QVBoxLayout *vbox = new QVBoxLayout();

    QComboBox *comboBox = new QComboBox();
    comboBox->addItems(QStringList() << "Высокий" << "Нормальный" << "Низкий");

    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("Введите описание задачи");

    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setMinimumDate(QDate::currentDate());

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), d, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), d, SLOT(reject()));

    vbox->addWidget(comboBox);
    vbox->addWidget(calendar);
    vbox->addWidget(lineEdit);
    vbox->addWidget(buttonBox);

    d->setLayout(vbox);

    if (d->exec() == QDialog::Accepted) {
        QString selectedPriority = comboBox->currentText();
        QString taskDescription = lineEdit->text();
        QDate selectedDate = calendar->selectedDate();

        QListWidgetItem *item = new QListWidgetItem;

        QString itemText;

        QUuid uuid = QUuid::createUuid();


        itemText.append("Идентификатор задачи: " + uuid.toString());
        itemText.append(" Приоритет: " + selectedPriority);
        itemText.append(" Дата выполнения: " + selectedDate.toString("dd.MM.yyyy"));
        itemText.append(" Описание задачи: ");

        if (!taskDescription.isEmpty()) {
            itemText.append(taskDescription);
        } else {
            itemText.append("без описания");
        }


        QFile file("data.txt");
        if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, tr("Ошибка"),tr("Возникла ошибка при сохранении"));
            return;
        }

        item->setText(itemText);
        ui->taskListWidget->addItem(item);


        QTextStream in(&file);
        in << "";

        for (int row = 0; row < ui->taskListWidget->count(); row++) {
            QListWidgetItem *item = ui->taskListWidget->item(row);
            QTextStream out(&file);
            out << item->text() << "\n";
        }

        file.close();
    }

}


void Widget::on_detailsTaskButton_clicked()
{
    if (ui->taskListWidget->selectedItems().isEmpty()) {
        QMessageBox::information(this, tr("Ошибка"), "Не удалось получить текущий элемент");
        return;
    }

    QListWidgetItem *item = ui->taskListWidget->item(ui->taskListWidget->currentRow());
    QMessageBox::information(this,tr("Получение текущего элемента"), item->text());

}


void Widget::on_deleteTaskButton_clicked()
{
    if (ui->taskListWidget->selectedItems().isEmpty()) {
        QMessageBox::information(this, tr("Ошибка"), "Не удалось получить текущий элемент");
        return;
    }


    ui->taskListWidget->takeItem(ui->taskListWidget->currentRow());

    QFile file("data.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this,tr("Ошибка"),tr("Не могу прочитать data.txt"));
        return;
    }

    QTextStream in(&file);
    in << "";

    for (int row = 0; row < ui->taskListWidget->count(); row++) {
        QListWidgetItem *item = ui->taskListWidget->item(row);
        QTextStream out(&file);
        out << item->text() << "\n";
    }

    file.close();

}

