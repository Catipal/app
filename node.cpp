#include "node.h"
#include "./ui_node.h"

NODE::NODE(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NODE)
{
    ui->setupUi(this);
}

NODE::~NODE()
{
    delete ui;
}


void NODE::on_pushButton_2_clicked()
{
    // --- 1. Retrieve input texts from the text edits ---
    QString keyText   = ui->textEdit_4->toPlainText().trimmed();
    QString valueText = ui->textEdit_3->toPlainText().trimmed();

    // If either field is empty, do nothing.
    if (keyText.isEmpty() || valueText.isEmpty())
        return;

    // --- 2. Retrieve placeholders and build a combined key string ---
    // For example, if textEdit_4 has placeholder "Key:" and the user enters "Apple",
    // then fullKey becomes "Key: Apple".
    QString keyPlaceholder   = ui->textEdit_4->placeholderText().trimmed();
    QString valuePlaceholder = ui->textEdit_3->placeholderText().trimmed();
    QString fullKey = keyPlaceholder + " " + keyText;

    // --- 3. Convert valueText to an integer ---
    bool conversionOk = false;
    int inputValue = valueText.toInt(&conversionOk);
    if (!conversionOk)
        return;  // Optionally show an error message

    // --- 4. Check listWidget_2 (for adding/summing) ---
    QListWidgetItem *itemList2 = nullptr;
    int count2 = ui->listWidget_2->count();
    for (int i = 0; i < count2; i++) {
        QListWidgetItem *item = ui->listWidget_2->item(i);
        // We assume the item's text is formatted as:
        // "fullKey: valuePlaceholder <number>"
        if (item->text().startsWith(fullKey + ":"))
        {
            itemList2 = item;
            break;
        }
    }

    if (itemList2) {
        // Item exists in listWidget_2: add the input value.
        int currentValue = itemList2->data(Qt::UserRole).toInt();
        int newValue = currentValue + inputValue;
        if (newValue > 0) {
            itemList2->setData(Qt::UserRole, newValue);
            itemList2->setText(fullKey + ": " + valuePlaceholder + " " + QString::number(newValue));
        } else {
            // If the accumulated value becomes zero or less, remove the item.
            int row = ui->listWidget_2->row(itemList2);
            delete ui->listWidget_2->takeItem(row);
        }
        return;  // Done
    }

    // --- 5. If no matching item in listWidget_2, check listWidget_1 (for removal) ---
    QListWidgetItem *itemList1 = nullptr;
    int count1 = ui->listWidget_1->count();
    for (int i = 0; i < count1; i++) {
        QListWidgetItem *item = ui->listWidget_1->item(i);
        if (item->text().startsWith(fullKey + ":"))
        {
            itemList1 = item;
            break;
        }
    }

    if (itemList1) {
        // Item exists in listWidget_1: subtract (remove) the input value.
        int currentValue = itemList1->data(Qt::UserRole).toInt();
        int newValue = currentValue - inputValue;
        if (newValue > 0) {
            itemList1->setData(Qt::UserRole, newValue);
            itemList1->setText(fullKey + ": " + valuePlaceholder + " " + QString::number(newValue));
        } else {
            int row = ui->listWidget_1->row(itemList1);
            delete ui->listWidget_1->takeItem(row);
        }
        return;  // Done
    }

    // --- 6. If the key is not found in either list, create a new item in listWidget_2 ---
    QListWidgetItem *newItem = new QListWidgetItem(
        fullKey + ": " + valuePlaceholder + " " + QString::number(inputValue)
        );
    newItem->setData(Qt::UserRole, inputValue);
    ui->listWidget_2->addItem(newItem);
}


void NODE::on_pushButton_clicked()
{
    // --- 1. Retrieve input texts from the text edits ---
    QString keyText   = ui->textEdit_2->toPlainText().trimmed();
    QString valueText = ui->textEdit_1->toPlainText().trimmed();

    // If either field is empty, do nothing.
    if (keyText.isEmpty() || valueText.isEmpty())
        return;

    // --- 2. Retrieve placeholders and build a combined key string ---
    // For example, if textEdit_2 has placeholder "Key:" and the user enters "Apple",
    // then fullKey becomes "Key: Apple".
    QString keyPlaceholder   = ui->textEdit_2->placeholderText().trimmed();
    QString valuePlaceholder = ui->textEdit_1->placeholderText().trimmed();
    QString fullKey = keyPlaceholder + " " + keyText;

    // --- 3. Convert valueText to an integer ---
    bool conversionOk = false;
    int inputValue = valueText.toInt(&conversionOk);
    if (!conversionOk)
        return;  // Optionally show an error message

    // --- 4. Check listWidget_1 (for adding/summing) ---
    QListWidgetItem *itemList1 = nullptr;
    int count2 = ui->listWidget_1->count();
    for (int i = 0; i < count2; i++) {
        QListWidgetItem *item = ui->listWidget_1->item(i);
        // We assume the item's text is formatted as:
        // "fullKey: valuePlaceholder <number>"
        if (item->text().startsWith(fullKey + ":"))
        {
            itemList1 = item;
            break;
        }
    }

    if (itemList1) {
        // Item exists in listWidget_1: add the input value.
        int currentValue = itemList1->data(Qt::UserRole).toInt();
        int newValue = currentValue + inputValue;
        if (newValue > 0) {
            itemList1->setData(Qt::UserRole, newValue);
            itemList1->setText(fullKey + ": " + valuePlaceholder + " " + QString::number(newValue));
        } else {
            // If the accumulated value becomes zero or less, remove the item.
            int row = ui->listWidget_1->row(itemList1);
            delete ui->listWidget_1->takeItem(row);
        }
        return;  // Done
    }

    // --- 5. If no matching item in listWidget_2, check listWidget_1 (for removal) ---
    QListWidgetItem *itemList2 = nullptr;
    int count1 = ui->listWidget_2->count();
    for (int i = 0; i < count1; i++) {
        QListWidgetItem *item = ui->listWidget_2->item(i);
        if (item->text().startsWith(fullKey + ":"))
        {
            itemList2 = item;
            break;
        }
    }

    if (itemList2) {
        // Item exists in listWidget_2: subtract (remove) the input value.
        int currentValue = itemList2->data(Qt::UserRole).toInt();
        int newValue = currentValue - inputValue;
        if (newValue > 0) {
            itemList2->setData(Qt::UserRole, newValue);
            itemList2->setText(fullKey + ": " + valuePlaceholder + " " + QString::number(newValue));
        } else {
            int row = ui->listWidget_2->row(itemList2);
            delete ui->listWidget_2->takeItem(row);
        }
        return;  // Done
    }

    // --- 6. If the key is not found in either list, create a new item in listWidget_1 ---
    QListWidgetItem *newItem = new QListWidgetItem(
        fullKey + ": " + valuePlaceholder + " " + QString::number(inputValue)
        );
    newItem->setData(Qt::UserRole, inputValue);
    ui->listWidget_1->addItem(newItem);
}


void NODE::on_pushButton_3_clicked()
{
    // 1. Retrieve the sell amount from textEdit_5.
    QString sellValueText = ui->textEdit_5->toPlainText().trimmed();
    if (sellValueText.isEmpty())
        return;

    bool conversionOk = false;
    int sellValue = sellValueText.toInt(&conversionOk);
    if (!conversionOk)
        return;  // Optionally, display an error if conversion fails.

    // 2. Check if there is at least one item in listWidget_1.
    if (ui->listWidget_1->count() == 0)
        return; // Nothing to sell from.

    // 3. For this example, operate on the first item in listWidget_1.
    QListWidgetItem *item = ui->listWidget_1->item(0);

    // 4. Retrieve the current stored value (assumed stored in Qt::UserRole).
    int currentValue = item->data(Qt::UserRole).toInt();

    // 5. Subtract the sell value.
    int newValue = currentValue - sellValue;

    // 6. If the new value is greater than zero, update the item; otherwise, remove it.
    if (newValue > 0) {
        item->setData(Qt::UserRole, newValue);
        // Update the displayed text.
        // We assume that the text format is something like:
        // "Key: <userKey>: Value: <number>"
        // To update just the number, we can try to find the last space and replace the trailing number.
        QString currentText = item->text();
        int lastSpace = currentText.lastIndexOf(" ");
        if (lastSpace != -1) {
            QString prefix = currentText.left(lastSpace + 1);  // includes the space
            item->setText(prefix + QString::number(newValue));
        } else {
            // Fallback if the format isn't as expected.
            item->setText(QString::number(newValue));
        }
    } else {
        // If newValue is 0 or negative, remove the item from listWidget_1.
        int row = ui->listWidget_1->row(item);
        delete ui->listWidget_1->takeItem(row);
    }
}


void NODE::on_pushButton_4_clicked()
{
    // 1. Get the buy amount from textEdit_5.
    QString buyAmountText = ui->textEdit_5->toPlainText().trimmed();
    if (buyAmountText.isEmpty())
        return;

    bool conversionOk = false;
    int buyAmount = buyAmountText.toInt(&conversionOk);
    if (!conversionOk)
        return; // Optionally, you might display an error message here.

    // 2. Ensure there is at least one item in listWidget_2.
    if (ui->listWidget_2->count() == 0)
        return; // Nothing to subtract from.

    // 3. For this example, operate on the first item in listWidget_2.
    QListWidgetItem *item = ui->listWidget_2->item(0);

    // 4. Retrieve the current stored value.
    int currentValue = item->data(Qt::UserRole).toInt();

    // Subtract the buy amount.
    int newValue = currentValue - buyAmount;

    // 5. If the new value is positive, update the item.
    if (newValue > 0) {
        item->setData(Qt::UserRole, newValue);
        // Update the displayed text.
        // Here we assume that the item’s text is in a format ending with the numeric value.
        QString currentText = item->text();
        int lastSpace = currentText.lastIndexOf(" ");
        if (lastSpace != -1)
            item->setText(currentText.left(lastSpace + 1) + QString::number(newValue));
        else
            item->setText(QString::number(newValue));
    } else {
        // If the new value is zero or negative, remove the item.
        int row = ui->listWidget_2->row(item);
        delete ui->listWidget_2->takeItem(row);
    }
}

