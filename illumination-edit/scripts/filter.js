//Digital filter audio

var audioController = application.findChild("audioController");
var waveForm = editWindow.findChild("editPanel").findChild("waveForm");

//print((11.45).toFixed(5))

var cutFrequency = 1000; //Hz
var Nf = 10

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
  for(var i = 0; i < l; i++)
    s += H[i] * data[i];
  return s;
}
/*----------------------------------------------------------------------------*/
function printFiltered(src, dst)
{
  var l = 30
  var isrc = (src * l + l/2).toFixed();
  var idst = (dst * l + l/2).toFixed();
  //print(src.toPresigion(4) ,dst.toPresigion(4),"\t")
  str = "\t|";
  for(var i = 0; i < l; i++)
  {
    if(i == isrc && i == idst)
      str += "*";
    else
    if(i == isrc)
      str += "+"
    else
    if(i == idst)
      str += "x"
    else
      str += "-"
  }
  str += "|";
  //print(src, dst, str);
  print(src.toFixed(4), dst.toFixed(4),str)
}

/*----------------------------------------------------------------------------*/
function filter()
{
  try
  {
    print(waveForm.selectionStart, waveForm.selectionLength)

    var wav = audioController.wavFile;
    var Fd = wav.format.frequency;
    var Fc = 1.0 * cutFrequency / Fd;
    var pi = Math.PI;
    var sin = Math.sin;
    var n = Nf/2;
    var i;
    var nf = 2*n + 1;

    print(Fd, Fc, n, Nf)

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
    for(i = 0; i < nf; i++)
      s += H[i];
    for(i = 0; i < nf; i++)
      H[i] /= s;

    print(H)
    var length = waveForm.selectionLength;
    wav.seek(waveForm.selectionStart)
    var data = wav.readMix(Nf - 1)
    length -= nf - 1
    print(length);
    waveForm.position = waveForm.selectionStart
    for(i = 0; i < length && i < 1000000; i++)
    {
      //print(i);
      var src = wav.readMix();
      data.shift();
      data.push(src[0]);
      var dst = HxData(H, data)
      printFiltered(src[0], dst)
      if(!(i % 100))
        waveForm.position = waveForm.selectionStart + i

    }

  }

  catch(error)
  {
    print(error);
  }
}
