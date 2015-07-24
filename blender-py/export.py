import bpy

scene = bpy.context.scene
scene.frame_set(1)

#bpy.ops.export_scene.autodesk_3ds(filepath=bpy.path.abspath(bpy.path.display_name_from_filepath(bpy.data.filepath) + '.3ds'))
bpy.ops.export_scene.obj(filepath=bpy.path.abspath(bpy.path.display_name_from_filepath(bpy.data.filepath) + '.obj'),
                         use_normals=True,
                         use_uvs=True,
                         use_materials=True,
                         use_triangles=True,
                         group_by_material=True,
                         path_mode='RELATIVE')

