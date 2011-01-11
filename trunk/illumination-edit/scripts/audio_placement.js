//Digital filter audio

var audioController = application.findChild("audioController");
var waveForm = editWindow.findChild("editPanel").findChild("waveForm");
var effectController = application.findChild("effectController")

//print((11.45).toFixed(5))

//Digital filter audio

var LBF = 1
var HBF = 2

var audioController = application.findChild("audioController");
var waveForm = editWindow.findChild("editPanel").findChild("waveForm");

//print((11.45).toFixed(5))

var cutFrequency = 1000; //Hz
var Nf = 10
var filterType = HBF

function Extension()
{
  this.call1 = function()
  {
    try
    {
      cutFrequency = 1000; //Hz
      filterType = LBF;
      this.filter();
    }
    catch(error)
    {
      print(error)
    }
  }

  this.call2 = function()
  {
    cutFrequency = 3000; //Hz
    filterType = HBF;
    this.filter();
  }

  this.onWavOpened = function (wavFile)
  {
    this.action1.enabled = true
    this.action2.enabled = true
  }
  /*----------------------------------------------------------------------------*/
  this.onWavClosed = function ()
  {
    this.action1.enabled = false
    this.action2.enabled = false
  }
  /*----------------------------------------------------------------------------*/
  this.title = "JS extensions/Effect placement/LBF place"
  this.action1 = application.registerExtension(this, this.call1)
  this.action1.enabled = false

  this.title = "JS extensions/Effect placement/HBF place"
  this.action2 = application.registerExtension(this, this.call2)
  this.action2.enabled = false

  audioController.wavOpen.connect(this, this.onWavOpened)
  audioController.wavClose.connect(this, this.onWavClosed)
}
/*----------------------------------------------------------------------------*/
Extension.prototype.HxData  = function(H,data)
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
  var isrc = (src * l/2 + l/2).toFixed();
  var idst = (dst * l/2 + l/2).toFixed();
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
Extension.prototype.filter = function()
{
  try
  {
    print(waveForm.selectionStart, waveForm.selectionLength)

    var wav = audioController.wavFile;
    var Fd = wav.format.frequency;
    var Fc = 1.0 * cutFrequency / Fd;
    var pi = Math.PI;
    var sin = Math.sin;
    var cos = Math.cos;
    var n = Nf/2;
    var i;
    var nf = 2*n + 1;

    print(Fd, Fc, n, Nf)

    var H = new Array(Nf);
    if(filterType == LBF)
    {
      print("Low band filter")
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
    }
    else
    if(filterType == HBF)
    {
      print("Hi band filter")
      for(i = 0; i <= n; i++)
      {
        if(!i)
          H[n] = 1 - 2 * Fc;
        else
        {
          H[n+i] = -1 * sin(2 * pi * i * Fc) /(i * pi);
          H[n-i] = H[n+i];
        }
      }
    }

    //Hamming widow
    for(i = 0; i < nf; i++)
      H[i] *= 0.54 - 0.46 * cos((2 * i * pi)/nf);

    //Normalise
    var s = 0.0;
    for(i = 0; i < nf; i++)
      s += H[i];
    for(i = 0; i < nf; i++)
      H[i] /= s;



    var length = waveForm.selectionLength;
    if(!length)
      length = audioController.length - waveForm.selectionStart;

    wav.seek(waveForm.selectionStart);

    var data = wav.readMix(Nf - 1)
    length -= nf - 1
    //waveForm.position = waveForm.selectionStart

    var aborted = false;
    var waitForm = uiLoad(scriptDir + "/wait.ui")

    this.onReject = function()
    {
      aborted = true
    }

    waitForm.findChild("buttonBox").rejected.connect(this, this.onReject)
    var progressBar = waitForm.findChild("progressBar")
    waitForm.show()
    application.processEvents()

    var max = 0;
    var max_position = 0;
    var last_position = 0;
    var max_period = wav.format.frequency/10;
    var min_peak = 0.6;
    var value = 1;

    for(i = 0; i < length && !aborted; i++)
    {
      //print(i);
      var src = wav.readMix();
      data.shift();
      data.push(src[0]);
      var dst = this.HxData(H, data)
//      printFiltered(src[0], dst)
      if(dst > max)
      {
        max = dst;
        max_position = i;
      }

      if(!(i % 1000))
      {
        waveForm.position = waveForm.selectionStart + i;
        progressBar.value = (i * 100)/length;
      }

      if(!(i % max_period))
      {
        if(max >= min_peak && max_position && max_position - last_position >= max_period)
        {
          var effect = new Effect(1, 1)
          effect.timeStart = ((max_position + waveForm.selectionStart) * 1000000.)/wav.format.frequency;
          effect.property("value", value);
          effect.property("led", 1)
          value <<= 1;
          if(!value)
            value = 1;
          last_position = max_position;
          max_position = 0;
        }
        else
        {
          max = 0;
          max_position = 0;
        }
      }

      application.processEvents()

    }
    waitForm.hide()
  }

  catch(error)
  {
    print(error);
  }
}

/*----------------------------------------------------------------------------*/
new Extension()
/*----------------------------------------------------------------------------*/
