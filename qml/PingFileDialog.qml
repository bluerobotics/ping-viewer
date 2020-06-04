import QtQuick 2.11
import QtQuick.Dialogs 1.3

Item {
    id: root

    property alias folder: fileDialog.folder
    property alias title: fileDialog.title
    property alias shortcuts: fileDialog.shortcuts
    property var fileName: ""
    property var fileUrl: ""
    property alias nameFilters: fileDialog.nameFilters

    signal accepted(var file)

    visible: false

    FileDialog {
        /**
         * This is necessary since qml url tipe is not a QUrl and does not have `path()`, `toLocalPath()` and
         *  fileName() functions.
         * This is based on the original implementation of QUrl:
         *  https://code.woboq.org/qt5/qtbase/src/corelib/io/qurl.cpp.html#_ZNK11QUrlPrivate11toLocalFileE12QUrlTwoFlagsIN4QUrl19UrlFormattingOptionENS1_25ComponentFormattingOptionEE
         *
         * For more information:
         *  https://bugreports.qt.io/browse/QTBUG-54988
         *  https://stackoverflow.com/questions/38610346/local-file-representation-of-qmls-url-type/38619149
         */
        /**
         * There is no `fileName()` function in qml url type
         */

        id: fileDialog

        function toLocalPath(qmlUrl) {
            // 1 (File) : File (remove file://) : format
            var finalString;
            var urlString = qmlUrl.toString();
            if (urlString.startsWith("file:///")) {
                // Check if is a windows string (8) or linux (7)
                var sliceValue = urlString.charAt(9) === ":" ? 8 : 7;
                finalString = urlString.substring(sliceValue);
            } else {
                finalString = urlString;
            }
            return finalString;
        }

        function getFileName(qmlFolder, qmlUrl) {
            var sizeToRemove = qmlFolder.toString().length - qmlUrl.toString().length;
            return qmlUrl.toString().slice(sizeToRemove);
        }

        //title: "Please choose a log file"
        //nameFilters: ["Binary files (*.bin)"]
        folder: shortcuts.home
        visible: root.visible
        onAccepted: {
            var fileUrl = fileDialog.fileUrl;
            var folderUrl = fileDialog.folder;
            root.fileUrl = toLocalPath(fileUrl);
            root.fileName = getFileName(folderUrl, fileUrl);
            print("Dialog window finished.");
            print(title, nameFilters, root.fileUrl, root.fileName);
            root.visible = false;
            root.accepted(root.fileUrl);
        }
        onRejected: root.visible = false
    }

}
