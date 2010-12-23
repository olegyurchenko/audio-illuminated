//Audio test

var audioController = application.findChild("audioController");
//print(audioController);
/*----------------------------------------------------------------------------*/
function AudioTest(audio)
{
  //1 connect to signals
  audio.wavOpen.connect(this, this.onWavOpened)
  audio.wavClose.connect(this, this.onWavClosed)
}
/*----------------------------------------------------------------------------*/
AudioTest.prototype.onWavOpened = function (wavFile)
{
  print("onWawOpened")
  print("fileName = " + wavFile.fileName)
  print("duration = " + wavFile.duration)
  print("length = " + wavFile.length)
  print("channels = " + audioController.wavFile.format.channels)
  print(wavFile.seek(1000000))
  var data = wavFile.read(100)
  for(i = 0; i < data.length; i++)
    print("data[" + i + "] = " + data[i])

  audioController.play(0);
}
/*----------------------------------------------------------------------------*/
AudioTest.prototype.onWavClosed = function ()
{
  print("onWawClosed")
}
/*----------------------------------------------------------------------------*/
audioTest = new AudioTest(audioController);
