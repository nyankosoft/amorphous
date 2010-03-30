import visual_effect
import gfx


#render_mgr = visual_effect.GetEntityRenderManager()
#render_mgr.EnableSoftShadow()

effect_mgr = visual_effect.GetScreenEffectManager()
lf = effect_mgr.GetLensFlare()
lf.AddTexture( texture_path="textures/packed_lens_flare-4x1.dds", num_segs_x=4, num_segs_y=1 )
lf.AddLensFlareRect( dim=0.09, scale_factor=1.0, dist_factor=0.70, color=gfx.Color(1.000,0.313,0.188,0.250), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.08, scale_factor=1.0, dist_factor=1.50, color=gfx.Color(1.000,0.813,0.250,0.130), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.07, scale_factor=1.0, dist_factor=0.47, color=gfx.Color(0.938,0.188,0.360,0.375), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.05, scale_factor=1.0, dist_factor=1.40, color=gfx.Color(0.125,0.250,0.938,0.125), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.04, scale_factor=1.0, dist_factor=0.80, color=gfx.Color(0.375,0.813,0.500,0.400), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.05, scale_factor=1.0, dist_factor=0.60, color=gfx.Color(1.000,0.125,0.125,0.062), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.04, scale_factor=1.0, dist_factor=0.42, color=gfx.Color(0.125,0.125,1.000,0.375), tex_seg_index_x=0 );
lf.AddLensFlareRect( dim=0.10, scale_factor=1.0, dist_factor=0.35, color=gfx.Color(1.000,1.000,1.000,0.125), tex_seg_index_x=1 );
lf.AddLensFlareRect( dim=0.03, scale_factor=1.0, dist_factor=1.20, color=gfx.Color(0.750,0.750,0.750,0.125), tex_seg_index_x=2 );
lf.AddLensFlareRect( dim=0.04, scale_factor=1.0, dist_factor=0.19, color=gfx.Color(0.938,0.938,0.938,0.375), tex_seg_index_x=2 );
