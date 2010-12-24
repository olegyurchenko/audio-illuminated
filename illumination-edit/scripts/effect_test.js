//Effects test

var effectController = application.findChild("effectController")

var effect = new Effect(1, 1)
effect.timeStart = 1

print(effect, effect.timeStart)

effect = new Effect(1, 1)

effectController.properties[1].timeStart = 100
print(effectController.properties[0] == effect)
print(effectController.properties[1] == effect)
print(effect.timeStart)
//effectController.properties[2] = Effect(1, 1)

print(effectController.properties.length)
