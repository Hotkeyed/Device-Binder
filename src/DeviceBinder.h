#ifndef DEVICEBINDER_H
#define DEVICEBINDER_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTableWidget>
#include <QCheckBox>
#include <QTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include "NumericTreeWidget.h" //
#include "device/DeviceManager.h" //
#include "device/Mapping.h" //

class DeviceBinder : public QMainWindow {
    Q_OBJECT

public:
    DeviceBinder(Mapping& mappingRef,QWidget* parent = nullptr);
    ~DeviceBinder();

    // Checkboxes from your Win32 code
    QCheckBox* kbIdCheck, * kbProdCheck, * kbManCheck, * kbIntCheck, * kbKeyCheck, * kbDownCheck, * kbUpCheck;
    QCheckBox* mIdCheck, * mProdCheck, * mManCheck, * mIntCheck, * mKeyCheck, * mDownCheck, * mUpCheck, * mPosCheck, * mXCheck, * mYCheck;
    QTextEdit* kbLogText, * mLogText;

    bool kbRunning = true;
    bool mRunning = true;

    void addLog(int type, const QString& message);

signals:
    void requestLogUpdate(int type, QString message);

private slots:
    void onLogReceived(int type, QString message);
    void populateDeviceTree();
    void onPositionToggled(bool checked);
    void onItemChanged(QTreeWidgetItem* item, int column);
    void onSaveMapping();   // Ported from IDM_SAVE_MAPPING
    void onApplyMapping();  // Ported from IDM_APPLY_MAPPING
    void onExportMapping();
    void onExportKbLog();
    void onExportMLog();
    void onToggleKbLog(); // To start/stop logging
    void onClearKbLog();  // To clear the text edit
    void onToggleMLog(); // Start/Stop mouse log
    void onClearMLog();  // Clear mouse text area

private:
    Mapping& currentMapping; // Initialized to "mapping.mapping" by default
    void setupUi();
    QTreeWidget* deviceTree;
    QLabel* summaryLabel; // Add this member
    QPushButton* btnToggleKb;
    bool isKbLogging = true; // Track state
    QPushButton* btnToggleM;
    bool isMLogging = true; // Tracking mouse logging state
};

#endif