#include "geometry/BoundingBox3f.h"

#include <algorithm>
#include <cstdio>
#include <limits>

#include <QString>

#include "geometry/GeometryUtils.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

BoundingBox3f::BoundingBox3f() :
	
	m_min( numeric_limits< float >::max(), numeric_limits< float >::max(), numeric_limits< float >::max() ),
	m_max( numeric_limits< float >::lowest(), numeric_limits< float >::lowest(), numeric_limits< float >::lowest() )

{

}

BoundingBox3f::BoundingBox3f( float minX, float minY, float minZ,
							 float maxX, float maxY, float maxZ ) :

	m_min( minX, minY, minZ ),
	m_max( maxX, maxY, maxZ )

{

}

BoundingBox3f::BoundingBox3f( const Vector3f& min, const Vector3f& max ) :

	m_min( min ),
	m_max( max )

{

}

BoundingBox3f::BoundingBox3f( const BoundingBox3f& rb ) :

	m_min( rb.m_min ),
	m_max( rb.m_max )

{

}

BoundingBox3f& BoundingBox3f::operator = ( const BoundingBox3f& rb )
{
	if( this != &rb )
	{
		m_min = rb.m_min;
		m_max = rb.m_max;
	}
	return *this;
}

BoundingBox3f::BoundingBox3f( const vector< Vector3f >& points, const Matrix4f& worldMatrix ) :

	m_min( numeric_limits< float >::max(), numeric_limits< float >::max(), numeric_limits< float >::max() ),
	m_max( numeric_limits< float >::lowest(), numeric_limits< float >::lowest(), numeric_limits< float >::lowest() )

{
	for( int i = 0; i < points.size(); ++i )
	{
		Vector4f xyz1( points[ i ], 1 );
		Vector3f xyz = ( worldMatrix * xyz1 ).xyz();
		float x = xyz.x;
		float y = xyz.y;
		float z = xyz.z;

		if( x < m_min.x )
		{
			m_min.x = x;
		}
		if( x > m_max.x )
		{
			m_max.x = x;
		}
		if( y < m_min.y )
		{
			m_min.y = y;
		}
		if( y > m_max.y )
		{
			m_max.y = y;
		}
		if( z < m_min.z )
		{
			m_min.z = z;
		}
		if( z > m_max.z )
		{
			m_max.z = z;
		}
	}
}

BoundingBox3f::BoundingBox3f( const vector< Vector4f >& points, const Matrix4f& worldMatrix ) :

	m_min( numeric_limits< float >::max(), numeric_limits< float >::max(), numeric_limits< float >::max() ),
	m_max( numeric_limits< float >::lowest(), numeric_limits< float >::lowest(), numeric_limits< float >::lowest() )

{
	for( int i = 0; i < points.size(); ++i )
	{
		Vector4f xyzw = worldMatrix * points[ i ];
		float x = xyzw.x;
		float y = xyzw.y;
		float z = xyzw.z;

		if( x < m_min.x )
		{
			m_min.x = x;
		}
		if( x > m_max.x )
		{
			m_max.x = x;
		}
		if( y < m_min.y )
		{
			m_min.y = y;
		}
		if( y > m_max.y )
		{
			m_max.y = y;
		}
		if( z < m_min.z )
		{
			m_min.z = z;
		}
		if( z > m_max.z )
		{
			m_max.z = z;
		}
	}
}

QString BoundingBox3f::toString() const
{
	QString out;

	out.append( "BoundingBox3f:\n" );
	out.append( "\tmin: " );
	out.append( m_min.toString() );
	out.append( "\n\tmax: " );
	out.append( m_max.toString() );

	return out;
}

Vector3f& BoundingBox3f::minimum()
{
	return m_min;
}

Vector3f& BoundingBox3f::maximum()
{
	return m_max;
}

Vector3f BoundingBox3f::minimum() const
{
	return m_min;
}

Vector3f BoundingBox3f::maximum() const
{
	return m_max;
}

Vector3f BoundingBox3f::range() const
{
	return( m_max - m_min );
}

Vector3f BoundingBox3f::center() const
{
	return( 0.5 * ( m_max + m_min ) );
}

float BoundingBox3f::volume() const
{
	Vector3f r = range();
	return r.x * r.y * r.z;
}

float BoundingBox3f::shortestSideLength() const
{
	Vector3f diameter = range();
	return min( diameter.x, min( diameter.y, diameter.z ) );
}

float BoundingBox3f::longestSideLength() const
{
	Vector3f diameter = range();
	return max( diameter.x, max( diameter.y, diameter.z ) );
}

std::vector< Vector3f > BoundingBox3f::corners() const
{
	std::vector< Vector3f > out( 8 );

	for( int i = 0; i < 8; ++i )
	{
		out[ i ] =
			Vector3f
			(
				( i & 1 ) ? minimum().x : maximum().x,
				( i & 2 ) ? minimum().y : maximum().y,
				( i & 4 ) ? minimum().z : maximum().z
			);
	}

	return out;
}

bool BoundingBox3f::containsPoint( const Vector3f& p ) const
{
	return GeometryUtils::pointInBox( p, minimum(), range() );
}

bool BoundingBox3f::overlaps( const BoundingBox3f& other )
{
	bool bOverlapsInDirection[3];

	Vector3f otherMin = other.minimum();
	Vector3f otherMax = other.maximum();

	for( int i = 0; i < 3; ++i )
	{
		bool bMinInside0 = ( otherMin[i] >= m_min[i] ) && ( otherMin[i] <= m_max[i] );
		bool bMinInside1 = ( m_min[i] >= otherMin[i] ) && ( m_min[i] <= otherMax[i] );

		bool bMaxInside0 = ( otherMax[i] >= m_min[i] ) && ( otherMax[i] <= m_max[i] );
		bool bMaxInside1 = ( m_max[i] >= otherMin[i] ) && ( m_max[i] <= otherMax[i] );

		bool bMinInside = bMinInside0 || bMinInside1;
		bool bMaxInside = bMaxInside0 || bMaxInside1;

		bOverlapsInDirection[i] = bMinInside || bMaxInside;
	}

	return bOverlapsInDirection[0] && bOverlapsInDirection[1] && bOverlapsInDirection[2];
}

bool BoundingBox3f::intersectRay( const Vector3f& origin, const Vector3f& direction )
{
	float tIntersect;
	return intersectRay( origin, direction, tIntersect );
}

bool BoundingBox3f::intersectRay( const Vector3f& origin, const Vector3f& direction,
	float& tIntersect )
{
	float tEnter = 0;
	float tExit = ( std::numeric_limits< float >::max )();

	intersectSlab( origin.x, direction.x, m_min.x, m_max.x, tEnter, tExit );
	intersectSlab( origin.y, direction.y, m_min.y, m_max.y, tEnter, tExit );
	intersectSlab( origin.z, direction.z, m_min.z, m_max.z, tEnter, tExit );

	bool intersected = ( tEnter < tExit );
	if( intersected )
	{
		if( tEnter == 0 )
		{
			tIntersect = tExit;
		}
		else
		{
			tIntersect = std::min( tEnter, tExit );
		}
	}
	return intersected;
}

bool BoundingBox3f::intersectRay( const Vector3f& origin, const Vector3f& direction,
	float& tNear, float& tFar ) const
{
	// compute t to each face
	Vector3f rcpDir = 1.0f / direction;
	
	// three "bottom" faces (min of the box)
	Vector3f tBottom = rcpDir * ( minimum() - origin );
	// three "top" faces (max of the box)
	Vector3f tTop = rcpDir * ( maximum() - origin );

	// find the smallest and largest distances along each axis
	Vector3f tMin = Vector3f::minimum( tBottom, tTop );
	Vector3f tMax = Vector3f::maximum( tBottom, tTop );

	// tNear is the largest tMin
	tNear = tMin.maximum();

	// tFar is the smallest tMax
	tFar = tMax.minimum();
	
	return tFar > tNear;
}

// static
BoundingBox3f BoundingBox3f::unite( const BoundingBox3f& b0, const BoundingBox3f& b1 )
{
    Vector3f b0Min = b0.minimum();
    Vector3f b0Max = b0.maximum();
    Vector3f b1Min = b1.minimum();
    Vector3f b1Max = b1.maximum();

    Vector3f newMin( min( b0Min.x, b1Min.x ), min( b0Min.y, b1Min.y ), min( b0Min.z, b1Min.z ) );
    Vector3f newMax( max( b0Max.x, b1Max.x ), max( b0Max.y, b1Max.y ), max( b0Max.z, b1Max.z ) );

    return BoundingBox3f( newMin, newMax );
}

// static
bool BoundingBox3f::intersect( const BoundingBox3f& b0, const BoundingBox3f& b1 )
{
	for( int i = 0; i < 3; ++i )
	{
		if( b0.minimum()[i] > b1.maximum()[i] ||
			b0.maximum()[i] < b1.minimum()[i] )
		{
			return false;
		}
	}

	return true;	
}

// static
bool BoundingBox3f::intersect( const BoundingBox3f& b0, const BoundingBox3f& b1, BoundingBox3f& intersection )
{
	bool intersects = intersect( b0, b1 );
	if( intersects )
	{
		Vector3f newMin( max( b0.minimum().x, b1.minimum().x ), max( b0.minimum().y, b1.minimum().y ), max( b0.minimum().z, b1.minimum().z ) );
		Vector3f newMax( min( b0.maximum().x, b1.maximum().x ), min( b0.maximum().y, b1.maximum().y ), min( b0.maximum().z, b1.maximum().z ) );
		
		intersection.minimum() = newMin;
		intersection.maximum() = newMax;
	}

	return intersects;
}

void BoundingBox3f::enlarge( const Vector3f& p )
{
	m_min.x = min( p.x, m_min.x );
	m_min.y = min( p.y, m_min.y );
	m_min.z = min( p.z, m_min.z );
	m_max.x = max( p.x, m_max.x );
	m_max.y = max( p.y, m_max.y );
	m_max.z = max( p.z, m_max.z );
}

void BoundingBox3f::scale( const Vector3f& s )
{
	Vector3f c = center();
	Vector3f r = range();	
	Vector3f r2 = s * r;

	m_min = c - 0.5f * r2;
	m_max = m_min + r2;
}

//////////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////////

void BoundingBox3f::intersectSlab( float origin, float direction, float s0, float s1,
	float& tEnter, float& tExit )
{
	float t0 = ( s0 - origin ) / direction;
	float t1 = ( s1 - origin ) / direction;

	if( t0 > t1 )
	{
		std::swap( t0, t1 );
	}

	if( t0 > tEnter )
	{
		tEnter = t0;
	}

	if( t1 < tExit )
	{
		tExit = t1;
	}
}
