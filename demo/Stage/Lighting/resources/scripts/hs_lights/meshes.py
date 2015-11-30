import math3d
import gfx
import stage
import stage_util


#def run():

misc_util = stage_util.CreateStageMiscUtility()

#ground_box_pose = math3d.Matrix34( math3d.Vector3(0.0,-0.5,0.0), math3d.Matrix33Identity() )
#ground_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(200,1,200), diffuse_color = gfx.Color(0.7,0.7,0.7,1.0), pose=ground_box_pose )

ground_entity = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/ground.msh" )
house_d00 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh" )
house_a00 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_house.msh",  pose=math3d.Matrix34( math3d.Vector3(-10,0,16), math3d.Matrix33Identity() ) )
house_0   = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/kosciol.msh",      pose=math3d.Matrix34( math3d.Vector3( 10,0,16), math3d.Matrix33Identity() ) )
house_3   = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_chapel.msh", pose=math3d.Matrix34( math3d.Vector3(-25,0,20), math3d.Matrix33Identity() ) )
house_b00 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0, 35), math3d.Matrix33RotationY(1.5708) ) )
house_b01 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0, 45), math3d.Matrix33RotationY(1.5708) ) )
house_b02 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0, 55), math3d.Matrix33RotationY(1.5708) ) )
house_b03 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0, 70), math3d.Matrix33RotationY(1.5708) ) )
house_b04 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0, 80), math3d.Matrix33RotationY(1.5708) ) )
house_b05 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0, 90), math3d.Matrix33RotationY(1.5708) ) )
house_b06 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0,105), math3d.Matrix33RotationY(1.5708) ) )
house_b07 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0,115), math3d.Matrix33RotationY(1.5708) ) )
house_b08 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw65.msh",         pose=math3d.Matrix34( math3d.Vector3( 15,0,125), math3d.Matrix33RotationY(1.5708) ) )
house_a01 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_house.msh",  pose=math3d.Matrix34( math3d.Vector3(-25,0, 35), math3d.Matrix33RotationY(1.5708) ) )
house_a02 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_house.msh",  pose=math3d.Matrix34( math3d.Vector3(-25,0, 50), math3d.Matrix33RotationY(1.5708) ) )
house_a02 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/gerzi_house.msh",  pose=math3d.Matrix34( math3d.Vector3(-25,0, 65), math3d.Matrix33RotationY(1.5708) ) )
house_d01 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3(-10,0,  35), math3d.Matrix33RotationY(-1.5708) ) )
house_d02 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3(-10,0,  50), math3d.Matrix33RotationY(-1.5708) ) )
house_d03 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3(-10,0,  65), math3d.Matrix33RotationY(-1.5708) ) )
house_d04 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3(-10,0,  35), math3d.Matrix33RotationY(-1.5708) ) )
house_d05 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3(-10,0,  50), math3d.Matrix33RotationY(-1.5708) ) )
house_d06 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3(-10,0,  65), math3d.Matrix33RotationY(-1.5708) ) )

# pos.X side
house_c00 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw43.msh",         pose=math3d.Matrix34( math3d.Vector3( 30,0,  35), math3d.Matrix33RotationY(-1.5708) ) )
house_c01 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw43.msh",         pose=math3d.Matrix34( math3d.Vector3( 30,0,  45), math3d.Matrix33RotationY(-1.5708) ) )
house_c02 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw43.msh",         pose=math3d.Matrix34( math3d.Vector3( 30,0,  55), math3d.Matrix33RotationY(-1.5708) ) )
house_c03 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw43.msh",         pose=math3d.Matrix34( math3d.Vector3( 30,0,  70), math3d.Matrix33RotationY(-1.5708) ) )
house_c04 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw43.msh",         pose=math3d.Matrix34( math3d.Vector3( 30,0,  80), math3d.Matrix33RotationY(-1.5708) ) )
house_c05 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fw43.msh",         pose=math3d.Matrix34( math3d.Vector3( 30,0,  90), math3d.Matrix33RotationY(-1.5708) ) )
house_d07 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3( 50,0,  35), math3d.Matrix33RotationY( 1.5708) ) )
house_d08 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3( 50,0,  50), math3d.Matrix33RotationY( 1.5708) ) )
house_d09 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3( 50,0,  65), math3d.Matrix33RotationY( 1.5708) ) )
house_d10 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3( 50,0,  85), math3d.Matrix33RotationY( 1.5708) ) )
house_d11 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3( 50,0, 100), math3d.Matrix33RotationY( 1.5708) ) )
house_d12 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/fachwerk.msh",     pose=math3d.Matrix34( math3d.Vector3( 50,0, 115), math3d.Matrix33RotationY( 1.5708) ) )

#box_pose = math3d.Matrix34( math3d.Vector3(0,3,0), math3d.Matrix33Identity() )

hs_lighting_shader = gfx.ShaderHandle()
hs_lighting_shader.Load( "Shader/PerPixelHSLighting.fx" )
hs_lighting_tech = gfx.ShaderTechniqueHandle()
hs_lighting_tech.SetTechniqueName( "HSLs" )

stage.SetShaderToEntity( entity=house_0,  shader=hs_lighting_shader, tech=hs_lighting_tech )
stage.SetShaderToEntity( entity=house_1,  shader=hs_lighting_shader, tech=hs_lighting_tech )
stage.SetShaderToEntity( entity=house_2,  shader=hs_lighting_shader, tech=hs_lighting_tech )
stage.SetShaderToEntity( entity=house_3,  shader=hs_lighting_shader, tech=hs_lighting_tech )

#for i in range(4):
#	mesh_pose = math3d.Matrix34( math3d.Vector3(0, 4 + i * 3, 0), math3d.Matrix33Identity() )
#	misc_util.CreateTriangleMeshFromMesh( mesh_path="models/humvee.msh", collision_mesh_path="models/humvee-cg.msh", pose = mesh_pose, mass=10 )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
