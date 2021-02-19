extends Control

var stage_base_id=0

func _ready():
	$TextureRect/ScrollContainer/TextureRect/arrow_buttons.input_array=SceneGlobals.levels.slice(0,4,1)#slice bc we don't want to include the non lvl lvls

func _process(delta):
	stage_base_id=$TextureRect/ScrollContainer/TextureRect/arrow_buttons.selected_item
	$TextureRect/ScrollContainer/TextureRect/arrow_buttons/lvl_name.text=SceneGlobals.levels[stage_base_id]
	$TextureRect/ScrollContainer/TextureRect/arrow_buttons/thumbnail.texture=load("res://assets/thumbnails/thumbnail_"+var2str(stage_base_id)+".png")
