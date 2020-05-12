#pragma once

#include "cheonsa_types.h"
#include "LinearMath/btIDebugDraw.h"

namespace cheonsa
{

	// our debug drawer implementation for bullet to use.
	class physics_bullet_debug_drawer_c
		: public btIDebugDraw
	{
	public:
		sint32_c _debug_mode;

	public:
		physics_bullet_debug_drawer_c();
		virtual ~physics_bullet_debug_drawer_c();

		virtual void drawLine( const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor ) override;
		virtual void drawLine( const btVector3 & from, const btVector3 & to, const btVector3 & color ) override;
		virtual void drawContactPoint( const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color ) override;
		virtual void reportErrorWarning( const char * warningString ) override;
		virtual void draw3dText( const btVector3 & location, const char * textString ) override;
		virtual void setDebugMode( int debugMode ) override;
		virtual int  getDebugMode() const override;

	};

}
