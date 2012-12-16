#ifndef _DEFS_H_
#define _DEFS_H_

#include "IwDebug.h"
#include "IwString.h"
#include "math.h"
#include "wctype.h"

#define SAFE_DELETE_ARRAY( p )  if (p) { delete[] (p); (p)=NULL; }
#define SAFE_DELETE( p )        if (p) { delete (p); (p)=NULL; }

#define	_BEGIN_NAMESPACE_		namespace fairy {
#define	_END_NAMESPACE_			}
#define	_USE_NAMESPACE_			using namespace fairy
#define	_NAMESPACE_( value )	fairy::value

#ifdef IW_DEBUG
#define APP_ENABLE_EDITOR 0
#else
#define APP_ENABLE_EDITOR 0
#endif // IW_DEBUG

#if APP_ENABLE_EDITOR
	#define  APP_ENABLE_KEYBOARD 1
	#define  APP_SAVE_XML 1
#else
	#define  APP_ENABLE_KEYBOARD 0
	#define  APP_SAVE_XML 0
#endif


#define GAME_CLASS_TYPENAME(a)	\
	virtual const char* GetClassName() const {return #a;}


#define DEGREE_TO_RADIANCE(x) float(x * M_PI/180.f)
#define RADIANCE_TO_DEGREE(x) float((x * 180.f) / M_PI)


inline bool operator < (const CIwStringS & a, const CIwStringS & b)
{
	return strcmp(a.c_str(), b.c_str()) < 0;
}

#endif //_DEFS_H_