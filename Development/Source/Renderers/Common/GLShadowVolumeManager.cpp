/*  NAME:
        GLShadowVolumeManager.cpp

    DESCRIPTION:
        Quesa OpenGL shadow volume caching.
       
    COPYRIGHT:
        Copyright (c) 2011-2018, Quesa Developers. All rights reserved.

        For the current release of Quesa, please see:

            <http://www.quesa.org/>
        
        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:
        
            o Redistributions of source code must retain the above copyright
              notice, this list of conditions and the following disclaimer.
        
            o Redistributions in binary form must reproduce the above
              copyright notice, this list of conditions and the following
              disclaimer in the documentation and/or other materials provided
              with the distribution.
        
            o Neither the name of Quesa nor the names of its contributors
              may be used to endorse or promote products derived from this
              software without specific prior written permission.
        
        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
        TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
        PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
        LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
        NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    ___________________________________________________________________________
*/

//=============================================================================
//      Include files
//-----------------------------------------------------------------------------
#include "GLShadowVolumeManager.h"
#include "GLGPUSharing.h"
#include "GLVBOManager.h"
#include "CQ3WeakObjectRef.h"
#include "GLUtils.h"
#include "E3Main.h"
#include "E3CustomElements.h"

#include <vector>
#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>

//=============================================================================
//		Internal constants
//-----------------------------------------------------------------------------
namespace
{
	const TQ3Uns32	kShadowCacheKey	= Q3_FOUR_CHARACTER_CONSTANT('s', 'v', 'c', 'k');
	
	const TQ3Uns32	kAbsentBuffer	= 0xFFFFFFFFU;

	const TQ3Uns32	kMaxSecondaryTexCoords = 16;
}

#ifndef GL_ARRAY_BUFFER
	#define GL_ARRAY_BUFFER					0x8892
	#define GL_ELEMENT_ARRAY_BUFFER			0x8893
	#define GL_STATIC_DRAW                  0x88E4
#endif

#ifndef GL_ARB_multitexture
	#define GL_TEXTURE0_ARB                   0x84C0
#endif

//#define VALIDATE_COUNTS		1


//=============================================================================
//		Internal types
//-----------------------------------------------------------------------------

#ifndef APIENTRY
	#if QUESA_OS_WIN32
		#define APIENTRY	__stdcall
	#else
		#define APIENTRY
	#endif
#endif


namespace
{
	typedef std::pair< TQ3GeometryObject, TQ3LightObject >	GeomAndLight;
	
	typedef std::map< GeomAndLight, class ShadowVBO* >		GeomLightToShadow;
		
	const TQ3RationalPoint4D	kDummyPoint = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	const float					kMaxLightDistSq	= 7.0e-6f;

	struct SecondaryTexCoord
	{
		TQ3Uns32		mGLBufferName;
		GLint			mCoordsPerVertex;
		GLenum			mTextureUnit;
	};

	/*!
		@class		ShadowVBO
		@abstract	Cached shadow volume VBO.
	*/
	class ShadowVBO
	{
	public:
							ShadowVBO(); // used for linked list sentinels
							ShadowVBO( TQ3GeometryObject inGeom,
										TQ3LightObject inLight,
										const TQ3RationalPoint4D& inLocalLightPos );
							~ShadowVBO();

		bool				IsStale() const;
							
		CQ3WeakObjectRef	mGeomObject;
		TQ3Uns32			mGeomEditIndex;

		CQ3WeakObjectRef	mLightObject;
		TQ3RationalPoint4D	mLocalLightPosition;	// w is 0 or 1
		
		GeomAndLight		mMapKey;

		TQ3Uns32			mNumTriIndices;
		TQ3Uns32			mNumQuadIndices;
		GLuint				mGLBufferNames[2];	// 0 is array, 1 is index
		TQ3Uns32			mBufferBytes;

		TQ3Uns32			mNumSecondaryTexCoords;
		SecondaryTexCoord	mSecondaryTexCoord[kMaxSecondaryTexCoords];
	
	private:
		ShadowVBO*			mPrev;
		ShadowVBO*			mNext;
		
		// Unimplemented; declared privately just to make sure that they don't
		// get used accidentally.
							ShadowVBO( const ShadowVBO& inOther );
		ShadowVBO&			operator=( const ShadowVBO& inOther );
		friend class ShadowVolCache;
	};
	
	
	/*!
		@class		ShadowVolCache
		@abstract	Cache of shadow volume VBOs for a sharing set of OpenGL
					contexts.
	*/
	class ShadowVolCache : public CQ3GPSharedCache
	{
	public:
							ShadowVolCache();
							~ShadowVolCache();

		void				DeleteVBO( ShadowVBO* inCachedVBO, const GLBufferFuncs& inFuncs );
		ShadowVBO*			FindVBO( TQ3GeometryObject inGeom,
									TQ3LightObject inLight,
									const TQ3RationalPoint4D& inLocalLightPos,
									const GLBufferFuncs& inFuncs );
		void				RenderVBO( ShadowVBO* inCachedVBO, const GLBufferFuncs& inFuncs );
		void				AddVBO( ShadowVBO* inVBO );
		void				FlushStaleShadows( const GLBufferFuncs& inFuncs );
		void				MakeRoom( TQ3Uns32 inBytesNeeded, const GLBufferFuncs& inFuncs );
		void				PurgeDownToSize( long long inTargetSize, const GLBufferFuncs& inFuncs );
		void				SetMaxBufferSize( long long inBufferSize, const GLBufferFuncs& inFuncs );
	
	#if VALIDATE_COUNTS
		void				ValidateByteCount() const;
		void				ValidateShadowCount() const;
	#endif
	
	private:
		
		GeomLightToShadow			mGeomLightToShadows;
		ShadowVBO					mListOldEnd;
		ShadowVBO					mListNewEnd;
		long long					mTotalBytes;
		long long					mMaxBufferBytes;
	};
}

#if VALIDATE_COUNTS
	#define VALIDATE_COUNT(cache)	do {\
										cache->ValidateByteCount();		\
										cache->ValidateShadowCount();	\
									} while (false)
#else
	#define VALIDATE_COUNT(cache)
#endif

//=============================================================================
//		Internal functions
//-----------------------------------------------------------------------------

static inline GLvoid* BufferObPtr( GLuint offset )
{
	return (GLvoid*)( ((char*)nullptr) + offset );
}


static ShadowVolCache* GetShadowCache( TQ3GLContext glContext )
{
	ShadowVolCache*	theCache = static_cast<ShadowVolCache*>( GLGPUSharing_GetCache(
		glContext, kShadowCacheKey ) );
	
	if (theCache == nullptr)
	{
		theCache = new(std::nothrow) ShadowVolCache;
		
		if (theCache != nullptr)
		{
			GLGPUSharing_AddCache( glContext, kShadowCacheKey, theCache );
		}
	}
	
	return theCache;
}

static bool IsSameLightPosition( const TQ3RationalPoint4D& a,
								const TQ3RationalPoint4D& b )
{
	TQ3RationalPoint4D d =
	{
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	};
	float distSq = d.x * d.x + d.y * d.y + d.z * d.z + d.w * d.w;
	return distSq < kMaxLightDistSq;
}


#pragma mark -

ShadowVBO::ShadowVBO()
	: mGeomObject()
	, mLightObject()
	, mBufferBytes( 0 )
	, mPrev( nullptr )
	, mNext( nullptr )
{
}

#if 0//Q3_DEBUG
static long sShadowVBOCount = 0;
#endif

ShadowVBO::ShadowVBO(					TQ3GeometryObject inGeom,
										TQ3LightObject inLight,
										const TQ3RationalPoint4D& inLocalLightPos )
	: mGeomObject( inGeom )
	, mGeomEditIndex( Q3Shared_GetEditIndex( inGeom ) )
	, mLightObject( inLight )
	, mLocalLightPosition( inLocalLightPos )
	, mMapKey( inGeom, inLight )
	, mBufferBytes( 0 )
	, mNumSecondaryTexCoords( 0 )
	, mPrev( nullptr )
	, mNext( nullptr )
{
	// Leave other fields uninitialized for now
	//Q3_MESSAGE_FMT("ShadowVBO created, %ld", ++sShadowVBOCount );
}

ShadowVBO::~ShadowVBO()
{
	//Q3_MESSAGE_FMT("ShadowVBO destroyed, %ld", --sShadowVBOCount);
}

bool	ShadowVBO::IsStale() const
{
	bool stale = true;
	
	if (mGeomObject.isvalid() && mLightObject.isvalid())
	{
		TQ3Uns32 curEditIndex = static_cast<E3Shared*>( mGeomObject.get() )->GetEditIndex();
		if (curEditIndex == mGeomEditIndex)
		{
			stale = false;
		}
	}

	return stale;
}

#pragma mark -

ShadowVolCache::ShadowVolCache()
	: mTotalBytes( 0 )
	, mMaxBufferBytes( 0 )
{
	mListOldEnd.mNext = &mListNewEnd;
	mListNewEnd.mPrev = &mListOldEnd;
	VALIDATE_COUNT(this);
}

ShadowVolCache::~ShadowVolCache()
{
	// Note:  This object will only be destroyed after the last OpenGL context
	// of its sharing group has gone away.  Hopefully, this will automatically
	// clear the VBOs from VRAM.  Hence, we do not need or want to call DeleteVBO,
	// but we do want to delete the ShadowVBO objects.
	ShadowVBO* listItem = mListOldEnd.mNext;
	
	while (listItem != &mListNewEnd)
	{
		ShadowVBO* toDelete = listItem;
		listItem = listItem->mNext;
		delete toDelete;
	}
}

#if VALIDATE_COUNTS
void	ShadowVolCache::ValidateByteCount() const
{
	long long trueTotal = 0;
	ShadowVBO* listPtr;
	for (listPtr = mListOldEnd.mNext; listPtr != &mListNewEnd; listPtr = listPtr->mNext)
	{
		trueTotal += listPtr->mBufferBytes;
	}
	Q3_ASSERT_FMT( trueTotal == mTotalBytes, "Actual shadow VBO byte count %lld != nominal %lld",
		 trueTotal, mTotalBytes );
}

void	ShadowVolCache::ValidateShadowCount() const
{
	size_t listCount = 0;
	ShadowVBO* listPtr;
	for (listPtr = mListOldEnd.mNext; listPtr != &mListNewEnd; listPtr = listPtr->mNext)
	{
		++listCount;
	}
	
	size_t mapCount = mGeomLightToShadows.size();
	Q3_ASSERT_FMT( mapCount == listCount, "Shadow cache list has %d shadows, "
		"map has %d shadows", (int)listCount, (int)mapCount );
}
#endif

void	ShadowVolCache::DeleteVBO( ShadowVBO* inCachedVBO, const GLBufferFuncs& inFuncs )
{
	// Remove the buffers from VRAM.
	if ( inCachedVBO->mNumTriIndices + inCachedVBO->mNumQuadIndices > 0)
	{
		(*inFuncs.glDeleteBuffersProc)( 2, inCachedVBO->mGLBufferNames );
		ForgetVBO( inCachedVBO->mGLBufferNames[0] );
		ForgetVBO( inCachedVBO->mGLBufferNames[1] );
	}
	for (TQ3Uns32 i = 0; i < inCachedVBO->mNumSecondaryTexCoords; ++i)
	{
		(*inFuncs.glDeleteBuffersProc)( 1, &inCachedVBO->mSecondaryTexCoord[i].mGLBufferName );
		ForgetVBO( inCachedVBO->mSecondaryTexCoord[i].mGLBufferName );
	}
	
	// Remove the record from the doubly-linked list.
	inCachedVBO->mPrev->mNext = inCachedVBO->mNext;
	inCachedVBO->mNext->mPrev = inCachedVBO->mPrev;
	
	mTotalBytes -= inCachedVBO->mBufferBytes;
	
	delete inCachedVBO;
}


void	ShadowVolCache::AddVBO( ShadowVBO* inVBO )
{
	VALIDATE_COUNT(this);
	
	// Add the new record to our map
	GeomAndLight key( inVBO->mGeomObject.get(), inVBO->mLightObject.get() );
	
	GeomLightToShadow::iterator found = mGeomLightToShadows.find( key );
	
	Q3_ASSERT_FMT( found == mGeomLightToShadows.end(), "Adding shadow VBO without deleting a stale one" );
	
	mGeomLightToShadows[ key ] = inVBO;

	// Insert the new record at the new end of a doubly-linked list.
	inVBO->mNext = &mListNewEnd;
	inVBO->mPrev = mListNewEnd.mPrev;
	inVBO->mPrev->mNext = inVBO;
	mListNewEnd.mPrev = inVBO;
	
	mTotalBytes += inVBO->mBufferBytes;
	
	VALIDATE_COUNT(this);
	
	//Q3_MESSAGE_FMT("Shadow cache %lld bytes", mTotalBytes );
}


ShadowVBO*	ShadowVolCache::FindVBO( TQ3GeometryObject inGeom,
									TQ3LightObject inLight,
									const TQ3RationalPoint4D& inLocalLightPos,
									const GLBufferFuncs& inFuncs )
{
	ShadowVBO* cachedShadow = nullptr;
	VALIDATE_COUNT(this);
	
	GeomAndLight key( inGeom, inLight );
	
	GeomLightToShadow::iterator found = mGeomLightToShadows.find( key );
	
	if (found != mGeomLightToShadows.end())
	{
		cachedShadow = found->second;
		
		if ( cachedShadow->IsStale() ||
			(! IsSameLightPosition( cachedShadow->mLocalLightPosition, inLocalLightPos )) )
		{
			DeleteVBO( cachedShadow, inFuncs );
			mGeomLightToShadows.erase( found );
			cachedShadow = nullptr;
		}
	}
	VALIDATE_COUNT(this);
	
	return cachedShadow;
}

void	ShadowVolCache::RenderVBO( ShadowVBO* inCachedVBO, const GLBufferFuncs& inFuncs )
{
	if ( inCachedVBO->mNumTriIndices + inCachedVBO->mNumQuadIndices > 0)
	{
		(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER, inCachedVBO->mGLBufferNames[0] );
		glVertexPointer( 4, GL_FLOAT, 0, BufferObPtr( 0 ) );

		TQ3Uns32 i;
		if (inCachedVBO->mNumSecondaryTexCoords > 0)
		{
			for (i = 0; i < inCachedVBO->mNumSecondaryTexCoords; ++i)
			{
				(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER, inCachedVBO->mSecondaryTexCoord[i].mGLBufferName );
				(*inFuncs.glClientActiveTextureProc)( inCachedVBO->mSecondaryTexCoord[i].mTextureUnit );
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );
				glTexCoordPointer( inCachedVBO->mSecondaryTexCoord[i].mCoordsPerVertex, GL_FLOAT, 0,
					BufferObPtr( 0 ) );
			}
			(*inFuncs.glClientActiveTextureProc)( GL_TEXTURE0_ARB );
		}

		(*inFuncs.glBindBufferProc)( GL_ELEMENT_ARRAY_BUFFER,
			inCachedVBO->mGLBufferNames[1] );
		
		if (inCachedVBO->mNumTriIndices > 0)
		{
			glDrawElements( GL_TRIANGLES, inCachedVBO->mNumTriIndices,
				GL_UNSIGNED_INT, BufferObPtr( 0 ) );
		}

		if (inCachedVBO->mNumQuadIndices > 0)
		{
			glDrawElements( GL_QUADS, inCachedVBO->mNumQuadIndices,
				GL_UNSIGNED_INT,
				BufferObPtr( inCachedVBO->mNumTriIndices * sizeof(GLuint) ) );
		}

		(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER, 0 );
		(*inFuncs.glBindBufferProc)( GL_ELEMENT_ARRAY_BUFFER, 0 );

		if (inCachedVBO->mNumSecondaryTexCoords > 0)
		{
			for (i = 0; i < inCachedVBO->mNumSecondaryTexCoords; ++i)
			{
				(*inFuncs.glClientActiveTextureProc)( inCachedVBO->mSecondaryTexCoord[i].mTextureUnit );
				glDisableClientState( GL_TEXTURE_COORD_ARRAY );
				(*inFuncs.glMultiTexCoord1fProc)( inCachedVBO->mSecondaryTexCoord[i].mTextureUnit, 0.0f );
			}
			(*inFuncs.glClientActiveTextureProc)( GL_TEXTURE0_ARB );
		}
	}
	
	// Remove the record from the doubly-linked list.
	inCachedVBO->mPrev->mNext = inCachedVBO->mNext;
	inCachedVBO->mNext->mPrev = inCachedVBO->mPrev;

	// Move it to the new end of the list
	inCachedVBO->mNext = &mListNewEnd;
	inCachedVBO->mPrev = mListNewEnd.mPrev;
	inCachedVBO->mPrev->mNext = inCachedVBO;
	mListNewEnd.mPrev = inCachedVBO;
	VALIDATE_COUNT(this);
}

void	ShadowVolCache::FlushStaleShadows( const GLBufferFuncs& inFuncs )
{
	VALIDATE_COUNT(this);
	GeomLightToShadow::iterator endMap = mGeomLightToShadows.end();
	GeomLightToShadow::iterator i = mGeomLightToShadows.begin();
	
	while (i != endMap)
	{
		// Get next iterator now, in case we invalidate the current one.
		GeomLightToShadow::iterator next = i;
		++next;
		
		ShadowVBO* theShadow = i->second;
		
		if (theShadow->IsStale())
		{
			DeleteVBO( theShadow, inFuncs );
			mGeomLightToShadows.erase( i );
		}
		
		i = next;
	}

	VALIDATE_COUNT(this);
}


void	ShadowVolCache::MakeRoom( TQ3Uns32 inBytesNeeded, const GLBufferFuncs& inFuncs )
{
	if ( (inBytesNeeded < mMaxBufferBytes) &&
		(mTotalBytes + inBytesNeeded > mMaxBufferBytes) )
	{
		long long targetSize = mMaxBufferBytes - inBytesNeeded;
		
		PurgeDownToSize( targetSize, inFuncs );
	}
}

void	ShadowVolCache::PurgeDownToSize( long long inTargetSize, const GLBufferFuncs& inFuncs )
{
	VALIDATE_COUNT(this);
	while ( (mTotalBytes > inTargetSize) && (mListOldEnd.mNext != &mListNewEnd) )
	{
		// Find the least recently used VBO from the doubly-linked list
		ShadowVBO* oldestVBO = mListOldEnd.mNext;
		
		// Remove it from the map index
		mGeomLightToShadows.erase( oldestVBO->mMapKey );
		
		// Remove it from the doubly linked list, remove the VBOs, update
		// mTotalBytes, and delete the ShadowVBO record.
		DeleteVBO( oldestVBO, inFuncs );
	}
	VALIDATE_COUNT(this);
}

void	ShadowVolCache::SetMaxBufferSize( long long inBufferSize, const GLBufferFuncs& inFuncs )
{
	if (inBufferSize < mMaxBufferBytes)
	{
		PurgeDownToSize( inBufferSize, inFuncs );
	}
	
	mMaxBufferBytes = inBufferSize;
}

#pragma mark -

/*!
	@function	StartFrame
	@abstract	Update the limit on memory that can be used in this cache.
	@param			glContext		An OpenGL context.
	@param			inFuncs			OpenGL buffer function pointers.
	@param			memLimitK		New memory limit in K-bytes.
*/
void	ShadowVolMgr::StartFrame(	TQ3GLContext glContext,
									const GLBufferFuncs& inFuncs,
									TQ3Uns32 memLimitK )
{
	ShadowVolCache*	theCache = GetShadowCache( glContext );
	
	if (theCache != nullptr)
	{
		theCache->SetMaxBufferSize( memLimitK * 1024LL, inFuncs );
	}
}

/*!
	@function	RenderShadowVolume
	@abstract	Look for a cached shadow volume for a given geometry, OpenGL
				context, and shadow-casting light.  If we find one, render it.
	@discussion		If we find the object in the cache, but the cached object
					is stale, we delete it from the cache and return false.
	@param			glContext		An OpenGL context.
	@param			inFuncs			OpenGL buffer function pointers.
	@param			inGeom			A geometry object.
	@param			inLight			A light object.
	@param			inLocalLightPos	The position of the light in local coordinates.
	@result			True if the object was found and rendered.
*/
TQ3Boolean		ShadowVolMgr::RenderShadowVolume(
									TQ3GLContext glContext,
									const GLBufferFuncs& inFuncs,
									TQ3GeometryObject inGeom,
									TQ3LightObject inLight,
									const TQ3RationalPoint4D& inLocalLightPos )
{
	TQ3Boolean	didRender = kQ3False;
	ShadowVolCache*	theCache = GetShadowCache( glContext );
	
	if (theCache != nullptr)
	{
		ShadowVBO* theVBO = theCache->FindVBO( inGeom, inLight, inLocalLightPos, inFuncs );
		
		if (theVBO != nullptr)
		{
			theCache->RenderVBO( theVBO, inFuncs );
			didRender = kQ3True;
		}
	}
	
	return didRender;
}

/*!
	@function		AddSecondaryTextureCoordsToVBO
	@abstract		Add an array of secondary texture coordinates to an
					already-cached VBO, as an optional follow-up to
					AddVBOToCache.
	@param			inCache				A VBO cache.
	@param			ioVBO				A new VBO object not yet added to the cache.
	@param			inTextureUnit		A texture unit to which we should supply
										the array, e.g., GL_TEXTURE1_ARB.
	@param			inNumPoints			Number of points (vertices).
	@param			inNumCoordsPerPoint	Number of floating-point values for each
										vertex.  Must be 1, 2, 3, or 4.
	@param			inInfiniteLight		Whether the light is infinite (directional).
	@param			inData				Array of data.  The number of floating-point
										values should be inNumPoints * inNumCoordsPerPoint.
*/
static void			AddSecondaryTextureCoordsToVBO(
									const GLBufferFuncs& inFuncs,
									ShadowVolCache* inCache,
									ShadowVBO* ioVBO,
									GLenum inTextureUnit,
									TQ3Uns32 inNumPoints,
									TQ3Uns32 inNumCoordsPerPoint,
									bool inInfiniteLight,
									const float* inData )
{
	if ( (ioVBO != nullptr) && (ioVBO->mNumSecondaryTexCoords < kMaxSecondaryTexCoords) )
	{
		TQ3Uns32	coordIndex = ioVBO->mNumSecondaryTexCoords;
		ioVBO->mNumSecondaryTexCoords += 1;
		SecondaryTexCoord& theCoord( ioVBO->mSecondaryTexCoord[ coordIndex ] );
		(*inFuncs.glGenBuffersProc)( 1, &theCoord.mGLBufferName );
		theCoord.mCoordsPerVertex = (GLint) inNumCoordsPerPoint;
		theCoord.mTextureUnit = inTextureUnit;
		
		TQ3Uns32 finitePointDataSize = inNumPoints * inNumCoordsPerPoint * sizeof(float);
		TQ3Uns32 infinitePointDataSize;
		if (inInfiniteLight)
		{
			infinitePointDataSize = inNumCoordsPerPoint * sizeof(float);
		}
		else // finite light, each finite point has a matching infinite point
		{
			infinitePointDataSize = finitePointDataSize;
		}
		TQ3Uns32 dataSize = finitePointDataSize + infinitePointDataSize;
		ioVBO->mBufferBytes += dataSize;
		inCache->MakeRoom( dataSize, inFuncs );
		
		std::vector<float> zeroBuf( infinitePointDataSize / sizeof(float) );
		
		(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER, theCoord.mGLBufferName );
		(*inFuncs.glBufferDataProc)( GL_ARRAY_BUFFER, dataSize,
			nullptr, GL_STATIC_DRAW );
		(*inFuncs.glBufferSubDataProc)( GL_ARRAY_BUFFER, 0, finitePointDataSize,
			inData );
		(*inFuncs.glBufferSubDataProc)( GL_ARRAY_BUFFER, finitePointDataSize,
			infinitePointDataSize, &zeroBuf[0] );
		RecordVBO( theCoord.mGLBufferName, ioVBO->mGeomObject.get(), dataSize, 3 );
		
		(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER, 0 );
	}
}

/*!
	@function		HandleSecondaryTextureCoords
	@abstract		Check for secondary texture coordinates attached to the geometry.
	@param			inGeom				A geometry object.
	@param			inCache				A VBO cache.
	@param			ioVBO				A new VBO object not yet added to the cache.
	@param			inInfiniteLight		Whether the light is infinite (directional).
*/
static void HandleSecondaryTextureCoords(
									TQ3GeometryObject inGeom,
									const GLBufferFuncs& inFuncs,
									ShadowVolCache* inCache,
									ShadowVBO* ioVBO,
									bool inInfiniteLight )
{
	TQ3Uns32 bufSize;
	if ( (kQ3Success == Q3Object_GetProperty( inGeom,
		kQ3GeometryPropertyCustomTextureCoordinates, 0, &bufSize, nullptr )) &&
		((bufSize % 4) == 0) &&
		bufSize >= sizeof(TQ3CustomTextureCoordinates) )
	{
		std::vector<TQ3Uns8>	buffer( bufSize );
		Q3Object_GetProperty( inGeom, kQ3GeometryPropertyCustomTextureCoordinates,
			bufSize, nullptr, &buffer[0] );
		TQ3CustomTextureCoordinates* tcRec(
			reinterpret_cast<TQ3CustomTextureCoordinates*>( &buffer[0] ) );
		GLenum textureUnit = GL_TEXTURE0_ARB + tcRec->textureUnit;
		AddSecondaryTextureCoordsToVBO( inFuncs, inCache, ioVBO, textureUnit,
			tcRec->numPoints, tcRec->coordsPerPoint, inInfiniteLight, tcRec->coords );
	}
}

/*!
	@function	AddShadowVolume
	@abstract	Add a shadow volume mesh to the cache.  Do not call this unless
				RenderCachedShadowVolume has just returned false.
	@param			glContext			An OpenGL context.
	@param			inFuncs			OpenGL buffer function pointers.
	@param			inGeom				A geometry object.
	@param			inLight				A light object.
	@param			inLocalLightPos		The position of the light in local coordinates.
	@param			inNumPoints			Number of points (vertices).
	@param			inPoints			Array of point locations.
	@param			inNumTriIndices		Number of triangle vertex indices to follow.
	@param			inNumQuadIndices	Number of quad vertex indices to follow.
	@param			inVertIndices		Array of vertex indices (triangle, then quad).
*/
void	ShadowVolMgr::AddShadowVolume(
									TQ3GLContext glContext,
									const GLBufferFuncs& inFuncs,
									TQ3GeometryObject inGeom,
									TQ3LightObject inLight,
									const TQ3RationalPoint4D& inLocalLightPos,
									TQ3Uns32 inNumPoints,
									const TQ3RationalPoint4D* inPoints,
									TQ3Uns32 inNumTriIndices,
									TQ3Uns32 inNumQuadIndices,
									const GLuint* inVertIndices )
{
	ShadowVolCache*	theCache = GetShadowCache( glContext );
	
	if (theCache != nullptr)
	{
		VALIDATE_COUNT(theCache);
		ShadowVBO* newVBO = new ShadowVBO( inGeom, inLight, inLocalLightPos );
		
		newVBO->mNumTriIndices = inNumTriIndices;
		newVBO->mNumQuadIndices = inNumQuadIndices;
		
		if (inNumTriIndices + inNumQuadIndices > 0)
		{
			newVBO->mBufferBytes = inNumPoints * sizeof(TQ3RationalPoint4D) +
				(inNumTriIndices + inNumQuadIndices) * sizeof(GLuint);
			theCache->MakeRoom( newVBO->mBufferBytes, inFuncs );

			// Get buffer names
			(*inFuncs.glGenBuffersProc)( 2, newVBO->mGLBufferNames );
			
			// Copy point data into array buffer.
			(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER,
				newVBO->mGLBufferNames[0] );
			(*inFuncs.glBufferDataProc)( GL_ARRAY_BUFFER,
				inNumPoints * sizeof(TQ3RationalPoint4D),
				inPoints, GL_STATIC_DRAW );
			GLenum error = glGetError();
			if (error == GL_OUT_OF_MEMORY)
			{
				DumpVBOs();
				Q3_ASSERT_FMT( false, "Failed to allocate shadow VBO");
			}
			(*inFuncs.glBindBufferProc)( GL_ARRAY_BUFFER, 0 );
			RecordVBO( newVBO->mGLBufferNames[0], inGeom, inNumPoints * sizeof(TQ3RationalPoint4D), 2 );
			
			// Index data into elements buffer.
			(*inFuncs.glBindBufferProc)( GL_ELEMENT_ARRAY_BUFFER,
				newVBO->mGLBufferNames[1] );
			(*inFuncs.glBufferDataProc)( GL_ELEMENT_ARRAY_BUFFER,
				(inNumTriIndices + inNumQuadIndices) * sizeof(GLuint),
				inVertIndices, GL_STATIC_DRAW );
			(*inFuncs.glBindBufferProc)( GL_ELEMENT_ARRAY_BUFFER, 0 );
			RecordVBO( newVBO->mGLBufferNames[1], inGeom, (inNumTriIndices + inNumQuadIndices) * sizeof(GLuint), 2 );
			
			HandleSecondaryTextureCoords( inGeom, inFuncs, theCache, newVBO,
				inLocalLightPos.w == 0.0f );
		}
		
		theCache->AddVBO( newVBO );
		VALIDATE_COUNT(theCache);
	}
}


/*!
	@function		Flush
	@abstract		Delete any cached VBOs for geometries that are no longer
					referenced elsewhere, or lights that are no longer
					referenced elsewhere.
	@param			glContext		An OpenGL context.
	@param			inFuncs			OpenGL buffer function pointers.
	@param			inRenderer		A Quesa renderer.
*/
void	ShadowVolMgr::Flush(
									TQ3GLContext glContext,
									const GLBufferFuncs& inFuncs,
									TQ3RendererObject inRenderer )
{
	ShadowVolCache*	theCache = GetShadowCache( glContext );
	
	if (theCache != nullptr)
	{
		theCache->FlushStaleShadows( inFuncs );
	}
}

