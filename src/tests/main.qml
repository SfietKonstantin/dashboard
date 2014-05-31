import QtQuick 2.0
import org.SfietKonstantin.widgets 2.0

Rectangle {
    id: container
    width: 600
    height: 600

    Item {
        id: flowContainer
        height: flow.height
        anchors.left: parent.left; anchors.right: widgetPanel.left
        property Item movingItem

        Timer {
            id: moveTimer
            property int moveCurrentIndex: -1
            property int moveTargetIndex: -1
            interval: 150
            repeat: false
            onTriggered: {
                if (moveTargetIndex != -1) {
                    widgetModel.move(moveTimer.moveCurrentIndex, moveTimer.moveTargetIndex)
                }
            }
        }

        Flow {
            id: flow
            anchors.left: parent.left; anchors.right: parent.right
            move: Transition {
                NumberAnimation {
                    properties: "x,y"
                    easing.type: Easing.InOutQuad
                }
            }

            Repeater {
                model: WidgetListModel {
                    id: widgetModel
                }

                delegate: WidgetContainer {
                    id: widgetContainer
                    widgetListModel: widgetModel
                    function getWidth() {
                        switch (model.contextInfo.size) {
                        case Widget.Small:
                            return flow.width / 4
                        case Widget.Medium:
                            return flow.width / 2
                        case Widget.Large:
                            return flow.width
                        }
                    }

                    width: getWidth()
                    minimumHeight: 200
//                    height: 100
                    moveParent: flowContainer
                    onClicked: widgetModel.remove(model.index)
                    onPressAndHold: {
                        switch (model.contextInfo.size) {
                        case Widget.Small:
                            widgetModel.setSize(model.index, Widget.Medium)
                            break
                        case Widget.Medium:
                            widgetModel.setSize(model.index, Widget.Large)
                            break
                        case Widget.Large:
                            widgetModel.setSize(model.index, Widget.Small)
                            break
                        }
                    }
                    onMoveStarted: {
                        flowContainer.movingItem = widgetContainer
                    }
                    onMoveFinished: {
                        moveTimer.moveTargetIndex = -1
                        moveTimer.moveCurrentIndex = -1
                        moveTimer.stop()
                        flowContainer.movingItem = null
                    }
                    onMoved: {
                        if (flowContainer.movingItem === widgetContainer) {
                            // Build the list of childrens
                            var children = []
                            for (var i = 0; i < flow.children.length; i++) {
                                var child = flow.children[i]
                                if (child.width > 0 && child.height > 0) {
                                    children.push(child)
                                }
                            }

                            children.sort(
                                function(a, b) {
                                    return (a.y * flowContainer.width + a.x
                                            - (b.y * flowContainer.width + b.x))
                                })


                            // We seek for the best index candidate
                            // First we find the row (ie starting index that fit the y)
                            var previousY = 0
                            var nextY = -1
                            var firstIndex = 0
                            var lastIndex = children.length
                            for (i = 0; i < children.length; i++) {
                                var currentY = children[i].y
                                if (currentY > previousY && currentY <= y) {
                                    firstIndex = i
                                    previousY = currentY
                                }

                                if (currentY > y && i < lastIndex) {
                                    lastIndex = i
                                    nextY = currentY
                                }
                            }

                            var finalIndex = lastIndex
                            // Now we scan the x position to check after which index it fits
                            for (i = firstIndex; i < lastIndex; i++) {
                                var width = children[i].width
                                var beginX = children[i].x
                                var midX = beginX + width / 2
                                var endX = beginX + width

                                if (y < nextY) {
                                    // If we are on the left: bias toward previous index
                                    // else, bias toward next
                                    if (x < endX) {
                                        if (x < midX) {
                                            finalIndex = i
                                        } else {
                                            finalIndex = i + 1
                                        }
                                        break
                                    }
                                }
                            }

                            moveTimer.moveCurrentIndex = model.index
                            moveTimer.moveTargetIndex = finalIndex
                            moveTimer.restart()
                        }
                    }
                }
            }
        }
    }

    Item {
        id: widgetPanel
        anchors.top: parent.top; anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 200

        InstalledWidgetListModel {
            id: installed
            searchPaths: ":/"
        }

        ListView {
            id: installedView
            anchors.fill: parent
            model: installed
            delegate: MouseArea {
                width: installedView.width
                height: 50
                onClicked: widgetModel.add(model.source)

                Text {
                    anchors.left: parent.left; anchors.leftMargin: 5
                    anchors.right: parent.right; anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                }
            }
        }
    }



}
