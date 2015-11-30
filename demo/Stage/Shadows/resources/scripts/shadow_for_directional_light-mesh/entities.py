import math3d
import gfx
import stage
import stage_util


misc_util = stage_util.CreateStageMiscUtility()

floor_pose = math3d.Matrix34( math3d.Vector3(0,-0.5,0), math3d.Matrix33Identity() )
static_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(64,1,64), diffuse_color = gfx.Color(1,1,1,1), pose = floor_pose )
misc_util.CreateStaticTriangleMeshFromMesh( mesh_path = "models/Gerzi3DARTHouse.msh" )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
