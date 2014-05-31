import QtQuick 2.0

Rectangle {
//    Component.onCompleted: console.debug(widget.size)
//    width: parent.width
    anchors.fill: parent
//    height: 50
//    width: 300
//    height: 200
//    anchors.centerIn: parent
    color: "blue"

    Text {
        anchors.centerIn: parent
        text: Math.floor(Math.random() * 1000)
    }

    MouseArea {
        anchors.fill: parent
        onClicked: console.debug("Clicked")
    }
}
