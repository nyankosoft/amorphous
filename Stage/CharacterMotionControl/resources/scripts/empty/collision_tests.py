import math3d
import gfx
import stage
import stage_util

misc_util = stage_util.CreateStageMiscUtility()


static_mesh = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/character_motion_collision_test.msh" )


for x in range(-1,2):
	for z in range(-1,2):
		if x == 0 and z == 0:
			continue

		box_pos    = math3d.Vector3( x*10.0, 2.0, z*10.0  )
		box_orient = math3d.Matrix33Identity()
		box_pose   = math3d.Matrix34( box_pos, box_orient )
		misc_util.CreateBoxFromMesh( mesh_path = "models/objects/wooden_crate_H0.25.msh", pose = box_pose )
