import math3d
import gfx
import stage
import stage_util
#import random


# Initialize the basic random number generator
#random.seed()

# Add a light
light_util = stage_util.CreateStageLightUtility()

dir_light = light_util.CreateHSDirectionalLightEntity( upper_color = gfx.Color(1.0,1.0,1.0,1.0), lower_color = gfx.Color(0.1,0.1,0.1,1.0), dir = math3d.Vector3(2,-3,1) )


#def run():

misc_util = stage_util.CreateStageMiscUtility()

misc_util.CreateSkybox()

# TODO: check if the floor surface have friction?

# building
misc_util.CreateEntity( name = "building",       model="models/architecture/building.msh",       position=math3d.Vector3( 0,0,0 ), is_static=True, shape="mesh" )
misc_util.CreateEntity( name = "building_floor", model="models/architecture/building_floor.msh", position=math3d.Vector3( 0,0,0 ), is_static=True, shape="mesh" )
#misc_util.CreateEntity( name = "floor",       model="models/architecture/grid_floor_plane.msh",       position=math3d.Vector3( 0,0,0 ), is_static=True, shape="mesh" )

# objects
misc_util.CreateEntity( name="my_crate", model="synty_crate-low_res-s0.64", position=math3d.Vector3( 3.9, 0.5, 2.0 ) )#, heading=random.uniform(-1,1) )
misc_util.CreateEntity( model="synty_crate-low_res-s0.64", position=math3d.Vector3( 3.9, 1.5, 2.0 ) )#, heading=random.uniform(-1,1) )
misc_util.CreateEntity( model="synty_crate-low_res-s0.64", position=math3d.Vector3( 3.9, 0.5, 2.7 ) )#, heading=random.uniform(-1,1) )
misc_util.CreateEntity( model="synty_crate-low_res-s0.64", position=math3d.Vector3( 3.9, 1.5, 2.7 ) )#, heading=random.uniform(-1,1) )
misc_util.CreateEntity( model="synty_crate-low_res-s0.64", position=math3d.Vector3( 3.2, 0.5, 2.7 ) )#, heading=random.uniform(-1,1) )
misc_util.CreateEntity( model="synty_crate-low_res-s0.64", position=math3d.Vector3( 3.2, 1.5, 2.7 ) )#, heading=random.uniform(-1,1) )

misc_util.CreateEntity( model="55_gallon_oil_barrel-blue-new",      position=math3d.Vector3( 4, 0.6, 18.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-blue-new",      position=math3d.Vector3( 5, 0.6, 18.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-blue-new",      position=math3d.Vector3( 6, 0.6, 18.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-blue-new",      position=math3d.Vector3( 4, 0.6, 18.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-blue-new",      position=math3d.Vector3( 5, 0.6, 18.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-blue-new",      position=math3d.Vector3( 6, 0.6, 18.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-red-rusty",     position=math3d.Vector3( 4, 0.6, 21.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-red-rusty",     position=math3d.Vector3( 5, 0.6, 21.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-red-rusty",     position=math3d.Vector3( 6, 0.6, 21.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-red-rusty",     position=math3d.Vector3( 4, 0.6, 21.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-red-rusty",     position=math3d.Vector3( 5, 0.6, 21.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-red-rusty",     position=math3d.Vector3( 6, 0.6, 21.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 4, 0.6, 24.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 5, 0.6, 24.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 6, 0.6, 24.0 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 4, 0.6, 24.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 5, 0.6, 24.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
misc_util.CreateEntity( model="55_gallon_oil_barrel-old-biohazard", position=math3d.Vector3( 6, 0.6, 24.5 ), heading=0, pitch=5 )#random.uniform(0,6) )
