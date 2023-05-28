import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Dialogs

MessageDialog {
    id: messageBox
    title: "Error"
    text: "The document has been modified."
    flags: Qt.WindowStaysOnTopHint
    modality: Qt.WindowModal
    buttons: MessageDialog.Ok

    onAccepted: close()
}
