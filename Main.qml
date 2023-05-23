import QtQuick
import QtQuick.Controls
import QtQuick.Window

import my.customcomponents 1.0

ApplicationWindow {
    width: 480
    height: 480
    visible: true
    title: qsTr("Image Barch")

    HorizontalHeaderView {
        id: horizontalHeader
        syncView: tableView
        anchors.left: tableView.left
    }

    ScrollView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: horizontalHeader.bottom

        TableView {
            id: tableView
            property var columnWidths: [250, 70, 60, 90]
            columnWidthProvider: function (column) { return columnWidths[column] }
            anchors.fill: parent
            anchors.margins: 2
            alternatingRows: true
            clip: true
            interactive: true
            rowSpacing: 1
            columnSpacing: 1
            model: ImageFilesModel {}
            selectionBehavior: TableView.SelectRows
            selectionModel: ItemSelectionModel {}

            delegate: Rectangle {
                required property bool current
                implicitHeight: 22

                color: row === tableView.currentRow
                       ? palette.highlight
                       : (tableView.alternatingRows && row % 2 != 0
                          ? palette.alternateBase
                          : palette.base)

                Text {
                    text: model.display
                    padding: 1
                }
            }
        }
    }
}
