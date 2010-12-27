//Digital filter audio

var audioController = application.findChild("audioController");
var cutFrequency = 500; //Hz
var Nf = 100

function Extension()
{
  this.title = "JS extensions/Filters/LBF"
  this.call = function()
  {
    filter()
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
  this.action.enabled = false

  audioController.wavOpen.connect(this, this.onWavOpened)
  audioController.wavClose.connect(this, this.onWavClosed)
}
/*----------------------------------------------------------------------------*/
new Extension()
/*----------------------------------------------------------------------------*/
function HxData(H,data)
{
  var s = 0.0;
  var l = Math.min(H.length, data.length)
  for(i = 0; i < l; i++)
    s += H[i] * data[i];
  return s;
}
/*----------------------------------------------------------------------------*/
function filter()
{
  try
  {
    var wav = audioController.wavFile;
    var Fd = wav.format.frequency;
    var Fc = 1.0 * cutFrequency / Fd;
    var pi = Math.PI;
    var sin = Math.sin;
    var n = Nf/2;
    Nf = 2*n + 1;

    var H = new Array(Nf);
    for(i = 0; i <= n; i++)
    {
      if(!i)
        H[n] = 2 * Fc;
      else
      {
        H[n+i] = sin(2 * pi * i * Fc) /(i * pi);
        H[n-i] = H[n+i];
      }
    }

    //Normalise
    var s = 0.0;
    for(i = 0; i < Nf; i++)
      s += H[i];
    for(i = 0; i < Nf; i++)
      H[i] /= s;

    var length = wav.length;
    var data = wav.readMix(Nf - 1)
    length -= Nf - 1

    for(i = 0; i < length && i < 1000000; i++)
    {
      var val = wav.readMix();
      data.shift();
      data.push(val);
      print(HxData(H, data))
    }

  }

  catch(error)
  {
    print(error);
  }
}
