import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

Rectangle {
    id: rectangle

    color: "#757575"
    property string textColor: "#ffffff"
    property string emitterColor: "#40000000"
    property string functionColor: "#40000000"
    property string hoverColor: "#60000000"

    property string blueColor: "#0277bd"
    property string hoverBlueColor: "#0288d1"

    property string greenColor: "#2e7d32"
    property string hoverGreenColor: "#388e3c"

    property string redColor: "#c62828"
    property string hoverRedColor: "#d32f2f"

    signal emitterSelected(string emitter)
    signal emitterCreate()
    signal emitterDelete(string emitter)

    signal functionSelected(string emitter, string name)
    signal functionCreate(string emitter, string name)
    signal functionDelete(string emitter, string name)

    ListView {
        id: emittersList
        anchors.fill: parent
        anchors.margins: 3
        spacing: 3
        model: effectModel
        orientation: ListView.Horizontal
        delegate: emitterDelegate

        header: Component {
            Item {
                width: 64
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                Rectangle {
                    id: addEmitter
                    anchors.fill: parent
                    anchors.rightMargin: 3
                    radius: 3
                    color: emitterColor

                    Text {
                        id: nameLabel
                        anchors.fill: parent
                        text: "+"
                        color: textColor
                        font.pointSize: 32
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: addEmitter.color = hoverColor
                        onExited: addEmitter.color = emitterColor
                        onClicked: {
                            emitterCreate()
                        }
                    }
                }
            }
        }
    }

    Component {
        id: emitterDelegate
        Rectangle {
            id: emitter
            width: 256
            height: parent.height
            radius: 3
            color: emitterColor

            Image {
                source: "file:///" + _IconPath
                width: 48
                height: 48
                anchors.margins: 8
                anchors.left: parent.left
                anchors.top: parent.top
            }

            Text {
                id: nameLabel
                anchors.left: parent.left
                anchors.right: parent.right
                height: 64
                text: Name
                font.pointSize: 16
                color: textColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: emitter.color = hoverColor
                    onExited: emitter.color = emitterColor
                    onClicked: {
                        emitterSelected(Name)
                    }
                }
            }

            ListView {
                id: functionList
                anchors.fill: parent
                anchors.margins: 3
                anchors.topMargin: 64
                spacing: 3
                clip: true
                model: _ChildModel

                delegate: Component {
                    Rectangle {
                        id: emitterFunction
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 32
                        radius: 3
                        color: functionColor

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: emitterFunction.color = hoverColor
                            onExited: emitterFunction.color = functionColor
                            onClicked: {
                                functionSelected(Name, modelData)
                            }
                        }

                        Image {
                            id: remove
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.margins: 4
                            width: 24
                            height: 24
                            source: "qrc:/Images/fontawesome/close.svg"
                            visible: false
                        }

                        ColorOverlay {
                            anchors.fill: remove
                            source: remove
                            color: "#60ffffff"

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: parent.color = "#80ffffff"
                                onExited: parent.color = "#60ffffff"
                                onClicked: {
                                    functionDelete(Name, modelData)
                                }
                            }
                        }

                        Text {
                            id: nameLabel
                            anchors.fill: parent
                            anchors.margins: 3
                            text: modelData
                            color: textColor
                            font.pointSize: 16
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                    }
                }

                Rectangle {
                    id: deleteEmitter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 3
                    height: 32
                    radius: 3
                    color: "#00000000"
                    border.color: redColor

                    Text {
                        id: deleteLabel
                        anchors.fill: parent
                        text: "Delete Emitter"
                        color: redColor
                        font.pointSize: 16
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            deleteEmitter.color = hoverRedColor
                            deleteLabel.color = textColor
                            deleteEmitter.border.color = "#00000000"
                        }
                        onExited: {
                            deleteEmitter.color = "#00000000"
                            deleteLabel.color = redColor
                            deleteEmitter.border.color = redColor
                        }
                        onClicked: {
                            emitterDelete(Name)
                        }
                    }
                }

                header: Component {
                    Item {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 32

                        Rectangle {
                            id: addFunction
                            anchors.fill: parent
                            anchors.bottomMargin: 3
                            radius: 3
                            color: greenColor

                            Text {
                                id: nameLabel
                                anchors.fill: parent
                                text: "Add Modifier"
                                color: textColor
                                font.pointSize: 16
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: addFunction.color = hoverGreenColor
                                onExited: addFunction.color = greenColor
                                onClicked: {
                                    menu.open()
                                }

                                Menu {
                                    id: menu
                                    y: parent.height

                                    MenuItem {
                                        text: "Lifetime"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "StartSize"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "StartColor"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "StartAngle"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "StartPosition"
                                        onTriggered: functionCreate(Name, text)
                                    }

                                    MenuItem {
                                        text: "ScaleSize"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "ScaleColor"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "ScaleAngle"
                                        onTriggered: functionCreate(Name, text)
                                    }
                                    MenuItem {
                                        text: "Velocity"
                                        onTriggered: functionCreate(Name, text)
                                    }

                                }
                            }
                        }
                    }
                }
            }

        }
    }

}
