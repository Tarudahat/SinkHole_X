extends Node2D

#player, enemies and maps
onready var player= get_node("player/player")
onready var enemies= get_node("enemies")
const enemy_types = 6
onready var enemy_used = [TYPE_BOOL]
onready var map= preload("res://maps/map.tscn")
onready var map2= preload("res://maps/map2.tscn") 
onready var map3= preload("res://maps/map3.tscn") 

onready var map4= preload("res://maps/map4.tscn") 
onready var rivers= preload("res://maps/lava_rivers.tscn") 

onready var map5= preload("res://maps/map5.tscn") 

onready var enemy= preload("res://objects/enemies/enemy.tscn")
onready var snow_enemy= preload("res://objects/enemies/snow_enemy.tscn")
onready var ball= preload("res://objects/enemies/ball.tscn")
onready var fire_ball= preload("res://objects/enemies/fire_ball.tscn")
onready var target_node= preload("res://objects/enemies/target.tscn")
onready var ghost= preload("res://objects/enemies/ghost.tscn")

#pause menu things
onready var pause_menu= preload("res://menus/pause_menu.tscn").instance()

#level selection/diff.
onready var map_current= Node
onready var difficulty= SceneGlobals.difficulty # 1=easy 2=normal 3=hard 4=impossible 
onready var level = SceneGlobals.level # 0=plains (OG) 1=Highway 2=Snow field 3=Volcano 4=Phantom plains

onready var map_object_layer= Node
onready var scorecounter= get_node("GUI/scorecounter")
onready var bridgecounter= get_node("GUI/bridgecounter")

onready var viewport_x=get_viewport().size.x

onready var random_x= rand_range(0.0,20.0)
onready var random_y= rand_range(0.0,12.0)

onready var prop_id = 0
onready var temp_tm = 0
onready var temp_tm2 = 0
onready var temp_tm3 = 0
onready var illegal_positions=[]

onready var item_delay=OS.get_system_time_secs()+7
onready var item_place_=false

onready var hole_update_delay=0
onready var hole_update_=true

onready var hole_spawn_delay=0
onready var spawn_hole_=true

onready var hole_position = Vector2(0,0)
onready var click_delay=0


func get_rnd_vector2D(str_):
	random_x= round(rand_range(0.0,19))
	random_y= round(rand_range(0.0,11))

	if str_=="player":
		return Vector2(random_x*64+32,random_y*64+32)
	elif str_=="snow_man":
		return Vector2(round(rand_range(0.0,18)),round(rand_range(2.0,10)))
	else:
		return Vector2(random_x,random_y)


func spawn_player():
	var player_position=get_rnd_vector2D("player")
	if map_object_layer.get_cell((player_position.x-32)/64,(player_position.y-32)/64)<10:
		player.position.x=player_position.x
		player.position.y=player_position.y
	else:
		spawn_player()

func spawn_hole():
	hole_position=get_rnd_vector2D("")
	if (map_object_layer.get_cell(hole_position.x, hole_position.y)>=0 and map_object_layer.get_cell(hole_position.x, hole_position.y)<=4) or ((map_object_layer.get_cell(hole_position.x, hole_position.y)>=10)and(map_object_layer.get_cell(hole_position.x, hole_position.y)!=14)):
		update_holes(map_object_layer)
		spawn_hole()
	map_object_layer.set_cell(hole_position.x, hole_position.y, 0)
	
func spawn_enemy(enemy_index,direction_,enemy_x_,enemy_y_):
	if enemy_index==0:
		var new_enemy=enemy.instance()
		enemies.add_child(new_enemy)
		new_enemy.rotation_degrees=90
		new_enemy.enemy_direction=direction_
		if direction_==-1:
			new_enemy.rotation_degrees=270
		new_enemy.position=Vector2(enemy_x_*64+32,enemy_y_*64+32)
		new_enemy.enemy_type=0
	elif enemy_index==1:
		var new_enemy=snow_enemy.instance()
		enemies.add_child(new_enemy)
		new_enemy.position=Vector2(enemy_x_*64+32,enemy_y_*64+32)
		new_enemy.enemy_type=1

		var new_ball=ball.instance()
		enemies.add_child(new_ball)
		new_ball.position=Vector2(200*64+32,200*64+32)
		new_ball.enemy_type=2
		new_ball.linked_node=new_enemy
	elif enemy_index==2:
		var new_target=target_node.instance()
		var new_enemy=fire_ball.instance()
		enemies.add_child(new_target)
		enemies.add_child(new_enemy)
		new_enemy.linked_node=new_target
		new_target.linked_node=new_enemy
		new_enemy.enemy_type=3
		new_target.position=Vector2(enemy_x_*64+32,enemy_y_*64+32)
		new_target.enemy_type=4
	elif enemy_index==3:
		var new_enemy=ghost.instance()
		enemies.add_child(new_enemy)
		new_enemy.position=get_rnd_vector2D("player")
		new_enemy.moves=OS.get_system_time_msecs()+1520*rand_range(2,5)
		new_enemy.enemy_type=5
		
	
func spawn_enemies(redo_):
	if enemy_used[0]==true:
		if enemy_used[1]==true or redo_==true:
			for i in range(round(get_rnd_vector2D("").y/PI)+9):
				if get_rnd_vector2D("").x>12 or i<5:
					var rnd_=[round(get_rnd_vector2D("").y/1.755),round(get_rnd_vector2D("").y/7.81),round(get_rnd_vector2D("").x/PI),round(get_rnd_vector2D("").y*2.1876)]
					illegal_positions.append(rnd_)
					for rnd__ in rnd_:
						illegal_positions.append(rnd__+1)
						illegal_positions.append(rnd__-1)
					if !rnd_[0] in illegal_positions:
						spawn_enemy(0,-1,rnd_[0],8)
					if !rnd_[1] in illegal_positions:
						spawn_enemy(0,1,rnd_[1],1)
					if get_rnd_vector2D("").x>15 or i<7:
						if !rnd_[2] in illegal_positions:
							spawn_enemy(0,-1,rnd_[2],10)
						if !rnd_[3] in illegal_positions:
							spawn_enemy(0,1,rnd_[3],3)
			if enemies.get_child_count()<=5:
				spawn_enemies(true)
			illegal_positions.clear()
		if enemy_used[2]==true and redo_==false:
			var snow_enemies=0
			while snow_enemies<round(rand_range(3,4)):
				hole_position=get_rnd_vector2D("snow_man")
				if !hole_position in illegal_positions:
					spawn_enemy(1,-1,hole_position.x,hole_position.y)
					snow_enemies+=1
				#why can't you append multiple things at once?
				illegal_positions.append(Vector2(hole_position.x,hole_position.y))
				illegal_positions.append(Vector2(hole_position.x,hole_position.y-1))
				illegal_positions.append(Vector2(hole_position.x,hole_position.y+1))
				illegal_positions.append(Vector2(hole_position.x-1,hole_position.y))
				illegal_positions.append(Vector2(hole_position.x+1,hole_position.y))
			illegal_positions.clear()
		if enemy_used[4]==true and redo_==false:
			var fire_balls=0
			while fire_balls<round(rand_range(4,6)):
				hole_position=get_rnd_vector2D("snow_man")
				spawn_enemy(2,-1,hole_position.x,hole_position.y)
				fire_balls+=1
		if enemy_used[6]==true and redo_==false:
			var spoop=0
			while spoop<2+difficulty:
				spawn_enemy(3,0,0,0)
				spoop+=1
				


func get_enemy_collision():
	for enemy_ in enemies.get_children():
		#update enemies here
		if enemy_used[1]==true and enemy_.enemy_type==0:
			update_car_enemy(enemy_)

		if enemy_used[2]==true and enemy_.enemy_type==1:
			map_object_layer.set_cell(enemy_.position.x/64, enemy_.position.y/64+1, -1)
			update_snow_enemy(enemy_)
		if enemy_used[3]==true and enemy_.enemy_type==2:
			update_snow_ball_enemy(enemy_)

		if enemy_used[4]==true and enemy_.enemy_type==3:
			update_fire_enemy(enemy_)

		if enemy_used[5]==true and enemy_.enemy_type==4:
			update_fire_target(enemy_)

		if enemy_used[6]==true and enemy_.enemy_type==5:
			update_ghost_enemy(enemy_)

		if enemy_.collided_with_player == true:

			if enemy_.enemy_type==3:
				if enemy_.position.y-16>enemy_.linked_node.position.y-90:
					player.player_state=1
				elif enemy_.position.y-16<=enemy_.linked_node.position.y-90:
					enemy_.collided_with_player=false
			else:
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
	
func update_snow_ball_enemy(enemy_):
	enemy_.rotation+=1+difficulty

	if enemy_.position==enemy_.target or enemy_.moves>=72:
		enemy_.position.x=200*64
		enemy_.can_spawn=true

	if round(rand_range(1,111))==69 and enemy_.can_spawn==true:
		enemy_.position=enemy_.linked_node.position
		enemy_.target=Vector2(player.position.x,player.position.y)
		enemy_.can_spawn=false
		enemy_.moves=0
		
	hole_position=enemy_.position
	if enemy_.target.x>enemy_.position.x:
		enemy_.position.x+=1+difficulty
	if enemy_.target.x<enemy_.position.x:
		enemy_.position.x-=1+difficulty
	if enemy_.target.y>enemy_.position.y:
		enemy_.position.y+=1+difficulty
	if enemy_.target.y<enemy_.position.y:
		enemy_.position.y-=1+difficulty

	if hole_position!=enemy_.position:
		enemy_.moves+=1
		if hole_position.x!=enemy_.position.x and hole_position.y!=enemy_.position.y:
			enemy_.moves+=1
	if hole_position==enemy_.position:
		enemy_.position=enemy_.target
	
func update_snow_enemy(enemy_):
	enemy_.get_child(0).frame=0

	if player.position.x>enemy_.position.x+32:
		enemy_.get_child(0).frame=2
	if player.position.x<enemy_.position.x-32:	
		enemy_.get_child(0).frame=1

func update_fire_target(enemy_):
	if enemy_.can_spawn==true:
		var rnd_position=get_rnd_vector2D("snow man")
		enemy_.position=Vector2(rnd_position.x*64+32,rnd_position.y*64+32)
		enemy_.linked_node.position.y=enemy_.position.y-(256+32*round(rand_range(1.0,4.0)))
		enemy_.can_spawn=false

func update_fire_enemy(enemy_):
	enemy_.position.x=enemy_.linked_node.position.x
	if enemy_.linked_node.position.y-16>=enemy_.position.y:
		enemy_.position.y+=1*difficulty
	else:
		enemy_.linked_node.can_spawn=true
		update_fire_target(enemy_.linked_node)
		
func update_ghost_enemy(enemy_):
	if player.position.x>enemy_.position.x:
		enemy_.position.x+=0.21*difficulty
	if player.position.x<enemy_.position.x:
		enemy_.position.x-=0.21*difficulty
	if player.position.y>enemy_.position.y:
		enemy_.position.y+=0.21*difficulty
	if player.position.y<enemy_.position.y:
		enemy_.position.y-=0.21*difficulty

	if enemy_.moves<=OS.get_system_time_msecs():
		if enemy_.get_child(0).get_animation()=="default":
			enemy_.get_child(0).play("poofo")
			enemy_.moves=OS.get_system_time_msecs()+220
		else:
			enemy_.position=get_rnd_vector2D("player")
			enemy_.moves=OS.get_system_time_msecs()+1520*rand_range(2,5)
			enemy_.get_child(0).play("default")
	
func update_holes(map_):
	for dummy_1 in range(20):
		for dummy_2 in range(12):
			if map_.get_cell(dummy_1,dummy_2)>-1 and map_.get_cell(dummy_1,dummy_2)<4:
				map_.set_cell(dummy_1, dummy_2, map_.get_cell(dummy_1,dummy_2)+1)



func get_colider(map_):
	#Tried doing collision the proper way, but it didn't feel right
	#and it's still wonky to use for what I want to do with it, so I made this garbage

	#uncomment to disable collision:
	#return -1

	var collision_points=[0,0,0,0]

	if map_.get_cell((player.position.x-18)/64,(player.position.y-16)/64)>-1:
		collision_points[0]=map_.get_cell((player.position.x-18)/64,(player.position.y-18)/64)
	if map_.get_cell((player.position.x-18)/64,(player.position.y+18)/64)>-1:
		collision_points[1]=map_.get_cell((player.position.x-18)/64,(player.position.y+18)/64)
	if map_.get_cell((player.position.x+18)/64,(player.position.y-16)/64)>-1:
		collision_points[2]=map_.get_cell((player.position.x+18)/64,(player.position.y-18)/64)
	if map_.get_cell((player.position.x+18)/64,(player.position.y+18)/64)>-1:
		collision_points[3]=map_.get_cell((player.position.x+18)/64,(player.position.y+18)/64)
	
	collision_points.sort()

	for i in range(4):
		if collision_points[i]!=0:
			return collision_points[i]
	
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
			if player.bridges>0 and player.player_state!=4:
				replace_item(map_object_layer,2,14)
				replace_item(map_object_layer,3,14)
				replace_item(map_object_layer,4,14)
				player.bridges-=1
			else:
				if player.player_state!=4:
					player.player_state=1
		if player.player_state!=4:
			if get_colider(map_object_layer)==5:
				player.speed=-1
				player.player_state=2
				replace_item(map_object_layer,5,-1)
				temp_tm=OS.get_system_time_secs()+6
			elif get_colider(map_object_layer)==6:
				player.bridges+=1
				replace_item(map_object_layer,6,-1)
			elif get_colider(map_object_layer)==7:
				player.player_state=3
				player.speed=2
				temp_tm2=OS.get_system_time_secs()+6
				replace_item(map_object_layer,7,-1)
			elif get_colider(map_object_layer)==8:
				player.score+=10000
				replace_item(map_object_layer,8,-1)
			elif get_colider(map_object_layer)==9:
				player.player_state=4
				player.speed=1
				player.sprite.play("shield")
				temp_tm3=OS.get_system_time_secs()+6
				replace_item(map_object_layer,9,-1)
			elif get_colider(map_object_layer)==13:
				#free item slot
				replace_item(map_object_layer,13,-1)
		if get_colider(map_object_layer)>=10 && get_colider(map_object_layer) <=12:
			player.player_state=1
	if enemy_used[0]==true:
		get_enemy_collision()

	
func add_object(map_,str_):
	if str_=="grass":
		prop_id=1
		for _dummy_1 in range(round(rand_range(7.0,12.0))):
			hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
			map_.set_cell(hole_position.x-1, hole_position.y, 12)
	if str_=="snow_grass":
		for _dummy_1 in range(round(rand_range(18.0,28.0))):
			hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
			map_.set_cell(hole_position.x-1, hole_position.y, 6)
	elif str_=="item":
		prop_id=round(rand_range(5.0,10.0))
		if prop_id>=9.2:
			prop_id=13
		hole_position=get_rnd_vector2D("")
		if map_.get_cell(hole_position.x, hole_position.y)==-1:#check if tile is empty
			map_.set_cell(hole_position.x, hole_position.y, prop_id)
	elif str_=="rocks":
		for _dummy_1 in range(round(rand_range(7.0,12.0))):
			hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
			map_.set_cell(hole_position.x-1, hole_position.y, 8)
	elif str_=="spoopy_grass":
		for _dummy_1 in range(round(rand_range(18.0,28.0))):
			hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
			map_.set_cell(hole_position.x-1, hole_position.y, 10)
	elif str_=="grave":
		for _dummy_1 in range(round(rand_range(8.0,11.0))):
			hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
			map_.set_cell(hole_position.x-1, hole_position.y, 12)
	elif str_!="item":#this needs to be last 
			for _dummy_1 in range(round(rand_range(25.0,35.0))):
				hole_position=get_rnd_vector2D("")#re-useing this it's rnd either way
				map_.set_cell(hole_position.x-1, hole_position.y, prop_id)

func game_over():
	#warning-ignore:return_value_discarded
	get_tree().change_scene("res://menus/game_over.tscn")

func _ready():
	#NOTOUCH#
	
	for _i in enemy_types:
		enemy_used.append(false)

	gen_map(level)
	spawn_enemies(false)
	spawn_player()
	self.add_child(pause_menu)
	
func gen_map(map_index):
	enemy_used[0]=true
	if map_index==0:
		enemy_used[0]=false
		map_current=map.instance()
	elif map_index==1:
		map_current=map2.instance()
		scorecounter.set("custom_colors/font_color", Color8(25,60,245,255))
		bridgecounter.set("custom_colors/font_color", Color8(25,60,245,255))
		enemy_used[1]=true

	elif map_index==2:
		map_current=map3.instance()
		scorecounter.set("custom_colors/font_color", Color8(25,60,245,255))
		bridgecounter.set("custom_colors/font_color", Color8(25,60,245,255))
		enemy_used[2]=true
		enemy_used[3]=true
	elif map_index==3:


		map_current=map4.instance()
		
		random_x=round(rand_range(0.0,8.0))
		var rnd_river=rivers.instance()
		var river_=rnd_river.get_child(random_x)

		rnd_river.remove_child(river_)
		map_current.add_child(river_)

		enemy_used[4]=true
		enemy_used[5]=true
	elif map_index==4:
		map_current=map5.instance()
		enemy_used[6]=true

	self.add_child(map_current)
	map_object_layer= map_current.get_child(0)
	self.move_child(map_current,0)
	gen_props(map_current,level)
	
func gen_props(map_,map_index):
	if map_index==0:
		add_object(map_,"grass")
	elif map_index==2:
		add_object(map_,"snow_grass")
	elif map_index==3:
		add_object(map_,"rocks")
	elif map_index==4:
		add_object(map_,"spoopy_grass")
		add_object(map_object_layer,"grave")
	


func _process(_delta):
	if Input.is_action_pressed("in_pause"):
		pause_menu.visible=true
		get_tree().paused=true


	if player.score<0:
		player.score=0
	
	scorecounter.text="Score: "+var2str(player.score)
	bridgecounter.text="Bridges: "+var2str(player.bridges)

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
		item_delay=OS.get_system_time_secs()+7+difficulty
		item_place_=false

	if player.player_state==1:
		SceneGlobals.player_score=player.score
		game_over()
	elif player.player_state==2:
		if temp_tm<=OS.get_system_time_secs():
			player.player_state=0
			player.speed=1
	elif player.player_state==3:
		if temp_tm2<=OS.get_system_time_secs():
			player.player_state=0
			player.speed=1 
	elif player.player_state==4:
		map_object_layer.set_cell(player.position.x/64,player.position.y/64,-1)
		if temp_tm3<=OS.get_system_time_secs():
			player.sprite.play("default")
			player.player_state=0

	do_physics()
	

