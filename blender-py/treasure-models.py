import bpy
import sys

scene = bpy.context.scene
scene.frame_set(1)

output_filename = None
parsing = False
for i in sys.argv:
    if parsing == True:
        output_filename = i
    elif i == '--':
        parsing = True

#for obj in bpy.data.objects:
#    obj.select = False
        
for obj in bpy.data.objects:

    if obj.name != 'Lamp' and obj.name != 'Lamp2' and obj.name != 'Camera':
        
        filename = obj.name

        if output_filename != None:
            if output_filename == '*' or output_filename == filename + '.obj':
                obj.hide_select = False
                obj.hide_viewport = False
                obj.hide_render = False
                obj.hide_set(False)
                obj.select_set(True)
                bpy.ops.export_scene.obj(filepath=bpy.path.abspath(filename) + '.obj',
                                         check_existing=False,
                                         use_selection=True,
                                         use_normals=True,
                                         use_uvs=True,
                                         use_materials=True,
                                         use_triangles=True,
                                         group_by_material=True,
                                         path_mode='RELATIVE')
                obj.hide_select = True
                obj.hide_viewport = True
                obj.hide_render = True
                obj.hide_set(True)
                obj.select_set(True)
        else:
            print('Filename: ' + filename)

