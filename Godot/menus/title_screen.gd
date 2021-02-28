extends Control

onready var selected_button=0

func _ready():
	SceneGlobals.load_game()
	for _i in range(27):
		var x=round(rand_range(0,20))
		var y=round(rand_range(0,12))
		$bg0.set_cell(x,y,1)
	for _i in range(12):
		var x=round(rand_range(0,20))
		var y=round(rand_range(0,12))
		if x <=5 or x >=16:
			if y>=6 or y<=3:
				$bg0.set_cell(x,y,12)
	
	SceneGlobals.return_where=0

func _input(_event):
	#garbage:
	#selecting
	if Input.is_action_pressed("ui_up"):
		selected_button-=1
	elif Input.is_action_pressed("ui_down"):
		selected_button+=1
	#cap 
	if selected_button>=2:
		selected_button=0
	if selected_button<=-1:
		selected_button=1	
	#cursor
	if (selected_button==0):
		$cursor.position=Vector2($cursor.position.x,563)
	if (selected_button==1):
		$cursor.position=Vector2($cursor.position.x,663)

	#accepting input for the buttons
	if selected_button==1 and ($quit_button.pressed==true or Input.is_action_just_pressed("in_accept")):
		get_tree().quit()
	elif selected_button==0 and ( $start_button.pressed==true or Input.is_action_just_pressed("in_accept")):
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/title_menu.tscn")

	if selected_button==0 and $quit_button.pressed==true:
		selected_button=1
	elif selected_button==1 and $start_button.pressed==true:
		selected_button=0
