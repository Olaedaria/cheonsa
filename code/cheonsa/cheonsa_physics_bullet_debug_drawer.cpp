#include "cheonsa_physics_bullet_debug_drawer.h"
#include "cheonsa_physics_types.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	physics_bullet_debug_drawer_c::physics_bullet_debug_drawer_c()
		: _debug_mode( 0 )
	{
	}

	physics_bullet_debug_drawer_c::~physics_bullet_debug_drawer_c()
	{
	}

	void physics_bullet_debug_drawer_c::drawLine( const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor )
	{
		engine.get_video_renderer_interface()->add_debug_line( true, vector64x3_c( from.x(), from.y(), from.z() ), vector64x3_c( to.x(), to.y(), to.z() ), vector32x4_c( static_cast< float32_c >( fromColor.x() ), static_cast< float32_c >( fromColor.y() ), static_cast< float32_c >( fromColor.z() ), 0.5f ), vector32x4_c( static_cast< float32_c >( toColor.x() ), static_cast< float32_c >( toColor.y() ), static_cast< float32_c >( toColor.z() ), 0.5f ) );
	}

	void physics_bullet_debug_drawer_c::drawLine( const btVector3 & from, const btVector3 & to, const btVector3 & color )
	{
		engine.get_video_renderer_interface()->add_debug_line( true, vector64x3_c( from.x(), from.y(), from.z() ), vector64x3_c( to.x(), to.y(), to.z() ), vector32x4_c( static_cast< float32_c >( color.x() ), static_cast< float32_c >( color.y() ), static_cast< float32_c >( color.z() ), 0.5f ) );
	}

	void physics_bullet_debug_drawer_c::drawContactPoint( const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color )
	{
	}

	void physics_bullet_debug_drawer_c::reportErrorWarning( const char* warningString )
	{
	}

	void physics_bullet_debug_drawer_c::draw3dText( const btVector3 & location, const char * textString )
	{
	}

	void physics_bullet_debug_drawer_c::setDebugMode( int debugMode )
	{
		_debug_mode = debugMode;
	}

	int  physics_bullet_debug_drawer_c::getDebugMode() const
	{
		return _debug_mode;
	}

}
