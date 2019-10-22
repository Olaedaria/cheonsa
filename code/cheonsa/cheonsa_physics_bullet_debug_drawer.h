#pragma once

#include "LinearMath/btIDebugDraw.h"

namespace cheonsa
{

	class physics_bullet_debug_drawer_c
		: public btIDebugDraw
	{
	public:
		int _debug_mode;

	public:
		physics_bullet_debug_drawer_c();
		virtual ~physics_bullet_debug_drawer_c();

		virtual void drawLine( const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor );
		virtual void drawLine( const btVector3 & from, const btVector3 & to, const btVector3 & color );
		virtual void drawContactPoint( const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color );
		virtual void reportErrorWarning( const char * warningString );
		virtual void draw3dText( const btVector3 & location, const char * textString );
		virtual void setDebugMode( int debugMode );
		virtual int  getDebugMode() const;

	};

}
