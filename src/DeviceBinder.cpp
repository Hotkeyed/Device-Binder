#include "DeviceBinder.h"


DeviceBinder* g_pMainWin = nullptr;

DeviceBinder::DeviceBinder(Mapping& mapping, QWidget* parent) : currentMapping(mapping), QMainWindow(parent) {
    g_pMainWin = this;
    setupUi();
    populateDeviceTree();

    connect(this, &DeviceBinder::requestLogUpdate, this, &DeviceBinder::onLogReceived);
}

DeviceBinder::~DeviceBinder() { g_pMainWin = nullptr; }

void DeviceBinder::setupUi() {
    this->setWindowTitle("Device Binder");
    this->resize(1440, 810);

    QMenuBar* mBar = this->menuBar();

    // File Menu
    QMenu* fileMenu = mBar->addMenu("&File");

    // Add actions with shortcuts (matching your IDM logic)
    QAction* saveAct = fileMenu->addAction("&Save Mapping");
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &DeviceBinder::onSaveMapping);

    QAction* applyAct = fileMenu->addAction("&Apply Mapping");
    connect(applyAct, &QAction::triggered, this, &DeviceBinder::onApplyMapping);

    QAction* exportAct = fileMenu->addAction("&Export Mapping");
    connect(exportAct, &QAction::triggered, this, &DeviceBinder::onExportMapping);

    fileMenu->addSeparator(); // Visual line in the menu

    // Export Submenu
    QAction* exportKeyboardLog = fileMenu->addAction("&Export Keyboard Logs");
    connect(exportKeyboardLog, &QAction::triggered, this, &DeviceBinder::onExportKbLog);

    QAction* exportMouseLog = fileMenu->addAction("&Export Mouse Logs");
    connect(exportMouseLog, &QAction::triggered, this, &DeviceBinder::onExportMLog);

    // --- THE REST OF THE UI ---
    QTabWidget* tabs = new QTabWidget(this);

    // --- TAB 1: DEVICES (Grouped View) ---
    QWidget* devicePage = new QWidget();
    QVBoxLayout* deviceLayout = new QVBoxLayout(devicePage);

    // Summary Label
    summaryLabel = new QLabel("Device Count: 0  Keyboard Count: 0  Mouse Count: 0");
    deviceLayout->addWidget(summaryLabel);

    deviceTree = new QTreeWidget();
    deviceTree->setColumnCount(4);
    deviceTree->setHeaderLabels({"Device ID", "Device Name", "Manufacturer Name", "Device Interface Name"});
    deviceTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID column
    deviceTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Product Name
    deviceTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Manufacturer
    deviceTree->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    deviceTree->header()->setStretchLastSection(true);
    deviceTree->setSortingEnabled(true);
    deviceTree->header()->setSectionsClickable(true);
    connect(deviceTree, &QTreeWidget::itemChanged, this, &DeviceBinder::onItemChanged);
    deviceLayout->addWidget(deviceTree);

    tabs->addTab(devicePage, "Devices");

    // --- TAB 2: KEYBOARD LOG ---
    QWidget* kbPage = new QWidget();
    QHBoxLayout* kbLayout = new QHBoxLayout(kbPage);
    QGroupBox* kbOptions = new QGroupBox("Options");
    QVBoxLayout* kbOptV = new QVBoxLayout(kbOptions);

    kbIntCheck = new QCheckBox("Device Interface Name");
    kbProdCheck = new QCheckBox("Product Name");
    kbManCheck = new QCheckBox("Manufacturer Name");
    kbKeyCheck = new QCheckBox("Device Key");
    kbIdCheck = new QCheckBox("Device ID");
    kbDownCheck = new QCheckBox("Key Down");
    kbUpCheck = new QCheckBox("Key Up");
    btnToggleKb = new QPushButton("Stop Log"); // Starts as "Stop" because isKbLogging = true
    QPushButton* btnClearKb = new QPushButton("Clear Log");

    kbIntCheck->setChecked(true);
    kbIdCheck->setChecked(true);
    kbKeyCheck->setChecked(true);
    kbIdCheck->setChecked(true);
    kbDownCheck->setChecked(true);

    kbOptV->addWidget(kbIntCheck); 
    kbOptV->addWidget(kbProdCheck); 
    kbOptV->addWidget(kbManCheck);
    kbOptV->addWidget(kbKeyCheck); 
    kbOptV->addWidget(kbIdCheck); 
    kbOptV->addWidget(kbDownCheck);
    kbOptV->addWidget(kbUpCheck); 
    kbOptV->addSpacing(100);
	kbOptV->addWidget(btnToggleKb); 
    kbOptV->addWidget(btnClearKb);
    kbOptV->addStretch();

    kbLogText = new QTextEdit(); 
    kbLogText->setReadOnly(true);
    kbLayout->addWidget(kbOptions, 1); kbLayout->addWidget(kbLogText, 4);
    tabs->addTab(kbPage, "Keyboard Log");
    connect(btnToggleKb, &QPushButton::clicked, this, &DeviceBinder::onToggleKbLog);
    connect(btnClearKb, &QPushButton::clicked, this, &DeviceBinder::onClearKbLog);

    // --- TAB 3: MOUSE LOG ---
    QWidget* mPage = new QWidget();
    QHBoxLayout* mLayout = new QHBoxLayout(mPage);
    QGroupBox* mOptions = new QGroupBox("Options");
    QVBoxLayout* mOptV = new QVBoxLayout(mOptions);

    mIntCheck = new QCheckBox("Device Interface Name");
    mProdCheck = new QCheckBox("Product Name");
    mManCheck = new QCheckBox("Manufacturer Name");
    mKeyCheck = new QCheckBox("Device Key");
    mIdCheck = new QCheckBox("Device ID");
    mDownCheck = new QCheckBox("Key Down");
    mUpCheck = new QCheckBox("Key Up");
	mPosCheck = new QCheckBox("Position");
	mXCheck = new QCheckBox("X Coordinate");
	mYCheck = new QCheckBox("Y Coordinate");
    btnToggleM = new QPushButton("Stop Log"); // Starts as "Stop" because isKbLogging = true
    QPushButton* btnClearM = new QPushButton("Clear Log");

    mIntCheck->setChecked(true);
    mIdCheck->setChecked(true);
    mKeyCheck->setChecked(true);
    mIdCheck->setChecked(true);
    mDownCheck->setChecked(true);
    mXCheck->setChecked(true);
    mYCheck->setChecked(true);
	mXCheck->setEnabled(false);
    mYCheck->setEnabled(false);


    mOptV->addWidget(mIntCheck);
    mOptV->addWidget(mProdCheck);
    mOptV->addWidget(mManCheck);
    mOptV->addWidget(mKeyCheck);
    mOptV->addWidget(mIdCheck);
    mOptV->addWidget(mDownCheck);
    mOptV->addWidget(mUpCheck);
	mOptV->addWidget(mPosCheck);
    mOptV->addWidget(mXCheck);
    mOptV->addWidget(mYCheck);
    mOptV->addSpacing(100);
    mOptV->addWidget(btnToggleM);
    mOptV->addWidget(btnClearM);
    mOptV->addStretch();
    


    mLogText = new QTextEdit();
    mLogText->setReadOnly(true);

    mLayout->addWidget(mOptions, 1);
    mLayout->addWidget(mLogText, 4);
    tabs->addTab(mPage, "Mouse Log");

    // Signal/Slot Connections
    connect(mPosCheck, &QCheckBox::toggled, this, &DeviceBinder::onPositionToggled);
    connect(btnToggleM, &QPushButton::clicked, this, &DeviceBinder::onToggleMLog);
    connect(btnClearM, &QPushButton::clicked, this, &DeviceBinder::onClearMLog);
    setCentralWidget(tabs);
}

void DeviceBinder::populateDeviceTree() {
    deviceTree->blockSignals(true);
    deviceTree->clear();

    // Create Root Groups
    QTreeWidgetItem* mouseGroup = new NumericTreeWidgetItem(deviceTree, {"Mice"});
    QTreeWidgetItem* kbGroup = new NumericTreeWidgetItem(deviceTree, {"Keyboards"});

    int kbCount = 0;
    int mCount = 0;

    auto& devs = DeviceManager::getInstance().devices; //
    for (const auto& d : devs) {
        if (d->type == 1) {
            kbCount++;
        } else if (d->type == 0) {
            mCount++;
        }
        QTreeWidgetItem* parent = d->type ? kbGroup : mouseGroup; // RIM_TYPEMOUSE=0

        QTreeWidgetItem* item = new NumericTreeWidgetItem(parent);
        item->setText(0, QString::number(d->id));
        item->setText(1, QString::fromStdWString(d->productName));
        item->setText(2, QString::fromStdWString(d->manufacturerName));
        item->setText(3, QString::fromStdString(d->deviceInterfaceName));

        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
    summaryLabel->setText(QString("Device Count: %1  Keyboard Count: %2  Mouse Count: %3")
                          .arg(devs.size())
                          .arg(kbCount)
                          .arg(mCount));
    deviceTree->expandAll();

    // Step 1: Tell Qt to fit the columns to the new text
    deviceTree->header()->resizeSections(QHeaderView::ResizeToContents);

    // Step 2: Switch back to Interactive so the user can take over
    for (int i = 0; i < 4; i++) {
        deviceTree->header()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    deviceTree->setSortingEnabled(true);
    deviceTree->sortByColumn(0, Qt::AscendingOrder);
    deviceTree->blockSignals(false);
}

void DeviceBinder::onPositionToggled(bool checked) {
    mXCheck->setEnabled(checked);
    mYCheck->setEnabled(checked);
}

void DeviceBinder::addLog(int type, const QString& msg) { emit requestLogUpdate(type, msg); }

void DeviceBinder::onSaveMapping() {
    // 1. Update the backend DeviceManager with values from the UI Tree
    // We use an iterator to traverse top-level groups (Mice/Keyboards) and their children
    QTreeWidgetItemIterator it(deviceTree);
    while (*it) {
        QTreeWidgetItem* item = *it;
        if (item->parent()) { // Only process child items (actual devices), not headers
            int newId = item->text(0).toInt();
            std::string interfaceName = item->text(3).toStdString();

            // Find the device in the singleton and update its ID
            for (auto& d : DeviceManager::getInstance().devices) {
                if (d->deviceInterfaceName == interfaceName) {
                    d->id = newId;
                    break;
                }
            }
        }
        ++it;
    }
    // 3. Save to the default "mapping.mapping" file
    currentMapping.saveMapping();

    this->setWindowTitle("Device Binder"); // Reset "Unsaved" title if you used one
    QMessageBox::information(this, "Success", "Mapping saved");
}

void DeviceBinder::onApplyMapping() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Apply Mapping", "", "Mapping Files (*.mapping);;All Files (*)");

    if (!fileName.isEmpty()) {
        // Load the new mapping
        Mapping newMapping(fileName.toStdString());

        // Apply it to the devices
        DeviceManager::getInstance().setMapping(newMapping);
        newMapping.saveMapping();

        // Refresh the UI to show the new IDs
        populateDeviceTree();
    }
}
void DeviceBinder::onExportMapping() {
    // 1. Get the custom path from the user
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export Mapping", "", "Mapping Files (*.mapping)");

    if (!fileName.isEmpty()) {
        // 2. Create a new mapping object for this specific file
        Mapping exportMap(fileName.toStdString());

        // 4. Write it to disk
        exportMap.saveMapping();

        QMessageBox::information(this, "Export", "Mapping exported successfully!");
    }
}
void DeviceBinder::onExportKbLog() {
    QString path = QFileDialog::getSaveFileName(this, "Export Keyboard Log", "", "Text (*.txt)");
    if (!path.isEmpty()) { 
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            // 3. Write the entire content of the log buffer
            out << kbLogText->toPlainText();
            file.close();
            QMessageBox::information(this, "Export Success", "Keyboard log exported successfully.");
        } else {
            QMessageBox::critical(this, "Export Error", "Could not open file for writing.");
        }
    }
}

void DeviceBinder::onExportMLog() {
    QString path = QFileDialog::getSaveFileName(this, "Export Mouse Log", "", "Text (*.txt)");
    if (!path.isEmpty()) { 
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << mLogText->toPlainText();
            file.close();
            QMessageBox::information(this, "Export Success", "Mouse log exported successfully.");
        } else {
            QMessageBox::critical(this, "Export Error", "Could not open file for writing.");
        }
    }
}

void DeviceBinder::onItemChanged(QTreeWidgetItem* item, int column) {
    if (column == 0) { // If the ID column was edited
        int newId = item->text(0).toInt();
        std::string interfaceName = item->text(3).toStdString();

        // Update the backend DeviceManager immediately
        for (auto& d : DeviceManager::getInstance().devices) {
            if (d->deviceInterfaceName == interfaceName) {
                d->id = newId;
                this->setWindowTitle("Device Binder - Unsaved Changes*");
                break;
            }
        }
    }
}
void DeviceBinder::onToggleKbLog() {
    isKbLogging = !isKbLogging;

    if (isKbLogging) {
        btnToggleKb->setText("Stop Log");
    } else {
        btnToggleKb->setText("Start Log");
    }
}

void DeviceBinder::onClearKbLog() {
    kbLogText->clear();
}
void DeviceBinder::onToggleMLog() {
    isMLogging = !isMLogging;
    btnToggleM->setText(isMLogging ? "Stop Log" : "Start Log");
}

void DeviceBinder::onClearMLog() {
    mLogText->clear();
}
void DeviceBinder::onLogReceived(int type, QString msg) {
    if (type == 0) { // Keyboard
        if (isKbLogging) {
            kbLogText->append(msg);
        }
    } else if (type == 1) { // Mouse
        if (isMLogging) { // Only update if Start was pressed
            mLogText->append(msg);
        }
    }
}