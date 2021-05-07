extends Sprite

var mouse_pressed=false
var mouse_position=Vector2(0,0)

var output_vector=Vector2(0,0)

func _process(delta):
	if not mouse_pressed:
		mouse_position = self.position
	
	self.get_child(0).position = mouse_position - self.position
	output_vector = mouse_position - self.position
	
	if output_vector.length() >= Vector2(50,0).length():
		output_vector = output_vector.normalized()*50
		self.get_child(0).position = output_vector
	
	SceneGlobals.digitale_joystick_vector=output_vector
		
func _input(event):
	if(event is InputEventMouseMotion):
		mouse_position=event.position
	if(event is InputEventMouseButton):
		if(event.button_index == 1):
			mouse_pressed = event.pressed


