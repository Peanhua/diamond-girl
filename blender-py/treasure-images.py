import bpy
import sys

scene = bpy.context.scene
scene.render.resolution_x = 1024
scene.render.resolution_y = 768
scene.render.image_settings.file_format = 'PNG'
scene.frame_set(1)

output_filename = None
parsing = False
for i in sys.argv:
    if parsing == True:
        output_filename = i
    elif i == '--':
        parsing = True
    
for obj in bpy.data.objects:

    if obj.name != 'Lamp' and obj.name != 'Lamp2' and obj.name != 'Camera':
        
        for mat in bpy.data.materials:

            if mat.name[0:4] == 'mat_':
        
                obj.hide = False
                obj.hide_render = False

                if obj.get('gemstones') == None:

                    filename = obj.name + '-' + mat.name[4:] + '.png'

                    if output_filename != None:
                        if output_filename == '*' or output_filename == filename:
                            obj.material_slots[0].material = mat
                            scene.render.filepath = bpy.path.abspath('tmp.' + filename)
                            bpy.ops.render.render(write_still=True)
                    else:
                        print('Filename: ' + filename)
                        
                else:

                    for gsmat in bpy.data.materials:

                        if gsmat.name[0:6] == 'gsmat_':

                            filename = obj.name + '-' + mat.name[4:] + '-' + gsmat.name[6:] + '.png'

                            if output_filename != None:
                                if output_filename == '*' or output_filename == filename:
                                    obj.material_slots[0].material = mat
                                    obj.material_slots[1].material = gsmat
                                    scene.render.filepath = bpy.path.abspath('tmp.' + filename)
                                    bpy.ops.render.render(write_still=True)
                            else:
                                print('Filename: ' + filename)

                obj.hide = True
                obj.hide_render = True
