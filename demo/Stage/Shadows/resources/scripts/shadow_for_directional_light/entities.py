import math3d
import gfx
import stage
import stage_util


#def run():

misc_util = stage_util.CreateStageMiscUtility()

static_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(10,1,10), diffuse_color = gfx.Color(1,1,1,1), pose = math3d.Matrix34Identity() )

for i in range(3):
	box_pose = math3d.Matrix34( math3d.Vector3(-2,2,0) + math3d.Vector3(i*2,i,0), math3d.Matrix33Identity() );
	misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(1,1,1), diffuse_color = gfx.Color(0.5,0.5,1.0,1.0), pose = box_pose )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
