import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Window

import my.customcomponents 1.0


ApplicationWindow {
    id: mainAppWindow
    width: 480
    height: 480
    visible: true
    title: qsTr("Image Barch")

    Loader {
        id: messageBoxLoader
    }

    HorizontalHeaderView {
        id: horizontalHeader
        syncView: tableView
        x: 2
        y: 1
    }

    ScrollView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: horizontalHeader.bottom

        TableView {
            id: tableView
            property var columnWidths: [250, 70, 60, 90]
            columnWidthProvider: function (column) { return columnWidths[column]; }
            anchors.fill: parent
            anchors.margins: 2
            alternatingRows: true
            clip: true
            interactive: true
            rowSpacing: 1
            columnSpacing: 1
            model: ImageFilesModel {
                id: imageFilesModel
                directoryPath: ImagesDirectory
            }
            selectionBehavior: TableView.SelectRows
            selectionModel: ItemSelectionModel {}

            Connections {
                target: imageFilesModel
                function onErrorMessage(message) {
                    messageBoxLoader.sourceComponent = Qt.createComponent("MessageBox.qml");
                    messageBoxLoader.item.parentWindow = mainAppWindow;
                    messageBoxLoader.item.text = "message";
                    messageBoxLoader.item.open();
                    messageBoxLoader.item.visible = true;
                }
            }

            delegate: Rectangle {
                id: itemDelegate
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

                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true

                    onPressed: {
                        tableView.selectionModel.setCurrentIndex(tableView.model.index(row, 0),
                                                                 ItemSelectionModel.ClearAndSelect)
                    }

                    onDoubleClicked: {
                        tableView.model.processFile(row)
                    }
                }
            }
        }
    }
}

