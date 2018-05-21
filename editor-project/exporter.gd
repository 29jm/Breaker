extends Node

func export_map(map):
	var size = map.get_used_rect().size
	var pos = map.get_used_rect().position
	var file = File.new()
	var tileset = map.tile_set

	file.open("maps/"+map.name+".map", File.WRITE)

	for y in range(pos.y, pos.y+size.y):
		var line = ""
		for x in range(pos.x, pos.x+size.x):
			var tile_name = tileset.tile_get_name(map.get_cell(x, y))
			line += $tileset.get_brick_representation(tile_name)
			if x != size.x-1:
				line += " "
		file.store_line(line)

func _ready():
	for map in get_children():
		if map is TileMap:
			export_map(map)
	get_tree().quit()