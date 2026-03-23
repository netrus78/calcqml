import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Calc 1.0

ApplicationWindow {
    width: 320
    height: 510
    visible: true
    title: "Calculator"
    color: "#024873"

    Calculator {
        id: calc
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: mainPage
    }

    Component {
        id: mainPage

        Item {
            anchors.fill: parent

            property bool secretMode: false
            property string secretBuffer: ""

            Timer {
                id: longPressTimer
                interval: 4000
                repeat: false
                onTriggered: {
                    secretMode = true
                    secretBuffer = ""
                    secretWindowTimer.start()
                    console.log("Long press detected!")
                }
            }

            Timer {
                id: secretWindowTimer
                interval: 5000
                repeat: false
                onTriggered: {
                    secretMode = false
                    secretBuffer = ""
                    console.log("Secret mode timeout")
                }
            }

            Rectangle {
                id: displayBackground
                width: parent.width
                height: 140
                color: "#04bfad"
                radius: 20

                Image {
                    source: "file:///путь файла/pic.png"
                    width: 320
                    height: 20
                    smooth: true
                    anchors.top: parent.top
                    anchors.left: parent.left
                }
                Text {
                    text: calc.expression
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 20
                    font.pixelSize: 16
                    color: "white"
                    opacity: 0.8
                }
                Text {
                    text: calc.display
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: 20
                    anchors.bottomMargin: 10

                    font.pixelSize: 42
                    font.bold: true
                    color: "white"
                }
            }

            Grid {
                columns: 4
                spacing: 20
                anchors.top: displayBackground.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 20

                Repeater {
                    model: [
                        "()","+/-","%","÷",
                        "7","8","9","x",
                        "4","5","6","-",
                        "1","2","3","+",
                        "C","0",".","="]

                    delegate: Rectangle {
                        width: 50
                        height: 50
                        radius: 25

                        color: (mouseArea.pressed ? getPressedColor(modelData) : getColor(modelData))

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            color: mouseArea.pressed ? getPressedTextColor(modelData) : getTextColor(modelData)
                            font.pixelSize: 18
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            property bool pressed: false

                            onPressed: {
                                pressed = true
                                if (modelData === "=") {
                                    longPressTimer.start()
                                }
                            }

                            onReleased: {
                                pressed = false
                                if (modelData === "=") {
                                    longPressTimer.stop()
                                }
                            }

                            onClicked: {
                                if (secretMode && modelData >= "0" && modelData <= "9") {
                                    secretBuffer += modelData

                                    if (!"123".startsWith(secretBuffer))
                                        secretBuffer = ""

                                    if (secretBuffer === "123") {
                                        secretMode = false
                                        secretWindowTimer.stop()
                                        stack.push(secretPage)
                                        console.log("Secret menu opened")
                                        return
                                    }
                                }

                                if (modelData >= "0" && modelData <= "9")
                                    calc.inputNumber(modelData)
                                else if (modelData === ".")
                                    calc.inputDot()
                                else if (modelData === "=")
                                    calc.calculate()
                                else if (modelData === "C")
                                    calc.clear()
                                else if (modelData === "+/-")
                                    calc.plusMinus()
                                else if (modelData === "%")
                                    calc.percent()
                                else
                                    calc.inputOperation(modelData)
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: secretPage

        Item {
            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                color: "#024873"

                Column {
                    anchors.centerIn: parent
                    spacing: 20

                    Text {
                        text: "Секретное меню"
                        font.pixelSize: 28
                        color: "white"
                    }

                    Button {
                        text: "Назад"
                        onClicked: stack.pop()
                    }
                }
            }
        }
    }

    function getColor(modelData) {
        if (modelData === "C") return "#f25e5e";
        else if ("0123456789.".indexOf(modelData) !== -1) return "#b0d1d8";
        else return "#0889a6";
    }

    function getTextColor(modelData) {
        if (modelData === "C") return "white";
        else if ("0123456789.".indexOf(modelData) !== -1) return "#024873";
        else return "white";
    }

    function getPressedColor(modelData) {
        if (modelData === "C") return "#f25e5e";
        else if ("0123456789.".indexOf(modelData) !== -1) return "#04bfad";
        else return "#f7e425";
    }

    function getPressedTextColor(modelData) {
        return "white";
    }
}
