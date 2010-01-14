import math3d
import gfx
import stage
import stage_util


#def run():

misc_util = stage_util.CreateStageMiscUtility()

# floor
floor_pose = math3d.Matrix34( math3d.Vector3(0.0,-0.5,0.0), math3d.Matrix33Identity() )
static_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(80,1,80), diffuse_color = gfx.Color(1,1,1,1), pose = floor_pose )

for x in range(-1,2):
	for z in range(-1,2):
		if x == 0 and z == 0:
			continue

		box_pos = math3d.Vector3( x*35.0, 2.0, z*35.0  )
		box_orient = math3d.Matrix33Identity()
		box_pose = math3d.Matrix34( box_pos, box_orient )
		misc_util.CreateBox( edge_lengths = math3d.Vector3(0.8,0.8,0.8), diffuse_color = gfx.Color(1.0,0.3,0.3,1.0), pose = box_pose )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
