#ifndef SCREEN_H
#define SCREEN_H

#include <fstream>
#include <libusb-1.0/libusb.h>

#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

class Screen : public QDialog {
    Q_OBJECT

  public:
    Screen(QWidget* parent = nullptr);
    ~Screen();

  private slots:
    void onComboUSB(int index);
    void onConnect();
    void onDisconnect();
    void onWrite();
    void onRead();
    void timeout();

  protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

  private:
    QVBoxLayout* layV = new QVBoxLayout(this);
    QComboBox* comboUSB = new QComboBox(this);
    QPushButton* butConnect = new QPushButton("Connect", this);
    QPushButton* butDisconnect = new QPushButton("Disconnect", this);
    QLabel* labFreq = new QLabel("frequency, Hz", this);
    QLineEdit* lineFreq = new QLineEdit(this);
    QPushButton* butWrite = new QPushButton("Write", this);
    QPushButton* butRead = new QPushButton("Read", this);
    QPushButton* butExit = new QPushButton("Exit", this);

    bool IsLibUSB = false;
    bool IsConnected = false;
    int transfered = 0;

    static struct libusb_device_handle* handle;
    static libusb_context* ctx;

    void writeFreq(uint16_t freq);

    uint16_t VID = 0x0483;
    // uint16_t PID = 0x3752;
    uint16_t PID = 0xDEAD;

    QTimer* timer = new QTimer(this);
    uint16_t Freq = 0;
    bool Key_1_pressed = false;
    bool Key_2_pressed = false;
    bool Key_3_pressed = false;
    bool Key_4_pressed = false;
    bool Key_5_pressed = false;
    bool Key_6_pressed = false;
    bool Key_7_pressed = false;
    bool Key_8_pressed = false;
    bool Key_9_pressed = false;
};
#endif // SCREEN_H
