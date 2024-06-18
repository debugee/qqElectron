
const {app, BrowserWindow, webFrameMain} = require('electron')
app.on('browser-window-created', (event, browser)=>{
    browser.webContents.on('did-frame-navigate',(event, url,httpResponseCode, httpStatusText, isMainFrame, frameProcessId, frameRoutingId)=>{
        const frame = webFrameMain.fromId(frameProcessId, frameRoutingId)
        if(frame){
            const code ="alert('asdf')"
            frame.executeJavaScript(code)
        }
    })
})

exports.pp = "fasdf"