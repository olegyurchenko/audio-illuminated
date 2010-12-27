//Extension test

var audioController = application.findChild("audioController");
function Extension()
{
  this.title = "JS extensions/Tests/Extension test"
  this.call = function()
  {
    print("Extension call")
    print(this.dialog.exec())
  }

  this.onWavOpened = function (wavFile)
  {
    this.action.enabled = true
  }
  /*----------------------------------------------------------------------------*/
  this.onWavClosed = function ()
  {
    this.action.enabled = false
  }
  /*----------------------------------------------------------------------------*/
  this.action = application.registerExtension(this, this.call)
  this.dialog = uiLoad(scriptDir + "/shift.ui")
  this.action.enabled = false

  audioController.wavOpen.connect(this, this.onWavOpened)
  audioController.wavClose.connect(this, this.onWavClosed)
}



new Extension()
