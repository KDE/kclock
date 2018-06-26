import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.4 as Kirigami

Kirigami.ApplicationWindow
{
    id: appwindow
    visible: true
    width: 600
    height: 440
    
    function switchToPage(pagename) {
        var page;
        if(pagename == "clock") page = mainpage;
        else if(pagename == "timer") page = pagetimer;
        else if(pagename == "stopwatch") page = pagestopwatch;
        else return;
        
        if(pageStack.depth > 1)
            pageStack.pop()
        if(page != mainpage)
            pageStack.push(page)
    }
    
    header: Kirigami.ApplicationHeader { }
    
    globalDrawer: Kirigami.GlobalDrawer {
        actions: [
            Kirigami.Action {
                text: "Clock"
                onTriggered: switchToPage("clock")
            },
            Kirigami.Action {
                text: i18n("Timer")
                onTriggered: switchToPage("timer")
            },
            Kirigami.Action {
                text: i18n("Stopwatch")
                onTriggered: switchToPage("stopwatch")
            }
        ]
        modal: !wideScreen
        width: 200
    }
    pageStack.initialPage: mainpage
    
    PageMain {
        id: mainpage
        objectName: "time"
    }
    PageTimer {
        id:pagetimer
        objectName: "timer"
    }
    PageStopwatch {
        id: pagestopwatch
        objectName: "stopwatch"
    }
}
