extends Control

var input_array=[]
export var default_value=0
var selected_item=0
var change_detected=false

func _ready():
	selected_item=default_value

func _input(event):
	
	change_detected=false
	
	if ($button_left.pressed==true and (event is InputEventMouseButton )) or (Input.is_action_just_released("in_left")==true) :
		selected_item-=1
		change_detected=true

	elif ($button_right.pressed==true and (event is InputEventMouseButton)) or (Input.is_action_just_released("in_right")==true) :
		selected_item+=1
		change_detected=true
	
	#Loop when overflow
	if selected_item<0:
		selected_item=len(input_array)-1
	elif selected_item>len(input_array)-1:
		selected_item=0

