/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

import QtQuick 2.0
import org.SfietKonstantin.widgets 2.0

Item {
    id: container
    property Item moveParent
    property int index
    property WidgetListModel widgetListModel
    signal moveStarted()
    signal moveFinished()
    signal moved(real x, real y)
    signal clicked()
    signal doubleClicked()
    signal pressAndHold()
    property int minimumWidth: 0
    property int minimumHeight: 0
    implicitWidth: Math.max(widgetContainer.childrenRect.width, minimumWidth)
    implicitHeight: Math.max(widgetContainer.childrenRect.height, minimumHeight)

    Item {
        id: widgetContainer
        function move() {
            if (mouseArea.isMoving) {
                container.moved(mouseArea.mouseX, mouseArea.mouseY)
            }
        }

        width: container.width
        height: container.height
        onXChanged: widgetContainer.move()
        onYChanged: widgetContainer.move()

        Component.onCompleted: {
            if (widgetListModel == null) {
                console.warn("Cannot create widget: no model set.")
                return
            }
            container.widgetListModel.createWidget(container.index, widgetContainer)
        }
    }

    MouseArea {
        id: mouseArea
        z: 1000
        anchors.fill: parent
        property Item previousParent
        property bool isMoving: false

        function startMove() {
            if (container.moveParent == null) {
                console.warn("Cannot move widget: no moveParent set. ")
                return
            }

            reparent(container.moveParent)
            widgetContainer.z = 1000
            drag.target = widgetContainer
            mouseArea.isMoving = true
            container.moveStarted()
        }


        function finishMove() {
            if (container.moveParent == null) {
                return
            }

            reparent(container)
            widgetContainer.z = 0
            moveBackAnimation.start()
            drag.target = null
            mouseArea.isMoving = false
            container.moveFinished()
        }

        function reparent(newParent) {
            var newPos = widgetContainer.mapToItem(newParent, 0, 0)
            widgetContainer.parent = newParent
            widgetContainer.x = newPos.x - widgetContainer.width / 2 * (1 - widgetContainer.scale)
            widgetContainer.y = newPos.y - widgetContainer.height / 2 * (1 - widgetContainer.scale)
        }

        onPressed: startMove()
        onReleased: finishMove()
        onClicked: container.clicked()
        onDoubleClicked: container.doubleClicked()
        onPressAndHold: container.pressAndHold()
    }

    NumberAnimation {
        id: moveBackAnimation
        target: widgetContainer
        properties: "x,y"
        to: 0
        easing.type: Easing.InOutQuad
    }
}
