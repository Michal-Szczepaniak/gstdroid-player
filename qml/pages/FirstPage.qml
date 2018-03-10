import QtQuick 2.0
import Sailfish.Silica 1.0
import VideoPlayer 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    Rectangle {
        anchors.fill: parent
        color: "white"

        VideoPlayer {
            id: player
            anchors.fill: parent
            source: _source
        }
    }

    Row {
        id: buttonRow
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: Theme.paddingMedium

        Button
        {
            text: "Play"
            onClicked: player.play()
        }
        Button {
            text: "Pause"
            onClicked: player.pause()
        }
        Button {
            text: "Stop"
            onClicked: player.stop()
        }
    }
}

