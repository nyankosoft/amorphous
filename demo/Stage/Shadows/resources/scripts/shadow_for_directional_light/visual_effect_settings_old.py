
import ScriptBase
import Stage
import Entity
import Light
import VisualEffect


def run():

#	Entity.SetTarget( "light0" )
#	Entity.SetPosition( -10.0, 10.5, 20.0 )

	# configure light0
	Light.SetTargetLight( "light0" )
#	Light.SetPosition( -10.0, 14.5, 20.0 )
#	Light.SetColor( 0, 1.0, 0.2, 0.2 )
#	Light.SetColor( 2, 0.2, 0.0, 0.0 )
	Light.SetColor( 0, 0.92, 0.92, 0.95 )
	Light.SetColor( 2, 0.10, 0.10, 0.10 )

	Light.SetAttenuationFactors( 0.001, 0.003, 0.0006 )

	# shadow settings
	enable_shadow = True

	if enable_shadow:
		# VisualEffect.EnableShadowMap()
		VisualEffect.EnableSoftShadow()
	else:
		VisualEffect.DisableShadowMap()

#	VisualEffect.SetBlur( 0.5, 0.5 )
	VisualEffect.ClearBlur()

#	VisualEffect.SetGlare( 0.085 )
	VisualEffect.ClearGlare()

#	VisualEffect.SetMonochrome( 0.25, 0.20, 0.20 )
	VisualEffect.ClearMonochrome()

#	VisualEffect.SetMotionBlur( 6.5 )
	VisualEffect.ClearMotionBlur()

	VisualEffect.EnableEnvMap()
	VisualEffect.AddEnvMapTarget( "floor" )

#	VisualEffect.DisableEnvMap()
#	VisualEffect.RemoveEnvMapTarget( "floor" )

#	VisualEffect.SaveEnvMapTextureToFile( "", "debug/envmap.dds" )

	return 1

ScriptBase.SetCallback( run )
