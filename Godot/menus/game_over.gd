extends Node2D

onready var score_display=""
onready var score_text=get_node("panel/score")

func _process(_delta):
	score_text.text=score_display


	
