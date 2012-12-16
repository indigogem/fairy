#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "defs.h"

#include "sprite.h"
#include "smartptr.h"
#include "tinyxml.h"
#include "vector"
#include "map"

_BEGIN_NAMESPACE_

const int GAME_MAX_NAME_LENGTH = 64;


class CLevel;
class CGameObject;
typedef shared_ptr<CGameObject> GameObjectPtr;

typedef uint32 ObjectId;

enum EVariantType
{
	INVALID,
	INT,
	FLOAT,
	STRING
};

struct SVarTypeStr
{
	const char * str;
	EVariantType type;
};
const SVarTypeStr gVarTypeToStr[] = 
{
	{ "int",    INT    },
	{ "float",  FLOAT  },
	{ "string", STRING }
};

const int gVarTypeToStrSize = sizeof(gVarTypeToStr)/sizeof(gVarTypeToStr[0]);

inline EVariantType GetVarType(const char * name)
{
	for ( int i = 0; i < gVarTypeToStrSize; i++ )
	{
		if ( strcasecmp( gVarTypeToStr[i].str, name ) == 0 )
		{
			return gVarTypeToStr[i].type;
		}
	}

	IwAssertMsg(GAME, false, ("GetVarType: Invalid variant type"));
	return INVALID;
}

inline const char * GetVarName(EVariantType type)
{
	for ( int i = 0; i < gVarTypeToStrSize; i++ )
	{
		if ( gVarTypeToStr[i].type == type )
		{
			return gVarTypeToStr[i].str;
		}
	}

	IwAssertMsg(GAME, false, ("GetVarName: Invalid variant type"));
	return "";
}

struct SVariant
{
	int          i;
	float        f;
	CIwStringS   str;
	CIwStringS   name;

	EVariantType type;
};

class CGameObject : public ref_counter
{
public:

	GAME_CLASS_TYPENAME(CGameObject)

	CGameObject(CLevel * level);
	CGameObject( const CGameObject& obj );
	virtual ~CGameObject();

	virtual void            Draw();
	virtual void            Update(float dt);
	virtual void            Init();
	virtual void            Deinit();
	virtual void            Load(TiXmlElement * objElem);
	virtual CGameObject *   Clone() const;
	virtual void            SetVisible( bool isVisible );
	
	virtual void            OnClick();
	virtual void            OnDrag(int32 x, int32 y);
	virtual void            OnDblClick();


#if APP_SAVE_XML	
	virtual void            Save(TiXmlElement * root);
	void                    SetSave(bool isSave) { mIsSave = isSave; }
	bool                    IsSaveable() const { return mIsSave; }
#endif // APP_SAVE_XML
	
	bool           			IsDead() const;
	void           			Kill();
	void           			SetActive( bool isActive );
	
	CIwFVec2       			GetPos() const;
	float          			GetRot() const;
	SpritePtr      			GetSprite( size_t id ) const;
	SpritePtr      			GetSprite( CIwStringS name ) const;
	template<class T>
	shared_ptr<T>           GetSprite(CIwStringS name) const
	{
		SpritesContainer::const_iterator it = mSprites.begin(), ite = mSprites.end();
		for (; it != ite; it++ )
		{
			if ( (*it)->name.size() > 0 && (*it)->name == name )
			{
				return shared_ptr_dcast<T, CSprite>((*it)->sprt);
			}
		}

		return NULL;
	}
	void                    AddSprite(const SSpriteParams & params);

	void                    SetPause(bool isPaused);

	void                    SetRotateSprite( float rot, size_t id );
	void                    SetPosSprite( float x, float y, size_t id );
	void                    SetVisibleSprite( bool isVisible, size_t id);
	void                    SetVisibleSprite( bool isVisible, const char * name );
	void                    SetName( const char * name );


	CIwStringS     			GetName() const;
	CIwFMat2D      			GetCachedTransform() const;

	virtual void   			SetPos( CIwFVec2 pos );
	virtual bool   			IsHitTest( int32 x, int32 y, bool isPhysFirst = true );

	void           			SetRot( float angle );
	void                    SetZ(float z);
	float          			GetZ() const;
	bool           			IsActive() const;
	bool                    IsVisible() const;
	void                    SetParam( const char * name, const char * type, const char * val );	
	bool                    GetParamFloat(const char * name, float & val);
	bool                    GetParamInt(const char * name, int & val);
	bool                    GetParamStr(const char * name, CIwStringS & str);
	void                    SetAlpha(float a);
	void                    Disappear( float time );
	ObjectId                GetId() const;


	class SSpriteDef : public ref_counter
	{
	public:
		SSpriteDef(){}
		virtual ~SSpriteDef(){};
		SSpriteDef(const SSpriteDef& s)
		{
			sprt      = (s.sprt)->Clone();
			pos       = s.pos;
			rot       = s.rot;
			isVisible = s.isVisible;
			name      = s.name;
		#if APP_SAVE_XML
			initParams = s.initParams;
		#endif // APP_SAVE_XML
		}
		virtual SSpriteDef* Clone()
		{
			return new SSpriteDef( *this );
		}
		SpritePtr  sprt;
		CIwFVec2   pos;
		float      rot;
		bool       isVisible;
		CIwStringS name;

#if APP_SAVE_XML
		SSpriteParams initParams;
#endif // APP_SAVE_XML
	};

	typedef shared_ptr<SSpriteDef> SpriteDefPtr;

	typedef std::vector<SpriteDefPtr> SpritesContainer;
	
	//typedef std::map<CIwStringS, SVariant> VariantMap;
	typedef std::map<uint32, SVariant> VariantMap;
	

protected:
	void          UpdateTransform();
	SVariant      CreateVariant(const char * name, const char * type, const char * val);
	void          VariantToStr( const SVariant & v, char * buff, int size  );
	bool          FindParam(const char * name, SVariant & v);
	ObjectId      GetUniqueId();

	static ObjectId mIds;

protected:
	SpritesContainer                mSprites;
	bool                            mIsDead;
	bool                            mIsActive;
	bool                            mIsVisible;
	bool                            mIsPaused;
	CIwFVec2                        mPos;
	float                           mZ;
	CIwStringS                      mName;
	CLevel *                        mLevel;
	float                           mRot;
	CIwFMat2D                       mCacheTransform;
	CIwStringS                      mTemplateName;
	VariantMap                      mParams;

	bool            				mIsDisappearing;
	float           				mDisappearingTime;
	float           				mDisappearingInitTime;
	ObjectId                        mId;

#if APP_SAVE_XML
	bool                            mIsSave;
#endif
	
};


_END_NAMESPACE_
#endif // _GAME_OBJECT_H_