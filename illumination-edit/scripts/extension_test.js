//Extension test

print(application.findChild("EditorWindow"))
function Extension()
{
  this.title = "Extension test"
  this.subMenu = "JS extensions"
  this.call = function()
  {
    print("Extension call")
  }
  application.registerExtension(this, this.call)
}

new Extension()
