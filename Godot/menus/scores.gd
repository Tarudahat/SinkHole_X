extends TextureRect

func _ready():
	$ScrollContainer/front_panel/score_board_text.bbcode_text=$ScrollContainer/front_panel/score_board_text.bbcode_text.replace("_total_score_",var2str(SceneGlobals.total_score))
	for i in range(5):
		for i2 in range(4):
			$ScrollContainer/front_panel/score_board_text.bbcode_text=$ScrollContainer/front_panel/score_board_text.bbcode_text.replace("_"+var2str(i)+"_"+var2str(i2)+"_" ,var2str(SceneGlobals.hi_scores[i][i2]))

func _input(_event):
	if $ScrollContainer/front_panel/button_cancel.pressed==true or Input.is_action_just_pressed("ui_cancel"):
		if SceneGlobals.return_where==0:
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://menus/title_screen.tscn")
		elif SceneGlobals.return_where==1:
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://menus/title_menu.tscn")
