/*  NAME:
        IRUpdate.c

    DESCRIPTION:
        Quesa interactive renderer update methods.

    COPYRIGHT:
        Copyright (c) 1999-2014, Quesa Developers. All rights reserved.

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
#include "IRPrefix.h"
#include "IRUpdate.h"
#include "IRTexture.h"
#include "IRTriBuffer.h"

#include "GLPrefix.h"
#include "GLCamera.h"
#include "GLDrawContext.h"
#include "GLUtils.h"

#include <algorithm>




//=============================================================================
//      Internal constants
//-----------------------------------------------------------------------------
#define kAAPointSize									0.5f
#define kAALineSize										0.5f


#ifndef	GL_SAMPLE_BUFFERS_ARB
	#define	GL_SAMPLE_BUFFERS_ARB			0x80A8
	#define	GL_MULTISAMPLE_ARB				0x809D
#endif




//=============================================================================
//      Internal functions
//-----------------------------------------------------------------------------
//      ir_state_reset : Reset our state to the defaults.
//-----------------------------------------------------------------------------
static void
ir_state_reset(TQ3InteractiveData *instanceData)
{	TQ3Uns32		n;



    // Reset our state
    instanceData->stateFill          = kQ3FillStyleFilled;
    instanceData->stateHilight       = nullptr;
    instanceData->stateInterpolation = kQ3InterpolationStyleVertex;
    instanceData->stateBackfacing    = kQ3BackfacingStyleBoth;
    instanceData->stateOrientation   = kQ3OrientationStyleCounterClockwise;

    Q3Point3D_Set( &instanceData->stateLocalCameraPosition,       0.0f, 0.0f,  0.0f);
    Q3Vector3D_Set(&instanceData->stateLocalCameraViewVector,     0.0f, 0.0f, -1.0f);
    Q3Matrix4x4_SetIdentity(&instanceData->stateMatrixLocalToCamera);
	instanceData->stateMatrixLocalToCameraInvTrValid = kQ3False;
    Q3Matrix4x4_SetIdentity(&instanceData->stateMatrixCameraToFrustum);
    Q3ColorRGB_Set(&instanceData->stateDefaultDiffuseColour,      kQ3ViewDefaultDiffuseColor);
    Q3ColorRGB_Set(&instanceData->stateDefaultSpecularColour,     kQ3ViewDefaultSpecularColor);
    Q3ColorRGB_Set(&instanceData->stateDefaultTransparencyColour, kQ3ViewDefaultTransparency);

	for (n = 0; n < 4; n++)
		instanceData->stateCurrentSpecularColour[n] = -1.0f;

	instanceData->stateCurrentSpecularControl = -1.0f;
	instanceData->stateTextureActive          = kQ3False;
	instanceData->stateTextureObject          = 0;
	instanceData->stateTextureIsTransparent   = kQ3False;

    instanceData->stateGeomDiffuseColour      = &instanceData->stateDefaultDiffuseColour;
    instanceData->stateGeomSpecularColour     = &instanceData->stateDefaultSpecularColour;
    instanceData->stateGeomTransparencyColour = &instanceData->stateDefaultTransparencyColour;
	instanceData->stateGeomSpecularControl    = kQ3ViewDefaultSpecularControl;
	instanceData->stateGeomHilightState       = kQ3Off;


    instanceData->stateViewDiffuseColour      = &instanceData->stateDefaultDiffuseColour;
    instanceData->stateViewSpecularColour     = &instanceData->stateDefaultSpecularColour;
    instanceData->stateViewTransparencyColour = &instanceData->stateDefaultTransparencyColour;
	instanceData->stateViewSpecularControl    = kQ3ViewDefaultSpecularControl;
	instanceData->stateViewHilightState       = kQ3Off;
	instanceData->stateViewIllumination       = kQ3ObjectTypeInvalid;


	instanceData->fogStyles.clear();
	TQ3FogStyleData	noFog;
	noFog.state = kQ3Off;
	instanceData->fogStyles.push_back( noFog );
	instanceData->curFogStyleIndex = 0;
	
}





//=============================================================================
//      operator== : Function object to compare fog style datas.
//-----------------------------------------------------------------------------
static	bool operator==( const TQ3FogStyleData& inOne, const TQ3FogStyleData& inTwo )
{
	bool	isSameFog = (inOne.state == inTwo.state);
	
	if (isSameFog && (inOne.state == kQ3On))
	{
		isSameFog = (inOne.mode == inTwo.mode) &&
			(inOne.color.r == inTwo.color.r) &&
			(inOne.color.g == inTwo.color.g) &&
			(inOne.color.b == inTwo.color.b);
			// Note, alpha is not relevant
			
		if (isSameFog)
		{
			switch (inOne.mode)
			{
				case kQ3FogModeExponential:
				case kQ3FogModeExponentialSquared:
					isSameFog = (inOne.density == inTwo.density);
					break;
				
				default:	// linear fog or anything else
					isSameFog = (inOne.fogStart == inTwo.fogStart) &&
						(inOne.fogEnd == inTwo.fogEnd);
					break;
			}
		}
	}
	return isSameFog;
}





//=============================================================================
//      Public functions
//-----------------------------------------------------------------------------
//      IRRenderer_State_StartPass : Start a frame.
//-----------------------------------------------------------------------------
#pragma mark -
void
IRRenderer_State_StartPass(TQ3InteractiveData *instanceData, TQ3ViewObject theView)
{
#pragma unused( theView )

	// Reset the state
	ir_state_reset(instanceData);
	
	
	// Our calls to glDrawElements always use a vertex array
	glEnableClientState(GL_VERTEX_ARRAY);

	
	// Other arrays are initially off.
	instanceData->glClientStateNormal = kQ3False;
	instanceData->glClientStateColor = kQ3False;
	instanceData->glClientStateUV = kQ3False;
	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}





//=============================================================================
//      IRRenderer_State_EndPass : Finish a frame.
//-----------------------------------------------------------------------------
void
IRRenderer_State_EndPass(TQ3InteractiveData *instanceData)
{


	// Dispose of our state
	if (instanceData->stateHilight != nullptr)
		Q3Object_Dispose(instanceData->stateHilight);



	// Reset the state
	ir_state_reset(instanceData);
}





//=============================================================================
//      IRRenderer_State_AdjustGL : Adjust the OpenGL state for a geometry.
//-----------------------------------------------------------------------------
//		Note :	Although QD3D allows us to specify some attributes at a
//				per-vertex level (e.g., specular colour), we currently support
//				these attributes at the geometry level with OpenGL (to minimise
//				the number of material changes we need to make).
//
//				May be possible to improve this if we keep track of what the
//				state was at the previous vertex and only update the OpenGL
//				state when it changes? 
//-----------------------------------------------------------------------------
void
IRRenderer_State_AdjustGL(TQ3InteractiveData *instanceData)
{	float		specularControl;
	GLfloat		shininess;



	// If we're using Phong illumination, update the specular colour and control
	if (instanceData->stateViewIllumination == kQ3IlluminationTypePhong)
		{
		// Update the specular colour if it's changed
		if (instanceData->stateGeomSpecularColour->r != instanceData->stateCurrentSpecularColour[0] ||
			instanceData->stateGeomSpecularColour->g != instanceData->stateCurrentSpecularColour[1] ||
			instanceData->stateGeomSpecularColour->b != instanceData->stateCurrentSpecularColour[2])
			{
			instanceData->stateCurrentSpecularColour[0] = instanceData->stateGeomSpecularColour->r;
			instanceData->stateCurrentSpecularColour[1] = instanceData->stateGeomSpecularColour->g;
			instanceData->stateCurrentSpecularColour[2] = instanceData->stateGeomSpecularColour->b;
			instanceData->stateCurrentSpecularColour[3] = 1.0f;

			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, instanceData->stateCurrentSpecularColour);
			}
			


		// Update the specular control if it's changed.
		if (instanceData->stateGeomSpecularControl != instanceData->stateCurrentSpecularControl)
			{
			// Grab the value
			instanceData->stateCurrentSpecularControl = instanceData->stateGeomSpecularControl;
			specularControl                           = instanceData->stateCurrentSpecularControl;
			
			shininess = GLUtils_SpecularControlToGLShininess( specularControl );
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
			}
		}
}





//=============================================================================
//      IRRenderer_Update_Matrix_LocalToCamera : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Matrix_LocalToCamera(TQ3ViewObject			theView,
											TQ3InteractiveData	*instanceData,
											TQ3Matrix4x4		*theMatrix)
{	TQ3Point3D			viewPosition = {0.0f, 0.0f,  0.0f};
	TQ3Vector3D			viewVector   = {0.0f, 0.0f, -1.0f};
	TQ3Matrix4x4		cameraToLocal;



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Update our state
	instanceData->stateMatrixLocalToCamera = *theMatrix;
	instanceData->stateMatrixLocalToCameraInvTrValid = kQ3False;



	// Determine the camera position and view vector in local coordinates
	Q3Matrix4x4_Invert(theMatrix, &cameraToLocal);

	Q3Point3D_Transform( &viewPosition, &cameraToLocal, &instanceData->stateLocalCameraPosition);
	Q3Vector3D_Transform(&viewVector,   &cameraToLocal, &viewVector);
	Q3Vector3D_Normalize(&viewVector,   &instanceData->stateLocalCameraViewVector);



	// Set the model-view transform
	GLCamera_SetModelView(theMatrix);



	// Adjust the normalisation state
#if QUESA_NORMALIZE_NORMALS
	//
	// If the current transform doesn't have a scale component, we can turn off automatic
	// normalization. Quesa's documented behaviour is that incoming TriMesh objects always
	// have normalized normals, and we always normalize Triangle normals, so we can avoid
	// having OpenGL do normalization if that is the case.
	//
	// Even if the current transform does have a scale component, we should probably use
	// GL_RESCALE_NORMAL if it's available - if the scale is uniform, this is potentially
	// more efficient than GL_NORMALIZE since OpenGL can use a simpler re-normalization.
	if (theMatrix->value[0][0] != 1.0f ||
		theMatrix->value[1][1] != 1.0f ||
		theMatrix->value[2][2] != 1.0f ||
		theMatrix->value[3][3] != 1.0f)
		glEnable(GL_NORMALIZE);
	else
		glDisable(GL_NORMALIZE);
#else
	// If QUESA_NORMALIZE_NORMALS is off, we make OpenGL do a little more work
	// in order to save work on the CPU whenever we create or reset a TriMesh.
	glEnable( GL_NORMALIZE );
#endif

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Matrix_CameraToFrustum : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Matrix_CameraToFrustum(TQ3ViewObject			theView,
											TQ3InteractiveData	*instanceData,
											TQ3Matrix4x4		*theMatrix)
{



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Update our state
	instanceData->stateMatrixCameraToFrustum = *theMatrix;



	// Set the projection transform
	GLCamera_SetProjection(theMatrix);

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_Interpolation : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_Interpolation(TQ3ViewObject				theView,
										TQ3InteractiveData		*instanceData,
										TQ3InterpolationStyle	*styleData)
{



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Set the interpolation style
	//
	// OpenGL has two shading model, flat and smooth (Gouraud shading).
	// Like QD3D's IR, we treat kQ3InterpolationStyleVertex as being
	// equivalent to kQ3InterpolationStylePixel.
	instanceData->stateInterpolation = *styleData;
	
	switch (instanceData->stateInterpolation) {
		case kQ3InterpolationStyleNone:
			glShadeModel(GL_FLAT);
			break;

		case kQ3InterpolationStyleVertex:
		case kQ3InterpolationStylePixel:
			glShadeModel(GL_SMOOTH);
		default:
			break;
		}	
	
	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_Backfacing : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_Backfacing(TQ3ViewObject		theView,
									TQ3InteractiveData	*instanceData,
									TQ3BackfacingStyle	*styleData)
{



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Set the backfacing style
	//
	// Note that backface culling is handled by the renderer rather than OpenGL,
	// to allow triangles to be culled based on an application-supplied normal
	// rather than always using the geometric normal (as per OpenGL culling).
	//
	// However we do need to adjust the OpenGL lighting model at this point, as
	// we want backfacing triangles to be lit as if they were front facing.
	instanceData->stateBackfacing = *styleData;

	switch (instanceData->stateBackfacing) {
		case kQ3BackfacingStyleFlip:
			// Enable 2-sided lighting
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			break;

		case kQ3BackfacingStyleBoth:
		case kQ3BackfacingStyleRemove:
		default:
			// Disable 2-sided lighting
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
			break;
		}

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_Fill : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_Fill(TQ3ViewObject		theView,
							 TQ3InteractiveData	*instanceData,
							 TQ3FillStyle		*styleData)
{



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Save and set the fill style
	instanceData->stateFill = *styleData;

	switch (instanceData->stateFill) {
		case kQ3FillStylePoints:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;

		case kQ3FillStyleEdges:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;

		case kQ3FillStyleFilled:
		default:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}	



	// If texture mapping is on, adjust the texture state
	if (instanceData->stateTextureActive)
		{
		if (instanceData->stateFill == kQ3FillStyleFilled)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
		}

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_Orientation : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_Orientation(TQ3ViewObject			theView,
									 TQ3InteractiveData		*instanceData,
									 TQ3OrientationStyle	*styleData)
{



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Set the front facing direction
	instanceData->stateOrientation = *styleData;

	switch (instanceData->stateOrientation) {
		case kQ3OrientationStyleClockwise:
			glFrontFace(GL_CW);
			break;

		case kQ3OrientationStyleCounterClockwise:
		default:
			glFrontFace(GL_CCW);
			break;
		}

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_Hilight : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_Hilight(TQ3ViewObject			theView,
								 TQ3InteractiveData		*instanceData,
								 TQ3AttributeSet		*styleData)
{
#pragma unused(theView)



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Save the hilight attribute set
	E3Shared_Replace(&instanceData->stateHilight, *styleData);

    return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_AntiAlias : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_AntiAlias(TQ3ViewObject					theView,
									TQ3InteractiveData			*instanceData,
									TQ3AntiAliasStyleData		*styleData)
{
	GLfloat				lineWidth;
	#if QUESA_OS_MACINTOSH
	
	const TQ3Uns32		ATI_FSAA_SAMPLES = 510;
	char				theBuffer[512];
	GLint				fsaaLevel;
	
	#endif



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Turn everything off
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	GLint	sampleBuffers = 0;
	glGetIntegerv( GL_SAMPLE_BUFFERS_ARB, &sampleBuffers );
	if (sampleBuffers > 0)
	{
		glDisable( GL_MULTISAMPLE_ARB );
	}



	// restore defaults
	glPointSize(1.0f);
	glLineWidth(1.0f);



	// Turn things back on as required
	if (styleData->state == kQ3On)
	{
		if ( (styleData->mode & kQ3AntiAliasModeMaskFullScreen) != 0 )
		{
			if (sampleBuffers > 0)
			{
				glEnable( GL_MULTISAMPLE_ARB );
			}
		}
		else
		{
			// Set up our aliasing thresholds
			//
			// Implementations should clamp anti-aliased lines to their minimum width
			// automatically, however some common systems (Rev-A G5s, some iBooks and
			// AlBooks) will draw zero-width lines if line smoothing is enabled.
			//
			// To avoid this we clamp our line widths to the minimum supported sizes,
			// to ensure we always have a visible line.
			lineWidth = E3Num_Max(kAALineSize,
				GLDrawContext_GetMinLineWidth(instanceData->glContext));

			glPointSize(kAAPointSize);
			glLineWidth(lineWidth);


			// Always do points
			glEnable(GL_POINT_SMOOTH);


			// Do edges or polygons as required
			if (styleData->mode & kQ3AntiAliasModeMaskEdges)
				glEnable(GL_LINE_SMOOTH);

			if (styleData->mode & kQ3AntiAliasModeMaskFilled)
				glEnable(GL_POLYGON_SMOOTH);
		}
	}

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Style_Fog : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Style_Fog(TQ3ViewObject		theView,
							 TQ3InteractiveData	*instanceData,
							 TQ3FogStyleData	*styleData)
{	GLfloat		fogColour[4];



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Set up the fog state
	if (styleData->state == kQ3On)
		{
		// Set up the colour
		fogColour[0] = (GLfloat) styleData->color.r;
		fogColour[1] = (GLfloat) styleData->color.g;
		fogColour[2] = (GLfloat) styleData->color.b;
		fogColour[3] = (GLfloat) styleData->color.a;



		// Set up the fog state
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR,  fogColour);



		// Set the fog mode (alpha fog is not supported in OpenGL)
		switch (styleData->mode) {
			case kQ3FogModeExponential:
				glFogi(GL_FOG_MODE, GL_EXP);
				glFogf(GL_FOG_DENSITY, styleData->density);
				break;
				
			case kQ3FogModeExponentialSquared:
				glFogi(GL_FOG_MODE, GL_EXP2);
				glFogf(GL_FOG_DENSITY, styleData->density);
				break;
				
			case kQ3FogModeLinear:
			case kQ3FogModeAlpha:
			default:
				glFogi(GL_FOG_MODE, GL_LINEAR);
				glFogf(GL_FOG_START,   styleData->fogStart);
				glFogf(GL_FOG_END,     styleData->fogEnd);
				break;
			}
		}


	// Or turn the fog off
	else
		glDisable(GL_FOG);
	
	
	// Update current fog in instance data
	std::vector<TQ3FogStyleData>::iterator foundFog =
		std::find( instanceData->fogStyles.begin(), instanceData->fogStyles.end(),
		*styleData );
	if (foundFog == instanceData->fogStyles.end())
	{
		try
		{
			instanceData->fogStyles.push_back( *styleData );
			instanceData->curFogStyleIndex = static_cast<TQ3Uns32>(instanceData->fogStyles.size() - 1);
		}
		catch (...)
		{
		}
	}
	else
	{
		instanceData->curFogStyleIndex = static_cast<TQ3Uns32>(foundFog - instanceData->fogStyles.begin());
	}
	

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Attribute_DiffuseColour : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Attribute_DiffuseColour(TQ3ViewObject			theView,
							 			  TQ3InteractiveData	*instanceData,
							 			  TQ3ColorRGB			*attributeData)
{
#pragma unused( theView )


	// Update our state
	instanceData->stateViewDiffuseColour = attributeData;

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Attribute_SpecularColour : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Attribute_SpecularColour(TQ3ViewObject		theView,
							 			  	TQ3InteractiveData	*instanceData,
							 			  	TQ3ColorRGB			*attributeData)
{
#pragma unused( theView )


	// Update our state
	instanceData->stateViewSpecularColour = attributeData;

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Attribute_SpecularControl : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Attribute_SpecularControl(TQ3ViewObject		theView,
							 			  	TQ3InteractiveData	*instanceData,
							 			  	float				*attributeData)
{
#pragma unused( theView )


	// Update our state
	instanceData->stateViewSpecularControl = *attributeData;

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Attribute_HilightState : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Attribute_HilightState(TQ3ViewObject			theView,
							 			 TQ3InteractiveData		*instanceData,
							 			 TQ3Switch				*attributeData)
{
#pragma unused( theView )


	// Update our state
	instanceData->stateViewHilightState = *attributeData;

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Attribute_TransparencyColour : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Attribute_TransparencyColour(TQ3ViewObject		theView,
							 					TQ3InteractiveData	*instanceData,
							 					TQ3ColorRGB			*attributeData)
{
#pragma unused( theView )


	// Update our state
	instanceData->stateViewTransparencyColour = attributeData;

	return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Shader_Illumination : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Shader_Illumination(TQ3ViewObject			theView,
							 			TQ3InteractiveData	*instanceData,
							 			TQ3ShaderObject		*shaderData)
{	GLfloat		specularColour[4]  = { 0.0f, 0.0f, 0.0f, 0.0f };
    GLfloat		specularControl[1] = { 0.0f };



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Update our state
    if (shaderData != nullptr && *shaderData != nullptr)
        instanceData->stateViewIllumination = Q3IlluminationShader_GetType(*shaderData);
    else
        instanceData->stateViewIllumination = kQ3ObjectTypeInvalid;



	// Update the OpenGL state
	switch (instanceData->stateViewIllumination) {
		default:
		case kQ3IlluminationTypeNULL:
			glDisable(GL_LIGHTING);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specularColour);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, specularControl);
			break;
		
		case kQ3IlluminationTypeLambert:
			glEnable(GL_LIGHTING);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specularColour);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, specularControl);
			break;
		
		case kQ3IlluminationTypePhong:
			glEnable(GL_LIGHTING);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, instanceData->stateCurrentSpecularColour);
			specularControl[0] =
				GLUtils_SpecularControlToGLShininess( instanceData->stateCurrentSpecularControl );
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, specularControl);
			break;
		}

    return(kQ3Success);
}





//=============================================================================
//      IRRenderer_Update_Shader_Surface : Update our state.
//-----------------------------------------------------------------------------
TQ3Status
IRRenderer_Update_Shader_Surface(TQ3ViewObject			theView,
							 		TQ3InteractiveData	*instanceData,
							 		TQ3ShaderObject		*shaderData)
{	TQ3TextureObject	theTexture;
	TQ3ShaderObject		theShader;
	TQ3ObjectType		theType;



	// Activate our context
	GLDrawContext_SetCurrent(instanceData->glContext, kQ3False);



	// Flush any buffered triangles
	if (instanceData->triBufferActive)
		IRTriBuffer_Draw(theView, instanceData);



	// Get the texture from the shader
	theShader  = (shaderData != nullptr ? *shaderData : nullptr);
	theTexture = nullptr;

	if (theShader != nullptr)
		{
		theType = Q3SurfaceShader_GetType(theShader);
		if (theType == kQ3SurfaceShaderTypeTexture)
			Q3TextureShader_GetTexture(theShader, &theTexture);
		}



	// Set the texture
	IRRenderer_Texture_Set(theView, instanceData, theShader, theTexture);
	


	// Clean up
	if (theTexture != nullptr)
		Q3Object_Dispose(theTexture);

    return(kQ3Success);
}
