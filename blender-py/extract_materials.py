import bpy

for mat in bpy.data.materials:
    print('Material: ' + mat.name +
          ' diffuse= {0[0]} {0[1]} {0[2]}'.format(mat.diffuse_color)
    )

