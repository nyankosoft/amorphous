import math3d
import gfx
import stage
import stage_util
import random


# Initialize the basic random number generator
random.seed()

# Add a light
light_util = stage_util.CreateStageLightUtility()

dir_light = light_util.CreateHSDirectionalLightEntity( upper_color = gfx.Color(1.0,1.0,1.0,1.0), lower_color = gfx.Color(0.1,0.1,0.1,1.0), dir = math3d.Vector3(2,-3,1) )


#def run():

misc_util = stage_util.CreateStageMiscUtility()

misc_util.CreateSkybox()

# terrain
terrain_05 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/terrain/compact_003_05.msh" )
terrain_06 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/terrain/compact_003_06.msh" )
terrain_09 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/terrain/compact_003_09.msh" )
terrain_15 = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/terrain/compact_003_10.msh" )



# buildings
building_pos    = math3d.Vector3( 400, 5.5, 320 )
building_orient = math3d.Matrix33Identity()
building_pose   = math3d.Matrix34( building_pos, building_orient )
building = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/architecture/2-story_concrete_warehouse.msh", pose=building_pose )

misc_util.CreateEntity( model="models/architecture/one-story_old_concreate_building.msh", position=math3d.Vector3( 425, 5.5, 320 ), is_static=True, shape="mesh" )

misc_util.CreateEntity( model="models/architecture/ancient_tower.msh", position=math3d.Vector3( 360, 3.0, 320 ), is_static=True, shape="mesh" )

##pool_pos    = math3d.Vector3( 400, 5.5, 350 )
##pool_orient = math3d.Matrix33Identity()
##pool_pose   = math3d.Matrix34( pool_pos, pool_orient )
##pool = misc_util.CreateStaticTriangleMeshFromMesh( mesh_path="models/architecture/pool.msh", pose=pool_pose )
##
###misc_util.CreateStaticWater( model="models/architecture/pool_water_surface.msh", name="pool_water_surface", position=math3d.Vector3( 400, 5.5, 350 ) )


misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 380, 10.0, 330 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 382, 10.0, 330 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 384, 10.0, 330 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 380, 10.0, 332 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 382, 10.0, 332 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 384, 10.0, 332 ), heading=random.uniform(0,6) )

misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 380, 13.0, 330 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 382, 13.0, 330 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 384, 13.0, 330 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 380, 13.0, 332 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 382, 13.0, 332 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="framed_wooden_crate_00", position=math3d.Vector3( 384, 13.0, 332 ), heading=random.uniform(0,6) )

misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 380, 13.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 382, 13.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 384, 13.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 380, 15.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 382, 15.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 384, 15.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 380, 17.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 382, 17.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 384, 17.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 380, 19.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 382, 19.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 384, 19.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 380, 21.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 382, 21.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 384, 21.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 380, 23.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 382, 23.0, 328 ), heading=random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 384, 23.0, 328 ), heading=random.uniform(0,6) )

misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 382, 5.0, 345 ) )
misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 384, 5.0, 345 ) )
misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 386, 5.0, 345 ) )

##misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 405, 26.0, 330 ) )
##misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 406, 26.0, 330 ) )
##misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 407, 26.0, 330 ) )

#misc_util.CreateBoxFromMesh( mesh_resource_path="models/misc/framed_wooden_crate_00.msh", pose=math3d.Matrix34( math3d.Vector3( 380, 15.0, 340 ), math3d.Matrix33Identity() ), mass=1.0 )
#misc_util.CreateBoxFromMesh( mesh_resource_path="models/misc/framed_wooden_crate_00.msh", pose=math3d.Matrix34( math3d.Vector3( 382, 15.0, 340 ), math3d.Matrix33Identity() ), mass=1.0 )
#misc_util.CreateBoxFromMesh( mesh_resource_path="models/misc/framed_wooden_crate_00.msh", pose=math3d.Matrix34( math3d.Vector3( 380, 15.0, 342 ), math3d.Matrix33Identity() ), mass=1.0 )
#misc_util.CreateBoxFromMesh( mesh_resource_path="models/misc/framed_wooden_crate_00.msh", pose=math3d.Matrix34( math3d.Vector3( 382, 15.0, 342 ), math3d.Matrix33Identity() ), mass=1.0 )

misc_util.CreateEntity( model="steel_tower", position=math3d.Vector3( 370, 3.0, 350 ), is_static=True )
#misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 384, 5.0, 345 ) )
#misc_util.CreateEntity( model="shooting_target_stand", position=math3d.Vector3( 386, 5.0, 345 ) )

# floor
#floor_pose = math3d.Matrix34( math3d.Vector3(0.0,-0.5,0.0), math3d.Matrix33Identity() )
#static_box = misc_util.CreateStaticBox( edge_lengths = math3d.Vector3(80,1,80), diffuse_color = gfx.Color(1,1,1,1), pose = floor_pose )

#for x in range(-1,2):
#	for z in range(-1,2):
#		if x == 0 and z == 0:
#			continue
#
#		box_pos = math3d.Vector3( x*35.0, 2.0, z*35.0  )
#		box_orient = math3d.Matrix33Identity()
#		box_pose = math3d.Matrix34( box_pos, box_orient )
#		misc_util.CreateBox( edge_lengths = math3d.Vector3(0.8,0.8,0.8), diffuse_color = gfx.Color(1.0,0.3,0.3,1.0), pose = box_pose )
#
#		box_pos = math3d.Vector3( x*5.0, 2.0, z*5.0  )
#		box_orient = math3d.Matrix33Identity()
#		box_pose = math3d.Matrix34( box_pos, box_orient )
#		misc_util.CreateBox( edge_lengths = math3d.Vector3(2.0,2.0,2.0), diffuse_color = gfx.Color(0.3,0.3,1.0,1.0), pose = box_pose )
#
#
#		# Create crates
#		for nx in range (-1,2):
#			for ny in range (0,3):
#				for nz in range(-1,2):
#					if random.randint(0,10) <= 5:
#						continue
#
#					jx = random.random() * 0.05
#					jz = random.random() * 0.05
#					crate_pos  = math3d.Vector3( x*10.0 + nx*1.1 + jx, 1.0 + ny*1.1, z*10.0 + nz*1.1 + jz )
#					crate_pose = math3d.Matrix34( crate_pos, math3d.Matrix33Identity() )
#					misc_util.CreateBoxFromMesh( "models/objects/wooden_crate_00.msh", crate_pose )

#return 1	# done - release the script

#ScriptBase.SetCallback( run )
