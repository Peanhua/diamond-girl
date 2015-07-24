import bpy

scene = bpy.context.scene
scene.render.image_settings.file_format = 'PNG'
scene.frame_set(1)

scene.render.filepath = bpy.path.abspath('tmp.' + bpy.path.display_name_from_filepath(bpy.data.filepath) + '.png')
bpy.ops.render.render(write_still=True)
