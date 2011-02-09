import math3d
import gfx
import stage
import stage_util


#def run():

misc_util = stage_util.CreateStageMiscUtility()

misc_util.CreateSkybox()

# floor
#static_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(50,1,50), diffuse_color = gfx.Color(1,1,1,1), pose = math3d.Matrix34Identity() )

for x in range(-2,3):
	for y in range(-2,3):
		for z in range(-2,3):
			box_pos = math3d.Vector3(x,100+y,z)
			#box_orient = math3d.Matrix33Identity()
			box_orient = math3d.Matrix33RotationZ(x) * math3d.Matrix33RotationX(z)
			box_pose = math3d.Matrix34( box_pos, box_orient )
			misc_util.CreateBox( edge_lengths = math3d.Vector3(0.8,0.8,0.8), diffuse_color = gfx.Color(1.0,0.5,0.5,1.0), pose = box_pose )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
