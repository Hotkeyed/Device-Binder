#ifndef DEVICEBINDER_H
#define DEVICEBINDER_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>

// Assuming these are your existing non-GUI headers
#include <device/DeviceManager.h>
#include <windows/Interceptor.h>

class DeviceBinder : public QMainWindow {
    Q_OBJECT

public:
    DeviceBinder(QWidget* parent = nullptr);
    ~DeviceBinder();

    // These public methods allow the interceptor callbacks to send data to the UI
    void postKeyboardLog(const QString& text);
    void postMouseLog(const QString& text);

signals:
    // Internal signal to bridge threads safely
    void logMessageReceived(QTextEdit* target, QString message);

private slots:
    void handleSaveMapping();
    void handleApplyMapping();
    void handleExportKeyboardLog();
    void handleExportMouseLog();
    void toggleKeyboardLog();
    void toggleMouseLog();
    void clearKeyboardLog();
    void clearMouseLog();
    void updateMouseOptionsState();

    // Thread-safe UI updater
    void appendToLog(QTextEdit* target, QString message);

private:
    void setupUi();
    void createDeviceTab();
    void createKeyboardTab();
    void createMouseTab();

    QTabWidget* tabs;

    // Device Tab
    QTableWidget* deviceTable;

    // Keyboard Tab
    QTextEdit* keyboardLogText;
    QCheckBox* kbInterfaceCheck, * kbProductCheck, * kbManufacturerCheck, * kbIdCheck, * kbKeyCheck, * kbDownCheck, * kbUpCheck;
    QPushButton* kbStartPauseBtn;
    bool kbRunning = true;

    // Mouse Tab
    QTextEdit* mouseLogText;
    QCheckBox* mInterfaceCheck, * mProductCheck, * mManufacturerCheck, * mIdCheck, * mKeyCheck, * mDownCheck, * mUpCheck;
    QCheckBox* mPosCheck, * mXCheck, * mYCheck;
    QPushButton* mStartPauseBtn;
    bool mRunning = true;
};

#endif