import QtQuick
import QtQuick.Controls

Item {
    property string message: ""

    Rectangle {
        width: 300
        height: 150
        color: "lightgray"

        Text {
            anchors.centerIn: parent
            text: message
            font.pixelSize: 16
        }

        Button {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: "OK"
            onClicked: {
                messageDismissedSignal()
            }
        }
    }

    signal messageDismissedSignal()
}
