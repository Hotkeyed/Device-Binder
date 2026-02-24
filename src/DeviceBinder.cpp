#include "DeviceBinder.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QHeaderView>
#include <QApplication>
#include <QFile>
#include <QTextStream>

DeviceBinder::DeviceBinder(QWidget* parent) : QMainWindow(parent) {
    setupUi();

    // Connect the thread-safe signal
    connect(this, &DeviceBinder::logMessageReceived, this, &DeviceBinder::appendToLog);
}
void DeviceBinder::handleSaveMapping() {
    QString path = QFileDialog::getSaveFileName(this, "Save Mapping", "", "JSON Files (*.json)");
    if (!path.isEmpty()) {
        // Implement your save logic here
    }
}
void DeviceBinder::setupUi() {
    setWindowTitle("Device Binder");
    resize(1200, 800);

    // Create Menubar (Replaces AppendMenuW)
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("Save Mapping", this, &DeviceBinder::handleSaveMapping);
    fileMenu->addAction("Apply Mapping", this, &DeviceBinder::handleApplyMapping);
    fileMenu->addSeparator();
    fileMenu->addAction("Export Keyboard Log", this, &DeviceBinder::handleExportKeyboardLog);
    fileMenu->addAction("Export Mouse Log", this, &DeviceBinder::handleExportMouseLog);

    tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    createDeviceTab();
    createKeyboardTab();
    createMouseTab();
}

void DeviceBinder::createDeviceTab() {
    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);

    deviceTable = new QTableWidget(0, 4);
    deviceTable->setHorizontalHeaderLabels({"Device ID", "Device Name", "Manufacturer", "Interface"});
    deviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // In a real app, you would loop through DeviceManager here to populate rows
    layout->addWidget(deviceTable);
    tabs->addTab(container, "Devices");
}

void DeviceBinder::createKeyboardTab() {
    QWidget* container = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(container);

    // Options Panel
    QGroupBox* options = new QGroupBox("Options");
    QVBoxLayout* optLayout = new QVBoxLayout(options);

    kbInterfaceCheck = new QCheckBox("Interface Name");
    kbProductCheck = new QCheckBox("Product Name");
    kbIdCheck = new QCheckBox("Device ID");
    kbDownCheck = new QCheckBox("Key Down");
    kbUpCheck = new QCheckBox("Key Up");

    kbInterfaceCheck->setChecked(true);
    kbIdCheck->setChecked(true);
    kbDownCheck->setChecked(true);

    kbStartPauseBtn = new QPushButton("Pause");
    connect(kbStartPauseBtn, &QPushButton::clicked, this, &DeviceBinder::toggleKeyboardLog);

    QPushButton* clearBtn = new QPushButton("Clear Log");
    connect(clearBtn, &QPushButton::clicked, this, &DeviceBinder::clearKeyboardLog);

    optLayout->addWidget(kbInterfaceCheck);
    optLayout->addWidget(kbProductCheck);
    optLayout->addWidget(kbIdCheck);
    optLayout->addWidget(kbDownCheck);
    optLayout->addWidget(kbUpCheck);
    optLayout->addStretch();
    optLayout->addWidget(kbStartPauseBtn);
    optLayout->addWidget(clearBtn);

    // Log Area
    keyboardLogText = new QTextEdit();
    keyboardLogText->setReadOnly(true);

    mainLayout->addWidget(options, 1);
    mainLayout->addWidget(keyboardLogText, 4);
    tabs->addTab(container, "Keyboard Log");
}

void DeviceBinder::createMouseTab() {
    // Similar to Keyboard tab, but adding the Position toggles
    QWidget* container = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(container);

    QGroupBox* options = new QGroupBox("Options");
    QVBoxLayout* optLayout = new QVBoxLayout(options);

    mPosCheck = new QCheckBox("Position");
    mXCheck = new QCheckBox("X");
    mYCheck = new QCheckBox("Y");
    mXCheck->setEnabled(false);
    mYCheck->setEnabled(false);

    connect(mPosCheck, &QCheckBox::toggled, this, &DeviceBinder::updateMouseOptionsState);

    optLayout->addWidget(mPosCheck);
    optLayout->addWidget(mXCheck);
    optLayout->addWidget(mYCheck);
    // ... add other checkboxes like keyboard ...

    mouseLogText = new QTextEdit();
    mouseLogText->setReadOnly(true);

    mainLayout->addWidget(options, 1);
    mainLayout->addWidget(mouseLogText, 4);
    tabs->addTab(container, "Mouse Log");
}

// Thread-safe update bridge
void DeviceBinder::postKeyboardLog(const QString& text) {
    if (kbRunning) emit logMessageReceived(keyboardLogText, text);
}

void DeviceBinder::postMouseLog(const QString& text) {
}

void DeviceBinder::appendToLog(QTextEdit* target, QString message) {
    target->append(message);
}

void DeviceBinder::toggleKeyboardLog() {
    kbRunning = !kbRunning;
    kbStartPauseBtn->setText(kbRunning ? "Pause" : "Resume");
}

void DeviceBinder::toggleMouseLog() {
}

void DeviceBinder::clearKeyboardLog() {
}

void DeviceBinder::clearMouseLog() {
}

void DeviceBinder::updateMouseOptionsState() {
    bool enabled = mPosCheck->isChecked();
    mXCheck->setEnabled(enabled);
    mYCheck->setEnabled(enabled);
}

void DeviceBinder::handleApplyMapping() {
}

void DeviceBinder::handleExportKeyboardLog() {
    QString path = QFileDialog::getSaveFileName(this, "Export Log", "", "Text Files (*.txt)");
    if (!path.isEmpty()) {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << keyboardLogText->toPlainText();
        }
    }
}

void DeviceBinder::handleExportMouseLog() {
}

// Standard Qt Destructor
DeviceBinder::~DeviceBinder() {}
int main(int argc, char* argv[]) {
    // 1. Initialize the Qt Application (High DPI, Styles, etc.)
    QApplication app(argc, argv);

    // 2. Create your Window object
    DeviceBinder window;

    // 3. You MUST call show(), otherwise it stays hidden in memory
    window.show();

    // 4. Start the event loop. This blocks until the window is closed.
    return app.exec();
}