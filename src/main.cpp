#include <QApplication>
#include <thread>
#include "DeviceBinder.h"
#include "windows/Interceptor.h"

extern DeviceBinder* g_pMainWin;

void keyboardCallback(const Keyboard& kb, const KEYSTATE state, const DeviceKey& key) {
    if (!g_pMainWin || !g_pMainWin->kbRunning) return;

    QString log = "";
    if (g_pMainWin->kbIntCheck->isChecked()) log += QString("Device Interface Name: %1 ").arg(kb.deviceInterfaceName);
    if (g_pMainWin->kbProdCheck->isChecked()) log += QString("Product Name: %1 ").arg(kb.productName);
    if (g_pMainWin->kbManCheck->isChecked()) log += QString("Manufacturer Name: %1 ").arg(kb.manufacturerName);
    if (g_pMainWin->kbIdCheck->isChecked()) log += QString("Device ID: %1 ").arg(kb.id);
    if (g_pMainWin->kbKeyCheck->isChecked()) log += QString("Key: %1 ").arg(key.names[0]);
    if (state == KEYSTATE::DOWN && g_pMainWin->kbDownCheck->isChecked()) log += QString("State: DOWN ");
    if (state == KEYSTATE::UP && g_pMainWin->kbUpCheck->isChecked()) log += QString("State: UP ");

    if (!log.isEmpty()) g_pMainWin->addLog(0, log);
}

void mouseCallback(const Mouse& m, const KEYSTATE state, const DeviceKey& key, int x, int y) {
    if (!g_pMainWin || !g_pMainWin->mRunning) return;
    bool addLine = false;
    QString log = "";
    if (g_pMainWin->mIntCheck->isChecked()) log += QString("Device Interface Name: %1 ").arg(m.deviceInterfaceName);
    if (g_pMainWin->mProdCheck->isChecked()) log += QString("Product Name: %1 ").arg(m.productName);
    if (g_pMainWin->mManCheck->isChecked()) log += QString("Manufacturer Name: %1 ").arg(m.manufacturerName);
    if (g_pMainWin->mIdCheck->isChecked()) log += QString("Device ID: %1 ").arg(m.id);
    if (g_pMainWin->mKeyCheck->isChecked()) log += QString("Key: %1 ").arg(key.names[0]);
    if (state == KEYSTATE::DOWN && g_pMainWin->mDownCheck->isChecked()) {
        log += QString("State: DOWN ");
        addLine = true;
    }
    if (state == KEYSTATE::UP && g_pMainWin->mUpCheck->isChecked()) { 
        log += QString("State: UP ");
        addLine = true;
    }
    if (g_pMainWin->mPosCheck->isChecked()) {
        if (g_pMainWin->mXCheck->isChecked()) {
            log += QString("X: %1 ").arg(x);
            addLine = true;
        }
        if (g_pMainWin->mYCheck->isChecked()) {
            log += QString("Y: %1 ").arg(y);
            addLine = true;
        }
    }

    if (!log.isEmpty() && addLine) g_pMainWin->addLog(1, log);
}
int main(int argc, char* argv[]) {
    Mapping mapping(std::filesystem::current_path().generic_string() + "/mapping.mapping");

    DeviceManager::getInstance().setMapping(mapping);
    QApplication app(argc, argv);

    DeviceBinder window(mapping);
    window.show();

    Interceptor interceptor;
    interceptor.keyboardGlobalInterceptors.push_back(keyboardCallback);
    interceptor.mouseGlobalInterceptors.push_back(mouseCallback);
    std::thread hardwareThread([&interceptor]() {
        interceptor.begin();
    });

    // 3. The program stays here while the window is open
    int result = app.exec();

    // 4. CLEANUP: This runs AFTER the window is closed
    interceptor.end();
    hardwareThread.join();

    return result;
}