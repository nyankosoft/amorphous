import ScriptBase
import Stage
import Light
import StageUtil


def run():

#	Stage.LoadSkybox( "skybox", "./Stage/Texture/sky10b.dds" )

	Light.LoadHSDirectionalLight( "Sunlight_Weak" )

	Stage.CreateBox( 1,1,1, "", "", 0,1,0 );

	return 1	# done - release the script

ScriptBase.SetCallback( run )
