extends Node2D

onready var total_score_display=""
onready var score_text=get_node("panel/score")
onready var hi_score_text=get_node("panel/HI")
onready var total_score_text=get_node("panel/total_score")
onready var added_score=-500
onready var max_total_score

func _ready():
	max_total_score=SceneGlobals.player_score+SceneGlobals.total_score

func _process(_delta):
	total_score_display="Total Score: "+var2str(SceneGlobals.total_score)+"+"+var2str(added_score)
	
	SceneGlobals.total_score+=100

	if SceneGlobals.total_score>=max_total_score:
		SceneGlobals.total_score=max_total_score

	total_score_text.text=total_score_display
	score_text.text="Score: "+var2str(SceneGlobals.player_score)

	if added_score<=-500:
		max_total_score=SceneGlobals.player_score+SceneGlobals.total_score
		added_score=SceneGlobals.player_score

	added_score-=100
	if added_score<=-1:
		added_score=0
		

	
func _input(event):
	if (Input.is_action_just_pressed("in_accept") or (event is InputEventMouseButton)):
		SceneGlobals.total_score=max_total_score
	
		# warning-ignore:return_value_discarded
		get_tree().change_scene("res://game.tscn")
	


	
