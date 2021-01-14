extends Node2D

onready var lvl_name=get_node("bg/lvl_name")


func _ready():
	lvl_name.text=SceneGlobals.levels[SceneGlobals.new_lvl_id]
	SceneGlobals.unlocked[SceneGlobals.new_lvl_id]=true
	SceneGlobals.new_lvl_id=-1	

func _process(_delta):
	pass

func _input(event):
	if (Input.is_action_just_pressed("in_accept")) or (event is InputEventMouseButton):

		for i_ in range(len(SceneGlobals.levels)):
			if SceneGlobals.unlocked[i_]==false and SceneGlobals.total_score>=SceneGlobals.prices[i_]:
				SceneGlobals.new_lvl_id=i_
				# warning-ignore:return_value_discarded
				get_tree().change_scene("res://menus/lvl_unlocked.tscn")

		if SceneGlobals.new_lvl_id==-1:
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://game.tscn")

