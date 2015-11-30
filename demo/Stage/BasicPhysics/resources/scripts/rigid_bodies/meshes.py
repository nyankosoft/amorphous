import math3d
import gfx
import stage
import stage_util


#def run():

misc_util = stage_util.CreateStageMiscUtility()

#static_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(80,1,80), diffuse_color = gfx.Color(1,1,1,1), pose = math3d.Matrix34Identity() )

misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/terrain.msh" )
#box_pose = math3d.Matrix34( math3d.Vector3(0,3,0), math3d.Matrix33Identity() )

#for i in range(4):
#	mesh_pose = math3d.Matrix34( math3d.Vector3(0, 4 + i * 3, 0), math3d.Matrix33Identity() )
#	misc_util.CreateTriangleMeshFromMesh( mesh_path="models/humvee.msh", collision_mesh_path="models/humvee-cg.msh", pose = mesh_pose, mass=10 )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
