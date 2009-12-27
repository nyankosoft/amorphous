import math3d
import gfx
import stage
import stage_util


#def run():

misc_util = stage_util.CreateStageMiscUtility()

#ground_box_pose = math3d.Matrix34( math3d.Vector3(0.0,-0.5,0.0), math3d.Matrix33Identity() )
#ground_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(200,1,200), diffuse_color = gfx.Color(0.7,0.7,0.7,1.0), pose=ground_box_pose )

ground_entity = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/ground.msh" )
house_0 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh" )
house_1 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/kosciol.msh",      pose=math3d.Matrix34( math3d.Vector3( 10,0,16), math3d.Matrix33Identity() ) )
house_2 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_house.msh",  pose=math3d.Matrix34( math3d.Vector3(-10,0,16), math3d.Matrix33Identity() ) )
house_3 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_chapel.msh", pose=math3d.Matrix34( math3d.Vector3(-25,0,20), math3d.Matrix33Identity() ) )

#box_pose = math3d.Matrix34( math3d.Vector3(0,3,0), math3d.Matrix33Identity() )

hs_lighting_shader = gfx.ShaderHandle()
hs_lighting_shader.Load( "Shader/PerPixelHSLighting.fx" )
hs_lighting_tech = gfx.ShaderTechniqueHandle()
hs_lighting_tech.SetTechniqueName( "HSLs" )

stage.SetShaderToEntity( entity=house_0, shader=hs_lighting_shader, tech=hs_lighting_tech )
stage.SetShaderToEntity( entity=house_1, shader=hs_lighting_shader, tech=hs_lighting_tech )
stage.SetShaderToEntity( entity=house_2, shader=hs_lighting_shader, tech=hs_lighting_tech )
stage.SetShaderToEntity( entity=house_3, shader=hs_lighting_shader, tech=hs_lighting_tech )

#for i in range(4):
#	mesh_pose = math3d.Matrix34( math3d.Vector3(0, 4 + i * 3, 0), math3d.Matrix33Identity() )
#	misc_util.CreateTriangleMeshFromMesh( mesh_path="models/humvee.msh", collision_mesh_path="models/humvee-cg.msh", pose = mesh_pose, mass=10 )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
