import math3d
import gfx
import stage
import stage_util
import random


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

		box_pos = math3d.Vector3( x*5.0, 2.0, z*5.0  )
		box_orient = math3d.Matrix33Identity()
		box_pose = math3d.Matrix34( box_pos, box_orient )
		misc_util.CreateBox( edge_lengths = math3d.Vector3(2.0,2.0,2.0), diffuse_color = gfx.Color(0.3,0.3,1.0,1.0), pose = box_pose )


		# Create crates
		for nx in range (-1,2):
			for ny in range (0,3):
				for nz in range(-1,2):
					if random.randint(0,10) <= 5:
						continue

					jx = random.random() * 0.05
					jz = random.random() * 0.05
					crate_pos  = math3d.Vector3( x*10.0 + nx*1.1 + jx, 1.0 + ny*1.1, z*10.0 + nz*1.1 + jz )
					crate_pose = math3d.Matrix34( crate_pos, math3d.Matrix33Identity() )
					misc_util.CreateBoxFromMesh( "models/objects/wooden_crate_00.msh", crate_pose )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
