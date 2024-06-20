#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "promocodewidget.h"
#include <QRandomGenerator>
#include <QCryptographicHash>
#include "Promocode.h"
#include <openssl/evp.h>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    promocodes_grid(new QGridLayout)
{
    ui->setupUi(this);
    ui->verticalLayout_3->setAlignment(Qt::AlignHCenter);

    QWidget *containerWidget = new QWidget(this);
    containerWidget->setLayout(promocodes_grid);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);

    ui->verticalLayout_2->layout()->addWidget(scrollArea);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_login_button_clicked()
{
    this->key = QCryptographicHash::hash(
                         ui->password_input->text().toUtf8(),
                         QCryptographicHash::Sha256).toHex();

    if (ui->password_input->text().toStdString() == RIGHT_PASSWORD) {
        ui->error_text->setText("");
        ui->stackedWidget->setCurrentIndex(1);
        generate_promocodes(10);
    } else {
        ui->error_text->setText("Неверный пароль!");
    }
}


void MainWindow::generate_promocodes(int count) {
    for (int i = 0; i < count; i++) {
        generate_promocode(i);
    }
}

void MainWindow::generate_promocode(int index) {
    QString generatedPlainCode = generate_random_string(4);
    QByteArray generatedEncryptedCode;
    encrypt_code(this->key, generatedPlainCode.toUtf8(), generatedEncryptedCode);

    QString encodedEncryptedCode = QString(generatedEncryptedCode.toHex());

    const auto promocode = new Promocode(encodedEncryptedCode);
    auto promocodeUi = new PromocodeWidget();
    this->invisiblePromocodeIndexes.push_back(index);
    int row = index / 2;
    int col = index % 2;
    promocodes_grid->addWidget(promocodeUi, row, col);
    this->promocodes.push_back(promocode);
}

QString MainWindow::generate_random_string(int length = 4) {
    const QString possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString randomString;

    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        randomString.append(possibleCharacters.at(index));
    }

    return randomString;
}

void MainWindow::on_pushButton_clicked()
{
    if (invisiblePromocodeIndexes.empty()) {
        qWarning() << "No invisible promocodes available.";
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(invisiblePromocodeIndexes.size());
    int promocodeIndex = invisiblePromocodeIndexes.at(randomIndex);
    PromocodeWidget* promocodeWidget = qobject_cast<PromocodeWidget*>(promocodes_grid->itemAt(promocodeIndex)->widget());

    QString encryptedPromoCode = promocodes.at(promocodeIndex)->code;
    QByteArray decryptedPromoCode;
    qDebug() << encryptedPromoCode;
    decrypt_code(this->key, QByteArray::fromHex(QByteArray::fromStdString(encryptedPromoCode.toStdString())), decryptedPromoCode);
    promocodeWidget->setPromocodeText(QString(decryptedPromoCode));
    invisiblePromocodeIndexes.erase(std::next(invisiblePromocodeIndexes.begin(), randomIndex), std::next(invisiblePromocodeIndexes.begin(), randomIndex + 1));
    generate_promocode(this->promocodes.size());
}

int MainWindow::decrypt_code(const QByteArray& key_hex, const QByteArray & encryptedBytes, QByteArray & decryptedBytes) {
    QByteArray key_ba = QByteArray::fromHex(key_hex);
    unsigned char key[32] = {0};
    memcpy(key, key_ba.data(), 32);

    QByteArray iv_hex("00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
    QByteArray iv_ba = QByteArray::fromHex(iv_hex);
    unsigned char iv[16] = {0};
    memcpy(iv, iv_ba.data(), 16);


    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        qDebug() << "*** EVP_DecryptInit_ex2() ERROR";
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

#define BUF_LEN 256
    unsigned char encrypted_buf[BUF_LEN] = {0}, decrypted_buf[BUF_LEN] = {0};
    int encr_len, decr_len;
    QDataStream encrypted_stream(encryptedBytes);
    decryptedBytes.clear();

    QBuffer decrypted_buffer(&decryptedBytes);
    decrypted_buffer.open(QBuffer::WriteOnly);
    encr_len = encrypted_stream.readRawData(reinterpret_cast<char*>(encrypted_buf), BUF_LEN);
    while(encr_len > 0) {
        if (!EVP_DecryptUpdate(ctx, decrypted_buf, &decr_len, encrypted_buf, encr_len)) {
            /* Error */
            decrypted_buffer.close();
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }
        decrypted_buffer.write(reinterpret_cast<char*>(decrypted_buf), decr_len);
        encr_len = encrypted_stream.readRawData(reinterpret_cast<char*>(encrypted_buf), BUF_LEN);
    }

    int tmplen;
    if (!EVP_DecryptFinal_ex(ctx, decrypted_buf, &tmplen)) {
        qDebug() << "*** EVP_DecryptFinal_ex() ERROR";
        decrypted_buffer.close();
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    decrypted_buffer.write(reinterpret_cast<char*>(decrypted_buf), tmplen);
    decrypted_buffer.close();
    EVP_CIPHER_CTX_free(ctx);

    return 0;
}

int MainWindow::encrypt_code(const QByteArray& key_hex, const QByteArray & plainBytes, QByteArray & encryptedBytes) {
    QByteArray key_ba = QByteArray::fromHex(key_hex);
    unsigned char key[32] = {0};
    memcpy(key, key_ba.data(), key_ba.size());

    QByteArray iv_hex("00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
    QByteArray iv_ba = QByteArray::fromHex(iv_hex);
    unsigned char iv[16] = {0};
    memcpy(iv, iv_ba.data(), iv_ba.size());

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        qDebug() << "*** EVP_EncryptInit_ex() ERROR";
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

#define BUF_LEN 256
    unsigned char plain_buf[BUF_LEN] = {0}, encrypted_buf[BUF_LEN + EVP_MAX_BLOCK_LENGTH] = {0};
    int plain_len, encr_len;
    QDataStream plain_stream(plainBytes);
    encryptedBytes.clear();

    QBuffer encrypted_buffer(&encryptedBytes);
    encrypted_buffer.open(QBuffer::WriteOnly);

    while((plain_len = plain_stream.readRawData(reinterpret_cast<char*>(plain_buf), BUF_LEN)) > 0) {
        if (!EVP_EncryptUpdate(ctx, encrypted_buf, &encr_len, plain_buf, plain_len)) {
            /* Error */
            encrypted_buffer.close();
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }
        encrypted_buffer.write(reinterpret_cast<char*>(encrypted_buf), encr_len);
    }

    if (!EVP_EncryptFinal_ex(ctx, encrypted_buf, &encr_len)) {
        qDebug() << "*** EVP_EncryptFinal_ex() ERROR";
        encrypted_buffer.close();
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    encrypted_buffer.write(reinterpret_cast<char*>(encrypted_buf), encr_len);
    encrypted_buffer.close();
    EVP_CIPHER_CTX_free(ctx);

    return 0;
}
