extends Node2D

onready var total_score_display=""
onready var score_text=get_node("panel/score") 
onready var hi_score_text=get_node("panel/HI")
onready var total_score_text=get_node("panel/total_score")
onready var added_score=-500
onready var max_total_score
onready var new_level=false

func _ready():
	max_total_score=SceneGlobals.player_score+SceneGlobals.total_score

	for i_ in range(len(SceneGlobals.levels)):
		if (SceneGlobals.unlocked[i_]==false) and (max_total_score>=SceneGlobals.prices[i_]):
			SceneGlobals.new_lvl_id=i_
		
		if SceneGlobals.new_lvl_id!=-1:
			break;


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

	if SceneGlobals.player_score>=SceneGlobals.hi_scores[SceneGlobals.level][SceneGlobals.difficulty]:
		SceneGlobals.hi_scores[SceneGlobals.level][SceneGlobals.difficulty]=SceneGlobals.player_score

	hi_score_text.text="High Score: "+var2str(SceneGlobals.hi_scores[SceneGlobals.level][SceneGlobals.difficulty])

	added_score-=100
	if added_score<=-1:
		added_score=0


	
func _input(event):
	if (Input.is_action_just_pressed("in_accept") or (event is InputEventMouseButton)):
		SceneGlobals.total_score=max_total_score

		if SceneGlobals.new_lvl_id!=-1:
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://menus/lvl_unlocked.tscn")
		else:
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://game.tscn")
	


	
