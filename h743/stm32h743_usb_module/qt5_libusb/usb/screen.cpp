#include "screen.h"

struct libusb_device_handle* Screen::handle = nullptr;
libusb_context* Screen::ctx = nullptr;

Screen::Screen(QWidget* parent) : QDialog(parent) {
    comboUSB->addItem("SYSTEM DRIVER");
    comboUSB->addItem("libusb");
    connect(
        comboUSB,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &Screen::onComboUSB);
    connect(butExit, &QPushButton::clicked, [&] { close(); });
    connect(butConnect, &QPushButton::clicked, this, &Screen::onConnect);
    connect(butDisconnect, &QPushButton::clicked, this, &Screen::onDisconnect);
    connect(butWrite, &QPushButton::clicked, this, &Screen::onWrite);
    connect(butRead, &QPushButton::clicked, this, &Screen::onRead);
    layV->addWidget(comboUSB);
    layV->addWidget(butConnect);
    layV->addWidget(butDisconnect);
    layV->addWidget(labFreq);
    layV->addWidget(lineFreq);
    layV->addWidget(butWrite);
    layV->addWidget(butRead);
    layV->addWidget(butExit);
    setLayout(layV);

    IsLibUSB = false;
    butConnect->setVisible(false);
    butDisconnect->setVisible(false);
    butRead->setVisible(false);

    connect(timer, &QTimer::timeout, this, &Screen::timeout);
}

Screen::~Screen() {}

void Screen::onComboUSB(int index) {
    if (!index) {
        IsLibUSB = false;
        butConnect->setVisible(false);
        butDisconnect->setVisible(false);
        butRead->setVisible(false);
    } else {
        IsLibUSB = true;
        butConnect->setVisible(true);
        butDisconnect->setVisible(true);
        butRead->setVisible(true);
    }
}

void Screen::onConnect() {
    /////////   USB     ////////////
    libusb_init(&ctx); // инициализируем библиотеку
    handle =
        libusb_open_device_with_vid_pid(ctx, VID, PID); // открываем устройство
    if (!handle) {
        perror("Device could not be open or found");
        libusb_exit(ctx);
        qDebug() << "Error in open usb";
        close();
    }
    libusb_claim_interface(handle, 0); // запрашиваем интерфейс 0 для себя
    IsConnected = true;
    qDebug() << "Connected";
}

void Screen::onDisconnect() {
    if (IsConnected) {
        libusb_release_interface(handle, 0); // отпускаем интерфейс 0
        libusb_close(handle); // закрываем устройство
        libusb_exit(ctx); // завершаем работу с библиотекой
        IsConnected = false;
        qDebug() << "Disconnected";
    }
}

void Screen::onWrite() {
    if (IsLibUSB) {
        unsigned char buf[64] = {1, 1, 1}; // массив данных
        uint16_t freq = lineFreq->text().toUShort();
        buf[1] = freq >> 8;
        buf[2] = freq & 0xFF;
        // передаём 10 байт из массива buf на endpoint 1 (этот ep должен быть
        // типа bulk)
        libusb_bulk_transfer(handle, 0x01, buf, 3, &transfered, 1000);
        qDebug() << "transfered=" << transfered;
    } else {
        std::ofstream file;
        file.open("/dev/beep1", std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            qDebug() << "no file";
            return;
        }
        // std::cout.rdbuf(file.rdbuf()); //redirect cout, trouble with flush
        uint16_t freq = lineFreq->text().toUShort();
        char buf1 = freq >> 8;
        char buf2 = freq & 0xFF;
        char buf[3] = {1, buf1, buf2};
        file.write(buf, 3);
        std::flush(file);
        file.close();
    }
}

void Screen::onRead() {
    unsigned char buf[64] = {0}; // массив данных
    int res = libusb_bulk_transfer(handle, (unsigned char)0x81, buf,
                                   sizeof(buf), &transfered, 1000);
    if (res != 0) {
        qDebug() << libusb_error_name(res);
        return;
    }
    qDebug() << "received=" << transfered;
}

void Screen::timeout() { writeFreq(Freq); }

void Screen::keyPressEvent(QKeyEvent* event) {
    int key = event->key(); // event->key() - целочисленный код клавиши
    switch (key) {
    case Qt::Key_1:
        Freq = 32;
        Key_1_pressed = true;
        break;
    case Qt::Key_2:
        Freq = 37;
        Key_2_pressed = true;
        break;
    case Qt::Key_3:
        Freq = 41;
        Key_3_pressed = true;
        break;
    case Qt::Key_4:
        Freq = 44;
        Key_4_pressed = true;
        break;
    case Qt::Key_5:
        Freq = 49;
        Key_5_pressed = true;
        break;
    case Qt::Key_6:
        Freq = 55;
        Key_6_pressed = true;
        break;
    case Qt::Key_7:
        Freq = 62;
        Key_7_pressed = true;
        break;
    case Qt::Key_8:
        Freq = 65;
        Key_8_pressed = true;
        break;
    case Qt::Key_9:
        Freq = 74;
        Key_9_pressed = true;
        break;
    }
    if (key >= Qt::Key_1 && key <= Qt::Key_9) {
        timer->start(10);
    }
}

void Screen::keyReleaseEvent(QKeyEvent* event) {
    int key = event->key(); // event->key() - целочисленный код клавиши

    switch (key) {
    case Qt::Key_1:
        Key_1_pressed = false;
        break;
    case Qt::Key_2:
        Key_2_pressed = false;
        break;
    case Qt::Key_3:
        Key_3_pressed = false;
        break;
    case Qt::Key_4:
        Key_4_pressed = false;
        break;
    case Qt::Key_5:
        Key_5_pressed = false;
        break;
    case Qt::Key_6:
        Key_6_pressed = false;
        break;
    case Qt::Key_7:
        Key_7_pressed = false;
        break;
    case Qt::Key_8:
        Key_8_pressed = false;
        break;
    case Qt::Key_9:
        Key_9_pressed = false;
        break;
    }
    bool res = Key_1_pressed + Key_2_pressed + Key_3_pressed + Key_4_pressed +
               Key_5_pressed + Key_6_pressed + Key_7_pressed + +Key_8_pressed +
               Key_9_pressed;
    if (Key_1_pressed) {
        Freq = 32;
    } else if (Key_2_pressed) {
        Freq = 37;
    } else if (Key_3_pressed) {
        Freq = 41;
    } else if (Key_4_pressed) {
        Freq = 44;
    } else if (Key_5_pressed) {
        Freq = 49;
    } else if (Key_6_pressed) {
        Freq = 55;
    } else if (Key_7_pressed) {
        Freq = 62;
    } else if (Key_8_pressed) {
        Freq = 65;
    } else if (Key_9_pressed) {
        Freq = 74;
    }
    if (!res) {
        timer->stop();
    }
}

void Screen::writeFreq(uint16_t freq) {
    if (IsLibUSB) {
        unsigned char buf[64] = {1, 1, 1}; // массив данных
        buf[1] = freq >> 8;
        buf[2] = freq & 0xFF;
        // передаём 10 байт из массива buf на endpoint 1 (этот ep должен быть
        // типа bulk)
        libusb_bulk_transfer(handle, 0x01, buf, 3, &transfered, 1000);
        qDebug() << "transfered=" << transfered;
    } else {
        std::ofstream file;
        file.open("/dev/beep1", std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            qDebug() << "no file";
            return;
        }
        // std::cout.rdbuf(file.rdbuf()); //redirect cout, trouble with flush
        char buf1 = freq >> 8;
        char buf2 = freq & 0xFF;
        char buf[3] = {1, buf1, buf2};
        file.write(buf, 3);
        std::flush(file);
        file.close();
    }
}
