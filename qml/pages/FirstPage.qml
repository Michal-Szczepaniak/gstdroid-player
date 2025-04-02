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
            videoSource: _videoSource
            audioSource: _audioSource
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
            width: Theme.itemSizeSmall
            onClicked: {
                player.play()
            }
        }
        Button {
            text: "Pause"
            width: Theme.itemSizeSmall
            onClicked: player.pause()
        }
        Button {
            text: "Stop"
            width: Theme.itemSizeSmall
            onClicked: player.stop()
        }
        Button {
            text: "Seek"
            width: Theme.itemSizeSmall
            onClicked: {
                player.seek(42300)
            }
        }
    }
}

