import bpy

scene = bpy.context.scene
scene.frame_set(1)

#bpy.ops.export_scene.autodesk_3ds(filepath=bpy.path.abspath(bpy.path.display_name_from_filepath(bpy.data.filepath) + '.3ds'))
bpy.ops.wm.obj_export(filepath=bpy.path.abspath(bpy.path.display_name_from_filepath(bpy.data.filepath) + '.obj'),
                      export_normals=True,
                      export_uv=True,
                      export_materials=True,
                      export_triangulated_mesh=True,
                      export_material_groups=True,
                      path_mode='RELATIVE')

