#include <QTreeWidgetItem>

class NumericTreeWidgetItem : public QTreeWidgetItem {
public:
    using QTreeWidgetItem::QTreeWidgetItem; // Inherit constructors

    // Override operator< to support numerical sorting for the ID column
    bool operator<(const QTreeWidgetItem& other) const override {
        int column = treeWidget()->sortColumn();

        // Column 0 is the "Device ID" column
        if (column == 0) {
            return this->text(0).toInt() < other.text(0).toInt();
        }

        // Default alphabetical sorting for other columns (Name, Manufacturer, etc.)
        return QTreeWidgetItem::operator<(other);
    }
};