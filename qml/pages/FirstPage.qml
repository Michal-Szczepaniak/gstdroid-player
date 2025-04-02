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
            videoSource: "https://github.com/Michal-Szczepaniak/gstdroid-player/raw/refs/heads/master/video.mp4"
            audioSource: "https://github.com/Michal-Szczepaniak/gstdroid-player/raw/refs/heads/master/audio.webm"
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
                player.seek(0)
            }
        }
    }
}

