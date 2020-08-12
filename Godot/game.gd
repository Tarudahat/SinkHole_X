extends Node2D

#player, enemies and maps
onready var player= get_node("player/player")
onready var enemies= get_node("enemies")
onready var enemies_used = false
onready var map= preload("res://map.tscn")
onready var map2= preload("res://map2.tscn") 
onready var map3= preload("res://map3.tscn") 
onready var enemy= preload("res://enemy.tscn")

#pause menu things
onready var pause_menu= preload("res://pause_menu.tscn").instance()

#level selection/diff.
onready var map_current= Node
onready var difficulty= SceneGlobals.difficulty # 1=easy 2=normal 3=hard 4=impossible 
onready var level = SceneGlobals.level # 0=plains (OG) 1=Highway

onready var map_object_layer= Node
onready var scorecounter= get_node("GUI/scorecounter")

onready var viewport_x=get_viewport().size.x

onready var game_over_screen=get_node("game_over")
onready var game_over_=false

onready var random_x= rand_range(0.0,20.0)
onready var random_y= rand_range(0.0,12.0)

onready var prop_id = 0
onready var temp_tm = 0
onready var temp_tm2 = 0
onready var illegal_positions=[]

onready var item_delay=OS.get_system_time_secs()+12
onready var item_place_=false
onready var hole_update_delay=0
onready var hole_update_=true
onready var hole_spawn_delay=0
onready var spawn_hole_=true
onready var hole_position = Vector2(0,0)
onready var stuck_position = Vector2(0,0)


func get_rnd_vector2D(str_):
	random_x= round(rand_range(0.0,19))
	random_y= round(rand_range(0.0,11))
	if str_=="player":
		return Vector2(random_x*64+32,random_y*64+32)
	else:
		return Vector2(random_x,random_y)


func spawn_player():
	hole_position=get_rnd_vector2D("player")
	player.player_x=hole_position.x
	player.player_y=hole_position.y

func spawn_hole():
	hole_position=get_rnd_vector2D("")
	if map_object_layer.get_cell(hole_position.x, hole_position.y)>=0 and map_object_layer.get_cell(hole_position.x, hole_position.y)<=4:
		update_holes(map_object_layer)
		spawn_hole()
	map_object_layer.set_cell(hole_position.x, hole_position.y, 0)
	
func spawn_car_enemy(direction_,enemy_x_,enemy_y_):
	var new_enemy=enemy.instance()
	enemies.add_child(new_enemy)
	new_enemy.rotation_degrees=90
	new_enemy.enemy_direction=direction_
	if direction_==-1:
		new_enemy.rotation_degrees=270
	new_enemy.position=Vector2(enemy_x_*64+32,enemy_y_*64+32)
	
func spawn_enemies(enemy_type):
	if enemy_type==0:
		for i in range(round(get_rnd_vector2D("").y/PI)+9):
			if get_rnd_vector2D("").x>12 or i<5:
				var rnd_=[round(get_rnd_vector2D("").y/1.755),round(get_rnd_vector2D("").y/7.81),round(get_rnd_vector2D("").x/PI),round(get_rnd_vector2D("").y*2.1876)]
				illegal_positions.append(rnd_)
				for rnd__ in rnd_:
					illegal_positions.append(rnd__+1)
					illegal_positions.append(rnd__-1)
				if !rnd_[0] in illegal_positions:
					 spawn_car_enemy(-1,rnd_[0],8)
				if !rnd_[1] in illegal_positions:
					 spawn_car_enemy(1,rnd_[1],1)
				if get_rnd_vector2D("").x>15 or i<7:
					if !rnd_[2] in illegal_positions:
						spawn_car_enemy(-1,rnd_[2],10)
					if !rnd_[3] in illegal_positions:
						spawn_car_enemy(1,rnd_[3],3)
		if enemies.get_child_count()<=4:
			spawn_enemies(0)

func get_enemy_collision():
	for enemy_ in enemies.get_children():
		update_car_enemy(enemy_)
		if enemy_.collided_with_player == true:
			player.player_state=1

func update_car_enemy(enemy_):
	enemy_.position+=Vector2(enemy_.enemy_direction*(difficulty+1.5),0)
	if enemy_.position.x<-130:
		enemy_.enemy_direction=1
		enemy_.rotation_degrees=90
		enemy_.position.y-=7*64
	elif enemy_.position.x>viewport_x+130:
		enemy_.enemy_direction=-1
		enemy_.rotation_degrees=270
		enemy_.position.y+=7*64
	map_object_layer.set_cell(enemy_.position.x/64, enemy_.position.y/64, -1)
	

func update_holes(map_):
	for dummy_1 in range(20):
		for dummy_2 in range(12):
			if map_.get_cell(dummy_1,dummy_2)>-1 and map_.get_cell(dummy_1,dummy_2)<4:
				map_.set_cell(dummy_1, dummy_2, map_.get_cell(dummy_1,dummy_2)+1)

func get_colider(map_):
	#I find the Godot collision stuff to be confusing 
	#and wonky to use for what I want to do with it, so I made this garbage

	if map_.get_cell((player.position.x-18)/64,(player.position.y-16)/64)>-1:
		return map_.get_cell((player.position.x-18)/64,(player.position.y-18)/64)
	elif map_.get_cell((player.position.x-18)/64,(player.position.y+18)/64)>-1:
		return map_.get_cell((player.position.x-18)/64,(player.position.y+18)/64)
	elif map_.get_cell((player.position.x+18)/64,(player.position.y-16)/64)>-1:
		return map_.get_cell((player.position.x+18)/64,(player.position.y-18)/64)
	elif map_.get_cell((player.position.x+18)/64,(player.position.y+18)/64)>-1:
		return map_.get_cell((player.position.x+18)/64,(player.position.y+18)/64)
	else:
		return -1

func replace_item(map_,index_,index_2):
	if map_.get_cell((player.position.x-18)/64,(player.position.y-16)/64)==index_:
		map_.set_cell((player.position.x-18)/64,(player.position.y-16)/64, index_2)
	elif map_.get_cell((player.position.x-18)/64,(player.position.y+18)/64)==index_:
		map_.set_cell((player.position.x-18)/64,(player.position.y+18)/64, index_2)
	elif map_.get_cell((player.position.x+18)/64,(player.position.y-16)/64)==index_:
		map_.set_cell((player.position.x+18)/64,(player.position.y-16)/64, index_2)
	elif map_.get_cell((player.position.x+18)/64,(player.position.y+18)/64)==index_:
		map_.set_cell((player.position.x+18)/64,(player.position.y+18)/64, index_2)
	else:
		#failed to remove item
		pass

func do_physics():
	if get_colider(map_object_layer)>1:
		if get_colider(map_object_layer)<=4:
			if player.bridges>0:
				replace_item(map_object_layer,2,9)
				replace_item(map_object_layer,3,9)
				replace_item(map_object_layer,4,9)
				player.bridges-=1
			else:
				player.player_state=1
		elif get_colider(map_object_layer)==5:
			stuck_position=Vector2(player.player_x,player.player_y)
			player.player_state=2
			replace_item(map_object_layer,5,-1)
			temp_tm=OS.get_system_time_secs()+4
		elif get_colider(map_object_layer)==6:
			player.bridges+=1
			replace_item(map_object_layer,6,-1)
		elif get_colider(map_object_layer)==7:
			player.player_state=3
			player.speed=2
			temp_tm2=OS.get_system_time_secs()+5
			replace_item(map_object_layer,7,-1)
		elif get_colider(map_object_layer)==8:
			player.score+=10000
			replace_item(map_object_layer,8,-1)
	if enemies_used==true:
		get_enemy_collision()

	
func add_object(map_,str_):
	if str_=="grass":
		prop_id=1
	elif str_=="rocks":
		prop_id=7
	elif str_=="item":
		prop_id=round(rand_range(5.0,8.0))
	else:
		#Invalid prop
		pass
	if str_!="item":
		for _dummy_1 in range(round(rand_range(25.0,35.0))):
			hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
			map_.set_cell(hole_position.x-1, hole_position.y, prop_id)
	elif str_=="item":
		hole_position=get_rnd_vector2D("")
		if map_.get_cell(hole_position.x, hole_position.y)==-1:
			map_.set_cell(hole_position.x, hole_position.y, prop_id)

func game_over():
	self.remove_child(pause_menu)
	game_over_screen.visible = true
	game_over_=true

func _ready():
	game_over_screen.visible = false

	gen_map(level)
	spawn_player()
	self.add_child(pause_menu)
	
func gen_map(map_index):
	if map_index==0:
		map_current=map.instance()
	elif map_index==1:
		map_current=map2.instance()
		scorecounter.set("custom_colors/font_color", Color8(25,50,220,255))
		enemies_used=true
		spawn_enemies(0)
	elif map_index==2:
		map_current=map3.instance()
		scorecounter.set("custom_colors/font_color", Color8(25,50,220,255))
		

	self.add_child(map_current)
	map_object_layer= map_current.get_child(0)
	self.move_child(map_current,0)
	gen_props(map_current,level)
	
func gen_props(map_,map_index):
	if map_index==0:
		add_object(map_,"grass")


func _process(_delta):

	if Input.is_action_pressed("ui_cancel") and game_over_!=true:
		pause_menu.visible=true
		get_tree().paused=true

	if game_over_!=true:
		scorecounter.text="Score: "+var2str(player.score)
		game_over_screen.score_display=scorecounter.text

		if spawn_hole_==false and hole_spawn_delay<=OS.get_system_time_secs():
			spawn_hole_=true
		
		if hole_update_==false and hole_update_delay<=OS.get_system_time_msecs():
			hole_update_=true

		
		if item_place_==false and item_delay<=OS.get_system_time_secs():
			item_place_=true

		if spawn_hole_==true:
			spawn_hole()
			hole_spawn_delay=OS.get_system_time_secs()+4-difficulty
			spawn_hole_=false
		
		if hole_update_==true:
			update_holes(map_object_layer)
			hole_update_delay=OS.get_system_time_msecs()+450
			hole_update_=false

		if item_place_==true:
			add_object(map_object_layer,"item")
			item_delay=OS.get_system_time_secs()+9+difficulty
			item_place_=false

		if player.player_state==1:
			game_over()
		elif player.player_state==2:
			player.player_x=stuck_position.x
			player.player_y=stuck_position.y
			if temp_tm<=OS.get_system_time_secs():
				player.player_state=0
		elif player.player_state==3:
			if temp_tm2<=OS.get_system_time_secs():
				player.player_state=0
				player.speed=1

		do_physics()
	

func _input(event):
	if game_over_==true:
		if event is InputEventMouseButton:
			# warning-ignore:return_value_discarded
			get_tree().change_scene("res://game.tscn")
