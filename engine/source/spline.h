#ifndef _SPLINE_H_
#define _SPLINE_H_

#include "defs.h"
#include "IwGeomFVec2.h"
#include "IwGeomFVec4.h"
#include "vector"

_BEGIN_NAMESPACE_

class SimpleSpline
{
public:
    SimpleSpline();
    ~SimpleSpline();


    void              AddPoint(const CIwFVec2& p);
    const CIwFVec2&   GetPoint(uint16 index) const;
    uint16            GetNumPoints() const;
    void              Clear();
    void              UpdatePoint(uint16 index, const CIwFVec2& value);
    CIwFVec2          Interpolate(float t) const;
    CIwFVec2          Interpolate(uint16 fromIndex, float t) const;
    void              SetAutoCalculate(bool autoCalc);
    void              RecalcTangents();
	void              SetPoints(const std::vector<CIwFVec2> & points);
	const std::vector<CIwFVec2>& GetPoints() const;

protected:
    bool                  mAutoCalc;

	std::vector<CIwFVec2> mPoints;
    std::vector<CIwFVec2> mTangents;
    
    CIwFVec4              mCoeffs[4];

};

_END_NAMESPACE_

#endif // _SPLINE_H_