import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Dialogs

MessageDialog {
    id: messageBox
    title: "Error"
    text: "The document has been modified."
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowStaysOnTopHint
    modality: Qt.ApplicationModal
    buttons: MessageDialog.Ok

    onAccepted: close()
}
