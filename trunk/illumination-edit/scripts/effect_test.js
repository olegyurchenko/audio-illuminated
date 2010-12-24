//Effects test

var effectController = application.findChild("effectController")

var effect = new Effect(1, 1)
effect.timeStart = 1

print(effect, effect.timeStart)

effect = new Effect(1, 1)

effectController.properties[1].timeStart = 100000
print(effectController.properties[0] == effect)
print(effectController.properties[1] == effect)
print(effect.timeStart)
//effectController.properties[2] = Effect(1, 1)

print(effectController.properties.length)
for(i = 0; i < effectController.properties.length; i++)
  print(effectController.properties[i])

var effects = effectController.selectEffects(0, 120000)
print(effects, effects.length)
//print(effectController.selectEffects)
