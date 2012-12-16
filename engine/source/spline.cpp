#include "spline.h"

_BEGIN_NAMESPACE_


SimpleSpline::SimpleSpline()
{
	mCoeffs[0] = CIwFVec4(  2, -2,  1,  1 );
	mCoeffs[1] = CIwFVec4( -3,  3, -2, -1 );
	mCoeffs[2] = CIwFVec4(  0,  0,  1,  0 );
	mCoeffs[3] = CIwFVec4(  1,  0,  0,  0 );

	mAutoCalc  = true;
}

SimpleSpline::~SimpleSpline()
{

}

void SimpleSpline::AddPoint( const CIwFVec2& p )
{
	mPoints.push_back(p);
	if (mAutoCalc)
	{
		RecalcTangents();
	}
}

const CIwFVec2& SimpleSpline::GetPoint( uint16 index ) const
{
	IwAssertMsg(GAME, index < mPoints.size(),("SimpleSpline::GetPoint: Point index is out of bounds!!"));

	return mPoints[index];
}

uint16 SimpleSpline::GetNumPoints() const
{
	return mPoints.size();
}

void SimpleSpline::Clear()
{
	mPoints.clear();
	mTangents.clear();
}

void SimpleSpline::UpdatePoint( uint16 index, const CIwFVec2& value )
{
	IwAssertMsg(GAME, index < mPoints.size(),("SimpleSpline::UpdatePoint: Point index is out of bounds!!"));

	mPoints[index] = value;
	if (mAutoCalc)
	{
		RecalcTangents();
	}
}

CIwFVec2 SimpleSpline::Interpolate( float t ) const
{
	// Currently assumes points are evenly spaced, will cause velocity
	// change where this is not the case

	float fSeg = t * (mPoints.size() - 1);
	uint32 segIdx = (uint32)fSeg;
	// Apportion t 
	t = fSeg - segIdx;

	return Interpolate(segIdx, t);
}

CIwFVec2 SimpleSpline::Interpolate( uint16 fromIndex, float t ) const
{
	IwAssertMsg(GAME, fromIndex < mPoints.size(), ("SimpleSpline::Interpolate: fromIndex out of bounds"));

	if ((fromIndex + 1) == (uint16)mPoints.size())
	{
		// Duff request, cannot blend to nothing
		// Just return source
		return mPoints[fromIndex];

	}

	// Fast special cases
	if (t == 0.0f)
	{
		return mPoints[fromIndex];
	}
	else if(t == 1.0f)
	{
		return mPoints[fromIndex + 1];
	}

	// Real interpolation
	// Form a vector of powers of t
	float t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	CIwFVec4 p(t3, t2, t, 1);


	// Algorithm is ret = powers * mCoeffs * Matrix4(point1, point2, tangent1, tangent2)
	const CIwFVec2& point1 = mPoints[fromIndex];
	const CIwFVec2& point2 = mPoints[fromIndex+1];
	const CIwFVec2& tan1 = mTangents[fromIndex];
	const CIwFVec2& tan2 = mTangents[fromIndex+1];
	CIwFVec4 pt[4];

	pt[0][0] = point1.x;
	pt[0][1] = point1.y;
	pt[0][2] = 0;
	pt[0][3] = 1.0f;

	pt[1][0] = point2.x;
	pt[1][1] = point2.y;
	pt[1][2] = 0;
	pt[1][3] = 1.0f;

	pt[2][0] = tan1.x;
	pt[2][1] = tan1.y;
	pt[2][2] = 0;
	pt[2][3] = 1.0f;

	pt[3][0] = tan2.x;
	pt[3][1] = tan2.y;
	pt[3][2] = 0;
	pt[3][3] = 1.0f;


	//Vector4 ret = powers * mCoeffs * pt;

	// powers * mCoeffs
	CIwFVec4 r1;
	for ( int i = 0; i < 4; i++ )
	{
		r1[i] = p[0]*mCoeffs[0][i] + p[1]*mCoeffs[1][i] + p[2]*mCoeffs[2][i] + p[3]*mCoeffs[3][i];
	}
	// r1 * pt


	CIwFVec4 ret;
	for ( int i = 0; i < 4; i++ )
	{
		ret[i] = r1[0]*pt[0][i] + r1[1]*pt[1][i] + r1[2]*pt[2][i] + r1[3]*pt[3][i];
	}

	return CIwFVec2(ret.x, ret.y);
}

void SimpleSpline::SetAutoCalculate( bool autoCalc )
{
	mAutoCalc = autoCalc;
}

void SimpleSpline::RecalcTangents()
{
	size_t i, numPoints;
	bool isClosed;

	numPoints = mPoints.size();
	if (numPoints < 2)
	{
		// Can't do anything yet
		return;
	}

	// Closed or open?
	if (mPoints[0] == mPoints[numPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}

	mTangents.resize(numPoints);



	for(i = 0; i < numPoints; ++i)
	{
		if (i ==0)
		{
			// Special case start
			if (isClosed)
			{
				// Use numPoints-2 since numPoints-1 is the last point and == [0]
				mTangents[i] = 0.5 * (mPoints[1] - mPoints[numPoints-2]);
			}
			else
			{
				mTangents[i] = 0.5 * (mPoints[1] - mPoints[0]);
			}
		}
		else if (i == numPoints-1)
		{
			// Special case end
			if (isClosed)
			{
				// Use same tangent as already calculated for [0]
				mTangents[i] = mTangents[0];
			}
			else
			{
				mTangents[i] = 0.5 * (mPoints[i] - mPoints[i-1]);
			}
		}
		else
		{
			mTangents[i] = 0.5 * (mPoints[i+1] - mPoints[i-1]);
		}

	}
}

const std::vector<CIwFVec2>& SimpleSpline::GetPoints() const
{
	return mPoints;
}

void SimpleSpline::SetPoints( const std::vector<CIwFVec2> & points )
{
	mPoints = points;
	if ( mAutoCalc )
	{
		RecalcTangents();
	}
}

_END_NAMESPACE_