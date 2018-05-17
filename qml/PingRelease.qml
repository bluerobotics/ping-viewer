import QtQuick 2.7
import QtQuick.Layouts 1.3

Item {
    id: root
    height: pingItem.height
    width: pingItem.width
    property var releaseLink

    function getJson(url) {
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.onreadystatechange=function() {
            //code for debug
            //print(xmlhttp.status, xmlhttp.readyState)
            if (xmlhttp.readyState === XMLHttpRequest.DONE && xmlhttp.status == 200) {
                handleJson(xmlhttp.responseText);
            }
        }
        xmlhttp.open("GET", url, true);
        xmlhttp.send();
    }

    function handleJson(response) {
        var releases = JSON.parse(response);
        //code for debug
        //print(JSON.stringify(releases, undefined, 2))

        // Check for release tag v0.0
        // Test release is t0.0
        var releaseTagRx
        if(GitTag[0] === 't') {
            releaseTagRx = /(t)\d+\.\d+/
            print('Running test version:')
        } else {
            releaseTagRx = /(v)\d+\.\d+/
        }
        print('Current version:', GitTag)

        for(var i in releases) {
            print('Checking version:', releases[i].tag_name)

            if(releaseTagRx.test(releases[i].tag_name)) {
                var version = parseFloat(releases[i].tag_name.substring(1))
                print('Release available:', version)
                if(version > parseFloat(GitTag.substring(1))) {
                    print('New release !')
                    pingItem.clicked = true
                    pingItem.visible = true
                    root.releaseLink = releases[i].html_url
                }
            }
        }
    }

    PingItem {
        id: pingItem
        icon: "/icons/bell_white.svg"
        state: "bottom-right"
        visible: false
        startAngle: -15
        finalAngle: 15
        finalAngleValue: finalAngle
        pingpong: true
        animationType: Easing.OutCubic
        item: RowLayout {
            PingButton {
                text: "New version available !"
                width: 200
                height: 200

                onClicked: {
                    Qt.openUrlExternally(root.releaseLink)
                }
            }
        }
    }

    Component.onCompleted: {
        getJson("https://api.github.com/repos/" + GitUserRepo + "/releases")
    }
}