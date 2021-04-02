extends Control

var stage_base_id=0

func _ready():
	$TextureRect/ScrollContainer/TextureRect/arrow_buttons.input_array=SceneGlobals.levels.slice(0,4,1)#slice bc we don't want to include the non lvl lvls

func _process(_delta):
	stage_base_id=$TextureRect/ScrollContainer/TextureRect/arrow_buttons.selected_item
	$TextureRect/ScrollContainer/TextureRect/arrow_buttons/lvl_name.text=SceneGlobals.levels[stage_base_id]
	$TextureRect/ScrollContainer/TextureRect/arrow_buttons/thumbnail.texture=load("res://assets/thumbnails/thumbnail_"+var2str(stage_base_id)+".png")
	
	if $TextureRect/ScrollContainer/TextureRect/button_cancel.pressed==true:
		#warning-ignore:return_value_discarded
		get_tree().change_scene("res://menus/title_menu.tscn")
		
	if $TextureRect/ScrollContainer/TextureRect/button_ok.pressed==true:
		SceneGlobals.level=5
		Customizer.using=true
		Customizer.level_base=stage_base_id
		
		var i=0
		for option in $TextureRect/ScrollContainer/TextureRect/items.on_off_list:
			Customizer.using_items[i]=option
			i+=1
		i=0
		for option in $TextureRect/ScrollContainer/TextureRect/enemies.on_off_list:
			Customizer.using_enemies[i]=option
			i+=1
		
		SceneGlobals.difficulty=2
		i=0
		for option in $TextureRect/ScrollContainer/TextureRect/difficulty.on_off_list:
			if option==true and i>0:
				SceneGlobals.difficulty=i
			i+=1
		
		Customizer.using_music=0
		i=0
		for option in $TextureRect/ScrollContainer/TextureRect/music.on_off_list:
			if option==true and i>0:
				Customizer.using_music=i
			i+=1
		
		Customizer.using_sound_fx=$TextureRect/ScrollContainer/TextureRect/sound_fx.on_off_list[0]
		
		#warning-ignore:return_value_discarded
		get_tree().change_scene("res://game.tscn")
