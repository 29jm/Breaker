tool
extends Node2D

var type_to_id = {
	"blank": 0,
	"normal": 1,
	"unbreakable": 2,
	"resistant": 3,
	"expand": 4,
	"one_up": 5,
	"new_ball": 6
}

# Needed because the order of colors matters in the color palette
# and a dictionnary doesn't preserve order
var color_names = []

func get_palette():
	var palette_file = File.new()
	var palette = {}
	palette_file.open("../palette.list", File.READ)

	for line in palette_file.get_as_text().split("\n"):
		var words = line.split(" ")
		if len(words) < 2:
			continue
		palette[words[1]] = Color(words[0])
		color_names.append(words[1])

	return palette

func get_texture_names():
	var names = []
	var dir = Directory.new()
	dir.open("textures")
	dir.list_dir_begin()

	while true:
		var fname = dir.get_next()
		if fname == "":
			break
		elif not fname in [".", ".."] and not fname.ends_with("import"):
			names.append("textures/"+fname)

	return names

func add_tile(type, texture, color):
	var sprite = Sprite.new()
	sprite.name = type
	sprite.texture = texture
	sprite.modulate = color
	self.add_child(sprite)
	sprite.set_owner(get_tree().get_edited_scene_root())

func create_tileset():
	var palette = get_palette()
	var texture_names = get_texture_names()
	var children_names = []

	for child in get_children():
		children_names.append(child.name)

	for texture_name in texture_names:
		var type = texture_name.get_file().get_basename()
		var texture = load(texture_name)

		if type == "blank" and not "blank" in children_names:
			add_tile("blank", texture, Color(0, 0, 0, 1))
			continue

		for color in palette.keys():
			var tile_name = type + "_" + color

			if not tile_name in children_names:
				add_tile(tile_name, texture, palette[color])

func get_brick_representation(bname):
	if bname == "blank":
		return "0 0"

	if len(color_names) == 0:
		get_palette()

	var idx = bname.find_last("_")
	var type_name = bname.substr(0, idx)
	var color_name = bname.substr(idx + 1, len(bname) - (idx + 1))

	return str(type_to_id[type_name]) + " " + str(color_names.find(color_name))

func _ready():
	if not Engine.editor_hint:
		return

	create_tileset()
