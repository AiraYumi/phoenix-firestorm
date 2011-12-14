/** 
 * @file llviewershadermgr.cpp
 * @brief Viewer shader manager implementation.
 *
 * $LicenseInfo:firstyear=2005&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */


#include "llviewerprecompiledheaders.h"

#include "llfeaturemanager.h"
#include "llviewershadermgr.h"

#include "llfile.h"
#include "llviewerwindow.h"
#include "llviewercontrol.h"
#include "pipeline.h"
#include "llworld.h"
#include "llwlparammanager.h"
#include "llwaterparammanager.h"
#include "llsky.h"
#include "llvosky.h"
#include "llrender.h"

#if LL_DARWIN
#include "OpenGL/OpenGL.h"
#endif

#ifdef LL_RELEASE_FOR_DOWNLOAD
#define UNIFORM_ERRS LL_WARNS_ONCE("Shader")
#else
#define UNIFORM_ERRS LL_ERRS("Shader")
#endif

// Lots of STL stuff in here, using namespace std to keep things more readable
using std::vector;
using std::pair;
using std::make_pair;
using std::string;

BOOL				LLViewerShaderMgr::sInitialized = FALSE;

LLVector4			gShinyOrigin;

//utility shaders
LLGLSLShader	gOcclusionProgram;
LLGLSLShader	gCustomAlphaProgram;
LLGLSLShader	gGlowCombineProgram;
LLGLSLShader	gTwoTextureAddProgram;

//object shaders
LLGLSLShader		gObjectSimpleProgram;
LLGLSLShader		gObjectSimpleWaterProgram;
LLGLSLShader		gObjectSimpleAlphaMaskProgram;
LLGLSLShader		gObjectSimpleWaterAlphaMaskProgram;
LLGLSLShader		gObjectFullbrightProgram;
LLGLSLShader		gObjectFullbrightWaterProgram;
LLGLSLShader		gObjectFullbrightAlphaMaskProgram;
LLGLSLShader		gObjectFullbrightWaterAlphaMaskProgram;
LLGLSLShader		gObjectFullbrightShinyProgram;
LLGLSLShader		gObjectFullbrightShinyWaterProgram;
LLGLSLShader		gObjectShinyProgram;
LLGLSLShader		gObjectShinyWaterProgram;
LLGLSLShader		gObjectBumpProgram;

LLGLSLShader		gObjectSimpleNonIndexedProgram;
LLGLSLShader		gObjectSimpleNonIndexedWaterProgram;
LLGLSLShader		gObjectAlphaMaskNonIndexedProgram;
LLGLSLShader		gObjectAlphaMaskNonIndexedWaterProgram;
LLGLSLShader		gObjectFullbrightNonIndexedProgram;
LLGLSLShader		gObjectFullbrightNonIndexedWaterProgram;
LLGLSLShader		gObjectFullbrightShinyNonIndexedProgram;
LLGLSLShader		gObjectFullbrightShinyNonIndexedWaterProgram;
LLGLSLShader		gObjectShinyNonIndexedProgram;
LLGLSLShader		gObjectShinyNonIndexedWaterProgram;

//object hardware skinning shaders
LLGLSLShader		gSkinnedObjectSimpleProgram;
LLGLSLShader		gSkinnedObjectFullbrightProgram;
LLGLSLShader		gSkinnedObjectFullbrightShinyProgram;
LLGLSLShader		gSkinnedObjectShinySimpleProgram;

LLGLSLShader		gSkinnedObjectSimpleWaterProgram;
LLGLSLShader		gSkinnedObjectFullbrightWaterProgram;
LLGLSLShader		gSkinnedObjectFullbrightShinyWaterProgram;
LLGLSLShader		gSkinnedObjectShinySimpleWaterProgram;

//environment shaders
LLGLSLShader		gTerrainProgram;
LLGLSLShader		gTerrainWaterProgram;
LLGLSLShader		gWaterProgram;
LLGLSLShader		gUnderWaterProgram;

//interface shaders
LLGLSLShader		gHighlightProgram;

//avatar shader handles
LLGLSLShader		gAvatarProgram;
LLGLSLShader		gAvatarWaterProgram;
LLGLSLShader		gAvatarEyeballProgram;
LLGLSLShader		gAvatarPickProgram;
LLGLSLShader		gImpostorProgram;

// WindLight shader handles
LLGLSLShader			gWLSkyProgram;
LLGLSLShader			gWLCloudProgram;

// Effects Shaders
LLGLSLShader			gGlowProgram;
LLGLSLShader			gGlowExtractProgram;
LLGLSLShader			gPostColorFilterProgram;
LLGLSLShader			gPostNightVisionProgram;

// Deferred rendering shaders
LLGLSLShader			gDeferredImpostorProgram;
LLGLSLShader			gDeferredEdgeProgram;
LLGLSLShader			gDeferredWaterProgram;
LLGLSLShader			gDeferredDiffuseProgram;
LLGLSLShader			gDeferredDiffuseAlphaMaskProgram;
LLGLSLShader			gDeferredNonIndexedDiffuseProgram;
LLGLSLShader			gDeferredNonIndexedDiffuseAlphaMaskProgram;
LLGLSLShader			gDeferredSkinnedDiffuseProgram;
LLGLSLShader			gDeferredSkinnedBumpProgram;
LLGLSLShader			gDeferredSkinnedAlphaProgram;
LLGLSLShader			gDeferredBumpProgram;
LLGLSLShader			gDeferredTerrainProgram;
LLGLSLShader			gDeferredTreeProgram;
LLGLSLShader			gDeferredAvatarProgram;
LLGLSLShader			gDeferredAvatarAlphaProgram;
LLGLSLShader			gDeferredLightProgram;
LLGLSLShader			gDeferredMultiLightProgram;
LLGLSLShader			gDeferredSpotLightProgram;
LLGLSLShader			gDeferredMultiSpotLightProgram;
LLGLSLShader			gDeferredSunProgram;
LLGLSLShader			gDeferredBlurLightProgram;
LLGLSLShader			gDeferredSoftenProgram;
LLGLSLShader			gDeferredShadowProgram;
LLGLSLShader			gDeferredShadowAlphaMaskProgram;
LLGLSLShader			gDeferredAvatarShadowProgram;
LLGLSLShader			gDeferredAttachmentShadowProgram;
LLGLSLShader			gDeferredAlphaProgram;
LLGLSLShader			gDeferredAvatarEyesProgram;
LLGLSLShader			gDeferredFullbrightProgram;
LLGLSLShader			gDeferredGIProgram;
LLGLSLShader			gDeferredGIFinalProgram;
LLGLSLShader			gDeferredPostGIProgram;
LLGLSLShader			gDeferredPostProgram;
LLGLSLShader			gDeferredPostNoDoFProgram;
LLGLSLShader			gDeferredWLSkyProgram;
LLGLSLShader			gDeferredWLCloudProgram;
LLGLSLShader			gDeferredStarProgram;
LLGLSLShader			gLuminanceGatherProgram;


//current avatar shader parameter pointer
GLint				gAvatarMatrixParam;

LLViewerShaderMgr::LLViewerShaderMgr() :
	mVertexShaderLevel(SHADER_COUNT, 0),
	mMaxAvatarShaderLevel(0)
{	
	/// Make sure WL Sky is the first program
	mShaderList.push_back(&gWLSkyProgram);
	mShaderList.push_back(&gWLCloudProgram);
	mShaderList.push_back(&gAvatarProgram);
	mShaderList.push_back(&gObjectShinyProgram);
	mShaderList.push_back(&gObjectShinyNonIndexedProgram);
	mShaderList.push_back(&gWaterProgram);
	mShaderList.push_back(&gAvatarEyeballProgram); 
	mShaderList.push_back(&gObjectSimpleProgram);
	mShaderList.push_back(&gImpostorProgram);
	mShaderList.push_back(&gObjectSimpleAlphaMaskProgram);
	mShaderList.push_back(&gObjectBumpProgram);
	mShaderList.push_back(&gUIProgram);
	mShaderList.push_back(&gCustomAlphaProgram);
	mShaderList.push_back(&gGlowCombineProgram);
	mShaderList.push_back(&gTwoTextureAddProgram);
	mShaderList.push_back(&gSolidColorProgram);
	mShaderList.push_back(&gOcclusionProgram);
	mShaderList.push_back(&gObjectFullbrightProgram);
	mShaderList.push_back(&gObjectFullbrightAlphaMaskProgram);
	mShaderList.push_back(&gObjectFullbrightShinyProgram);
	mShaderList.push_back(&gObjectFullbrightShinyWaterProgram);
	mShaderList.push_back(&gObjectSimpleNonIndexedProgram);
	mShaderList.push_back(&gObjectSimpleNonIndexedWaterProgram);
	mShaderList.push_back(&gObjectAlphaMaskNonIndexedProgram);
	mShaderList.push_back(&gObjectAlphaMaskNonIndexedWaterProgram);
	mShaderList.push_back(&gObjectFullbrightNonIndexedProgram);
	mShaderList.push_back(&gObjectFullbrightNonIndexedWaterProgram);
	mShaderList.push_back(&gObjectFullbrightShinyNonIndexedProgram);
	mShaderList.push_back(&gObjectFullbrightShinyNonIndexedWaterProgram);
	mShaderList.push_back(&gSkinnedObjectSimpleProgram);
	mShaderList.push_back(&gSkinnedObjectFullbrightProgram);
	mShaderList.push_back(&gSkinnedObjectFullbrightShinyProgram);
	mShaderList.push_back(&gSkinnedObjectShinySimpleProgram);
	mShaderList.push_back(&gSkinnedObjectSimpleWaterProgram);
	mShaderList.push_back(&gSkinnedObjectFullbrightWaterProgram);
	mShaderList.push_back(&gSkinnedObjectFullbrightShinyWaterProgram);
	mShaderList.push_back(&gSkinnedObjectShinySimpleWaterProgram);
	mShaderList.push_back(&gTerrainProgram);
	mShaderList.push_back(&gTerrainWaterProgram);
	mShaderList.push_back(&gObjectSimpleWaterProgram);
	mShaderList.push_back(&gObjectFullbrightWaterProgram);
	mShaderList.push_back(&gObjectSimpleWaterAlphaMaskProgram);
	mShaderList.push_back(&gObjectFullbrightWaterAlphaMaskProgram);
	mShaderList.push_back(&gAvatarWaterProgram);
	mShaderList.push_back(&gObjectShinyWaterProgram);
	mShaderList.push_back(&gObjectShinyNonIndexedWaterProgram);
	mShaderList.push_back(&gUnderWaterProgram);
	mShaderList.push_back(&gDeferredSunProgram);
	mShaderList.push_back(&gDeferredBlurLightProgram);
	mShaderList.push_back(&gDeferredSoftenProgram);
	mShaderList.push_back(&gDeferredLightProgram);
	mShaderList.push_back(&gDeferredMultiLightProgram);
	mShaderList.push_back(&gDeferredAlphaProgram);
	mShaderList.push_back(&gDeferredSkinnedAlphaProgram);
	mShaderList.push_back(&gDeferredFullbrightProgram);
	mShaderList.push_back(&gDeferredAvatarEyesProgram);
	mShaderList.push_back(&gDeferredPostGIProgram);
	mShaderList.push_back(&gDeferredEdgeProgram);
	mShaderList.push_back(&gDeferredPostProgram);
	mShaderList.push_back(&gDeferredGIProgram);
	mShaderList.push_back(&gDeferredGIFinalProgram);
	mShaderList.push_back(&gDeferredWaterProgram);
	mShaderList.push_back(&gDeferredAvatarAlphaProgram);
	mShaderList.push_back(&gDeferredWLSkyProgram);
	mShaderList.push_back(&gDeferredWLCloudProgram);
	mShaderList.push_back(&gDeferredStarProgram);
}

LLViewerShaderMgr::~LLViewerShaderMgr()
{
	mVertexShaderLevel.clear();
	mShaderList.clear();
}

// static
LLViewerShaderMgr * LLViewerShaderMgr::instance()
{
	if(NULL == sInstance)
	{
		sInstance = new LLViewerShaderMgr();
	}

	return static_cast<LLViewerShaderMgr*>(sInstance);
}

void LLViewerShaderMgr::initAttribsAndUniforms(void)
{
	if (mReservedAttribs.empty())
	{
		mReservedAttribs.push_back("materialColor");
		mReservedAttribs.push_back("specularColor");
		mReservedAttribs.push_back("binormal");
		mReservedAttribs.push_back("object_weight");

		mAvatarAttribs.reserve(5);
		mAvatarAttribs.push_back("weight");
		mAvatarAttribs.push_back("clothing");
		mAvatarAttribs.push_back("gWindDir");
		mAvatarAttribs.push_back("gSinWaveParams");
		mAvatarAttribs.push_back("gGravity");

		mAvatarUniforms.push_back("matrixPalette");

		mReservedUniforms.reserve(24);
		mReservedUniforms.push_back("diffuseMap");
		mReservedUniforms.push_back("specularMap");
		mReservedUniforms.push_back("bumpMap");
		mReservedUniforms.push_back("environmentMap");
		mReservedUniforms.push_back("cloude_noise_texture");
		mReservedUniforms.push_back("fullbright");
		mReservedUniforms.push_back("lightnorm");
		mReservedUniforms.push_back("sunlight_color");
		mReservedUniforms.push_back("ambient");
		mReservedUniforms.push_back("blue_horizon");
		mReservedUniforms.push_back("blue_density");
		mReservedUniforms.push_back("haze_horizon");
		mReservedUniforms.push_back("haze_density");
		mReservedUniforms.push_back("cloud_shadow");
		mReservedUniforms.push_back("density_multiplier");
		mReservedUniforms.push_back("distance_multiplier");
		mReservedUniforms.push_back("max_y");
		mReservedUniforms.push_back("glow");
		mReservedUniforms.push_back("cloud_color");
		mReservedUniforms.push_back("cloud_pos_density1");
		mReservedUniforms.push_back("cloud_pos_density2");
		mReservedUniforms.push_back("cloud_scale");
		mReservedUniforms.push_back("gamma");
		mReservedUniforms.push_back("scene_light_strength");

		mReservedUniforms.push_back("depthMap");
		mReservedUniforms.push_back("shadowMap0");
		mReservedUniforms.push_back("shadowMap1");
		mReservedUniforms.push_back("shadowMap2");
		mReservedUniforms.push_back("shadowMap3");
		mReservedUniforms.push_back("shadowMap4");
		mReservedUniforms.push_back("shadowMap5");

		mReservedUniforms.push_back("normalMap");
		mReservedUniforms.push_back("positionMap");
		mReservedUniforms.push_back("diffuseRect");
		mReservedUniforms.push_back("specularRect");
		mReservedUniforms.push_back("noiseMap");
		mReservedUniforms.push_back("lightFunc");
		mReservedUniforms.push_back("lightMap");
		mReservedUniforms.push_back("luminanceMap");
		mReservedUniforms.push_back("giLightMap");
		mReservedUniforms.push_back("giMip");
		mReservedUniforms.push_back("edgeMap");
		mReservedUniforms.push_back("bloomMap");
		mReservedUniforms.push_back("sunLightMap");
		mReservedUniforms.push_back("localLightMap");
		mReservedUniforms.push_back("projectionMap");
		mReservedUniforms.push_back("diffuseGIMap");
		mReservedUniforms.push_back("specularGIMap");
		mReservedUniforms.push_back("normalGIMap");
		mReservedUniforms.push_back("minpGIMap");
		mReservedUniforms.push_back("maxpGIMap");
		mReservedUniforms.push_back("depthGIMap");
		mReservedUniforms.push_back("lastDiffuseGIMap");
		mReservedUniforms.push_back("lastNormalGIMap");
		mReservedUniforms.push_back("lastMinpGIMap");
		mReservedUniforms.push_back("lastMaxpGIMap");
					
		mWLUniforms.push_back("camPosLocal");

		mTerrainUniforms.reserve(5);
		mTerrainUniforms.push_back("detail_0");
		mTerrainUniforms.push_back("detail_1");
		mTerrainUniforms.push_back("detail_2");
		mTerrainUniforms.push_back("detail_3");
		mTerrainUniforms.push_back("alpha_ramp");

		mGlowUniforms.push_back("glowDelta");
		mGlowUniforms.push_back("glowStrength");

		mGlowExtractUniforms.push_back("minLuminance");
		mGlowExtractUniforms.push_back("maxExtractAlpha");
		mGlowExtractUniforms.push_back("lumWeights");
		mGlowExtractUniforms.push_back("warmthWeights");
		mGlowExtractUniforms.push_back("warmthAmount");

		mShinyUniforms.push_back("origin");

		mWaterUniforms.reserve(12);
		mWaterUniforms.push_back("screenTex");
		mWaterUniforms.push_back("screenDepth");
		mWaterUniforms.push_back("refTex");
		mWaterUniforms.push_back("eyeVec");
		mWaterUniforms.push_back("time");
		mWaterUniforms.push_back("d1");
		mWaterUniforms.push_back("d2");
		mWaterUniforms.push_back("lightDir");
		mWaterUniforms.push_back("specular");
		mWaterUniforms.push_back("lightExp");
		mWaterUniforms.push_back("fogCol");
		mWaterUniforms.push_back("kd");
		mWaterUniforms.push_back("refScale");
		mWaterUniforms.push_back("waterHeight");
	}	
}
	

//============================================================================
// Set Levels

S32 LLViewerShaderMgr::getVertexShaderLevel(S32 type)
{
	return LLPipeline::sDisableShaders ? 0 : mVertexShaderLevel[type];
}

//============================================================================
// Shader Management

void LLViewerShaderMgr::setShaders()
{
	//setShaders might be called redundantly by gSavedSettings, so return on reentrance
	static bool reentrance = false;
	
	if (!gPipeline.mInitialized || !sInitialized || reentrance)
	{
		return;
	}

	//setup preprocessor definitions
	LLShaderMgr::instance()->mDefinitions["samples"] = llformat("%d", gGLManager.getNumFBOFSAASamples(gSavedSettings.getU32("RenderFSAASamples")));
	LLShaderMgr::instance()->mDefinitions["NUM_TEX_UNITS"] = llformat("%d", gGLManager.mNumTextureImageUnits);

	reentrance = true;

	// Make sure the compiled shader map is cleared before we recompile shaders.
	mShaderObjects.clear();
	
	initAttribsAndUniforms();
	gPipeline.releaseGLBuffers();

	if (gSavedSettings.getBOOL("VertexShaderEnable"))
	{
		LLPipeline::sWaterReflections = gGLManager.mHasCubeMap;
		LLPipeline::sRenderGlow = gSavedSettings.getBOOL("RenderGlow"); 
		LLPipeline::updateRenderDeferred();
	}
	else
	{
		LLPipeline::sRenderGlow = FALSE;
		LLPipeline::sWaterReflections = FALSE;
	}
	
	//hack to reset buffers that change behavior with shaders
	gPipeline.resetVertexBuffers();

	if (gViewerWindow)
	{
		gViewerWindow->setCursor(UI_CURSOR_WAIT);
	}

	// Lighting
	gPipeline.setLightingDetail(-1);

	// Shaders
	LL_INFOS("ShaderLoading") << "\n~~~~~~~~~~~~~~~~~~\n Loading Shaders:\n~~~~~~~~~~~~~~~~~~" << LL_ENDL;
	for (S32 i = 0; i < SHADER_COUNT; i++)
	{
		mVertexShaderLevel[i] = 0;
	}
	mMaxAvatarShaderLevel = 0;

	LLGLSLShader::sNoFixedFunction = false;
	if (LLFeatureManager::getInstance()->isFeatureAvailable("VertexShaderEnable") 
		&& gSavedSettings.getBOOL("VertexShaderEnable"))
	{
		//using shaders, disable fixed function
		LLGLSLShader::sNoFixedFunction = true;

		S32 light_class = 2;
		S32 env_class = 2;
		S32 obj_class = 2;
		S32 effect_class = 2;
		S32 wl_class = 2;
		S32 water_class = 2;
		S32 deferred_class = 0;
		
		if (LLFeatureManager::getInstance()->isFeatureAvailable("RenderDeferred") &&
		    gSavedSettings.getBOOL("RenderDeferred") &&
			gSavedSettings.getBOOL("RenderAvatarVP") &&
			gSavedSettings.getBOOL("WindLightUseAtmosShaders"))
		{
			if (gSavedSettings.getS32("RenderShadowDetail") > 0)
			{
				if (gSavedSettings.getBOOL("RenderDeferredGI"))
				{ //shadows + gi
					deferred_class = 3;
				}
				else
				{ //shadows
					deferred_class = 2;
				}
			}
			else
			{ //no shadows
				deferred_class = 1;
			}

			//make sure hardware skinning is enabled
			//gSavedSettings.setBOOL("RenderAvatarVP", TRUE);
			
			//make sure atmospheric shaders are enabled
			//gSavedSettings.setBOOL("WindLightUseAtmosShaders", TRUE);
		}


		if (!(LLFeatureManager::getInstance()->isFeatureAvailable("WindLightUseAtmosShaders")
			  && gSavedSettings.getBOOL("WindLightUseAtmosShaders")))
		{
			// user has disabled WindLight in their settings, downgrade
			// windlight shaders to stub versions.
			wl_class = 1;
		}

		if(!gSavedSettings.getBOOL("EnableRippleWater"))
		{
			water_class = 0;
		}

		// Trigger a full rebuild of the fallback skybox / cubemap if we've toggled windlight shaders
		if (mVertexShaderLevel[SHADER_WINDLIGHT] != wl_class && gSky.mVOSkyp.notNull())
		{
			gSky.mVOSkyp->forceSkyUpdate();
		}

		// Load lighting shaders
		mVertexShaderLevel[SHADER_LIGHTING] = light_class;
		mVertexShaderLevel[SHADER_INTERFACE] = light_class;
		mVertexShaderLevel[SHADER_ENVIRONMENT] = env_class;
		mVertexShaderLevel[SHADER_WATER] = water_class;
		mVertexShaderLevel[SHADER_OBJECT] = obj_class;
		mVertexShaderLevel[SHADER_EFFECT] = effect_class;
		mVertexShaderLevel[SHADER_WINDLIGHT] = wl_class;
		mVertexShaderLevel[SHADER_DEFERRED] = deferred_class;

		BOOL loaded = loadBasicShaders();

		if (loaded)
		{
			gPipeline.mVertexShadersEnabled = TRUE;
			gPipeline.mVertexShadersLoaded = 1;

			// Load all shaders to set max levels
			loadShadersEnvironment();
			loadShadersWater();
			loadShadersWindLight();
			loadShadersEffects();
			loadShadersInterface();
			
			// Load max avatar shaders to set the max level
			mVertexShaderLevel[SHADER_AVATAR] = 3;
			mMaxAvatarShaderLevel = 3;
						
			if (gSavedSettings.getBOOL("RenderAvatarVP") && loadShadersObject())
			{ //hardware skinning is enabled and rigged attachment shaders loaded correctly
				BOOL avatar_cloth = gSavedSettings.getBOOL("RenderAvatarCloth");
				S32 avatar_class = 1;
				
				// cloth is a class3 shader
				if(avatar_cloth)
				{
					avatar_class = 3;
				}

				// Set the actual level
				mVertexShaderLevel[SHADER_AVATAR] = avatar_class;
				loadShadersAvatar();
				if (mVertexShaderLevel[SHADER_AVATAR] != avatar_class)
				{
					if (mVertexShaderLevel[SHADER_AVATAR] == 0)
					{
						gSavedSettings.setBOOL("RenderAvatarVP", FALSE);
					}
					if(llmax(mVertexShaderLevel[SHADER_AVATAR]-1,0) >= 3)
					{
						avatar_cloth = true;
					}
					else
					{
						avatar_cloth = false;
					}
					gSavedSettings.setBOOL("RenderAvatarCloth", avatar_cloth);
				}
			}
			else
			{ //hardware skinning not possible, neither is deferred rendering
				mVertexShaderLevel[SHADER_AVATAR] = 0;
				mVertexShaderLevel[SHADER_DEFERRED] = 0;

				if (gSavedSettings.getBOOL("RenderAvatarVP"))
				{
					gSavedSettings.setBOOL("RenderDeferred", FALSE);
					gSavedSettings.setBOOL("RenderAvatarCloth", FALSE);
					gSavedSettings.setBOOL("RenderAvatarVP", FALSE);
				}

				loadShadersAvatar(); // unloads
				loadShadersObject();
			}

			if (!loadShadersDeferred())
			{
				gSavedSettings.setBOOL("RenderDeferred", FALSE);
				reentrance = false;
				setShaders();
				return;
			}
		}
		else
		{
			LLGLSLShader::sNoFixedFunction = false;
			gPipeline.mVertexShadersEnabled = FALSE;
			gPipeline.mVertexShadersLoaded = 0;
			mVertexShaderLevel[SHADER_LIGHTING] = 0;
			mVertexShaderLevel[SHADER_INTERFACE] = 0;
			mVertexShaderLevel[SHADER_ENVIRONMENT] = 0;
			mVertexShaderLevel[SHADER_WATER] = 0;
			mVertexShaderLevel[SHADER_OBJECT] = 0;
			mVertexShaderLevel[SHADER_EFFECT] = 0;
			mVertexShaderLevel[SHADER_WINDLIGHT] = 0;
			mVertexShaderLevel[SHADER_AVATAR] = 0;
		}
	}
	else
	{
		LLGLSLShader::sNoFixedFunction = false;
		gPipeline.mVertexShadersEnabled = FALSE;
		gPipeline.mVertexShadersLoaded = 0;
		mVertexShaderLevel[SHADER_LIGHTING] = 0;
		mVertexShaderLevel[SHADER_INTERFACE] = 0;
		mVertexShaderLevel[SHADER_ENVIRONMENT] = 0;
		mVertexShaderLevel[SHADER_WATER] = 0;
		mVertexShaderLevel[SHADER_OBJECT] = 0;
		mVertexShaderLevel[SHADER_EFFECT] = 0;
		mVertexShaderLevel[SHADER_WINDLIGHT] = 0;
		mVertexShaderLevel[SHADER_AVATAR] = 0;
	}
	
	if (gViewerWindow)
	{
		gViewerWindow->setCursor(UI_CURSOR_ARROW);
	}
	gPipeline.createGLBuffers();

	reentrance = false;
}

void LLViewerShaderMgr::unloadShaders()
{
	gOcclusionProgram.unload();
	gUIProgram.unload();
	gCustomAlphaProgram.unload();
	gGlowCombineProgram.unload();
	gTwoTextureAddProgram.unload();
	gSolidColorProgram.unload();

	gObjectSimpleProgram.unload();
	gImpostorProgram.unload();
	gObjectSimpleAlphaMaskProgram.unload();
	gObjectBumpProgram.unload();
	gObjectSimpleWaterProgram.unload();
	gObjectSimpleWaterAlphaMaskProgram.unload();
	gObjectFullbrightProgram.unload();
	gObjectFullbrightWaterProgram.unload();
	gObjectFullbrightAlphaMaskProgram.unload();
	gObjectFullbrightWaterAlphaMaskProgram.unload();

	gObjectShinyProgram.unload();
	gObjectFullbrightShinyProgram.unload();
	gObjectFullbrightShinyWaterProgram.unload();
	gObjectShinyWaterProgram.unload();

	gObjectSimpleNonIndexedProgram.unload();
	gObjectSimpleNonIndexedWaterProgram.unload();
	gObjectAlphaMaskNonIndexedProgram.unload();
	gObjectAlphaMaskNonIndexedWaterProgram.unload();
	gObjectFullbrightNonIndexedProgram.unload();
	gObjectFullbrightNonIndexedWaterProgram.unload();

	gObjectShinyNonIndexedProgram.unload();
	gObjectFullbrightShinyNonIndexedProgram.unload();
	gObjectFullbrightShinyNonIndexedWaterProgram.unload();
	gObjectShinyNonIndexedWaterProgram.unload();

	gSkinnedObjectSimpleProgram.unload();
	gSkinnedObjectFullbrightProgram.unload();
	gSkinnedObjectFullbrightShinyProgram.unload();
	gSkinnedObjectShinySimpleProgram.unload();
	
	gSkinnedObjectSimpleWaterProgram.unload();
	gSkinnedObjectFullbrightWaterProgram.unload();
	gSkinnedObjectFullbrightShinyWaterProgram.unload();
	gSkinnedObjectShinySimpleWaterProgram.unload();
	

	gWaterProgram.unload();
	gUnderWaterProgram.unload();
	gTerrainProgram.unload();
	gTerrainWaterProgram.unload();
	gGlowProgram.unload();
	gGlowExtractProgram.unload();
	gAvatarProgram.unload();
	gAvatarWaterProgram.unload();
	gAvatarEyeballProgram.unload();
	gAvatarPickProgram.unload();
	gHighlightProgram.unload();

	gWLSkyProgram.unload();
	gWLCloudProgram.unload();

	gPostColorFilterProgram.unload();
	gPostNightVisionProgram.unload();

	gDeferredDiffuseProgram.unload();
	gDeferredDiffuseAlphaMaskProgram.unload();
	gDeferredNonIndexedDiffuseAlphaMaskProgram.unload();
	gDeferredNonIndexedDiffuseProgram.unload();
	gDeferredSkinnedDiffuseProgram.unload();
	gDeferredSkinnedBumpProgram.unload();
	gDeferredSkinnedAlphaProgram.unload();

	mVertexShaderLevel[SHADER_LIGHTING] = 0;
	mVertexShaderLevel[SHADER_OBJECT] = 0;
	mVertexShaderLevel[SHADER_AVATAR] = 0;
	mVertexShaderLevel[SHADER_ENVIRONMENT] = 0;
	mVertexShaderLevel[SHADER_WATER] = 0;
	mVertexShaderLevel[SHADER_INTERFACE] = 0;
	mVertexShaderLevel[SHADER_EFFECT] = 0;
	mVertexShaderLevel[SHADER_WINDLIGHT] = 0;

	gPipeline.mVertexShadersLoaded = 0;
}

BOOL LLViewerShaderMgr::loadBasicShaders()
{
	// Load basic dependency shaders first
	// All of these have to load for any shaders to function
	
	S32 sum_lights_class = 3;

	// class one cards will get the lower sum lights
	// class zero we're not going to think about
	// since a class zero card COULD be a ridiculous new card
	// and old cards should have the features masked
	if(LLFeatureManager::getInstance()->getGPUClass() == GPU_CLASS_1)
	{
		sum_lights_class = 2;
	}

	// If we have sun and moon only checked, then only sum those lights.
	if (gPipeline.getLightingDetail() == 0)
	{
		sum_lights_class = 1;
	}

	// Use the feature table to mask out the max light level to use.  Also make sure it's at least 1.
	S32 max_light_class = gSavedSettings.getS32("RenderShaderLightingMaxLevel");
	sum_lights_class = llclamp(sum_lights_class, 1, max_light_class);

	// Load the Basic Vertex Shaders at the appropriate level. 
	// (in order of shader function call depth for reference purposes, deepest level first)

	vector< pair<string, S32> > shaders;
	shaders.reserve(10);
	shaders.push_back( make_pair( "windlight/atmosphericsVarsV.glsl",		mVertexShaderLevel[SHADER_WINDLIGHT] ) );
	shaders.push_back( make_pair( "windlight/atmosphericsHelpersV.glsl",	mVertexShaderLevel[SHADER_WINDLIGHT] ) );
	shaders.push_back( make_pair( "lighting/lightFuncV.glsl",				mVertexShaderLevel[SHADER_LIGHTING] ) );
	shaders.push_back( make_pair( "lighting/sumLightsV.glsl",				sum_lights_class ) );
	shaders.push_back( make_pair( "lighting/lightV.glsl",					mVertexShaderLevel[SHADER_LIGHTING] ) );
	shaders.push_back( make_pair( "lighting/lightFuncSpecularV.glsl",		mVertexShaderLevel[SHADER_LIGHTING] ) );
	shaders.push_back( make_pair( "lighting/sumLightsSpecularV.glsl",		sum_lights_class ) );
	shaders.push_back( make_pair( "lighting/lightSpecularV.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	shaders.push_back( make_pair( "windlight/atmosphericsV.glsl",			mVertexShaderLevel[SHADER_WINDLIGHT] ) );
	shaders.push_back( make_pair( "avatar/avatarSkinV.glsl",				1 ) );
	shaders.push_back( make_pair( "avatar/objectSkinV.glsl",				1 ) );

	// We no longer have to bind the shaders to global glhandles, they are automatically added to a map now.
	for (U32 i = 0; i < shaders.size(); i++)
	{
		// Note usage of GL_VERTEX_SHADER_ARB
		if (loadShaderFile(shaders[i].first, shaders[i].second, GL_VERTEX_SHADER_ARB) == 0)
		{
			return FALSE;
		}
	}

	// Load the Basic Fragment Shaders at the appropriate level. 
	// (in order of shader function call depth for reference purposes, deepest level first)

	shaders.clear();
	shaders.reserve(13);
	S32 ch = gGLManager.mNumTextureImageUnits-1;

	if (gGLManager.mGLVersion < 3.1f)
	{ //force to 1 texture index channel for old drivers
		ch = 1;
	}

	std::vector<S32> index_channels;
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "windlight/atmosphericsVarsF.glsl",		mVertexShaderLevel[SHADER_WINDLIGHT] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "windlight/gammaF.glsl",					mVertexShaderLevel[SHADER_WINDLIGHT]) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "windlight/atmosphericsF.glsl",			mVertexShaderLevel[SHADER_WINDLIGHT] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "windlight/transportF.glsl",				mVertexShaderLevel[SHADER_WINDLIGHT] ) );	
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "environment/waterFogF.glsl",				mVertexShaderLevel[SHADER_WATER] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightNonIndexedF.glsl",					mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightAlphaMaskNonIndexedF.glsl",					mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightFullbrightNonIndexedF.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightFullbrightNonIndexedAlphaMaskF.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightWaterNonIndexedF.glsl",				mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightWaterAlphaMaskNonIndexedF.glsl",				mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightFullbrightWaterNonIndexedF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightFullbrightWaterNonIndexedAlphaMaskF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightShinyNonIndexedF.glsl",				mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightFullbrightShinyNonIndexedF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightShinyWaterNonIndexedF.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(-1);	 shaders.push_back( make_pair( "lighting/lightFullbrightShinyWaterNonIndexedF.glsl", mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightF.glsl",					mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightAlphaMaskF.glsl",					mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightFullbrightF.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightFullbrightAlphaMaskF.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightWaterF.glsl",				mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightWaterAlphaMaskF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightFullbrightWaterF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightFullbrightWaterAlphaMaskF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightShinyF.glsl",				mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightFullbrightShinyF.glsl",	mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightShinyWaterF.glsl",			mVertexShaderLevel[SHADER_LIGHTING] ) );
	index_channels.push_back(ch);	 shaders.push_back( make_pair( "lighting/lightFullbrightShinyWaterF.glsl", mVertexShaderLevel[SHADER_LIGHTING] ) );
	
	for (U32 i = 0; i < shaders.size(); i++)
	{
		// Note usage of GL_FRAGMENT_SHADER_ARB
		if (loadShaderFile(shaders[i].first, shaders[i].second, GL_FRAGMENT_SHADER_ARB, index_channels[i]) == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL LLViewerShaderMgr::loadShadersEnvironment()
{
	BOOL success = TRUE;

	if (mVertexShaderLevel[SHADER_ENVIRONMENT] == 0)
	{
		gTerrainProgram.unload();
		return FALSE;
	}

	if (success)
	{
		gTerrainProgram.mName = "Terrain Shader";
		gTerrainProgram.mFeatures.calculatesLighting = true;
		gTerrainProgram.mFeatures.calculatesAtmospherics = true;
		gTerrainProgram.mFeatures.hasAtmospherics = true;
		gTerrainProgram.mFeatures.hasGamma = true;
		gTerrainProgram.mShaderFiles.clear();
		gTerrainProgram.mShaderFiles.push_back(make_pair("environment/terrainV.glsl", GL_VERTEX_SHADER_ARB));
		gTerrainProgram.mShaderFiles.push_back(make_pair("environment/terrainF.glsl", GL_FRAGMENT_SHADER_ARB));
		gTerrainProgram.mShaderLevel = mVertexShaderLevel[SHADER_ENVIRONMENT];
		success = gTerrainProgram.createShader(NULL, &mTerrainUniforms);
	}

	if (!success)
	{
		mVertexShaderLevel[SHADER_ENVIRONMENT] = 0;
		return FALSE;
	}
	
	LLWorld::getInstance()->updateWaterObjects();
	
	return TRUE;
}

BOOL LLViewerShaderMgr::loadShadersWater()
{
	BOOL success = TRUE;
	BOOL terrainWaterSuccess = TRUE;

	if (mVertexShaderLevel[SHADER_WATER] == 0)
	{
		gWaterProgram.unload();
		gUnderWaterProgram.unload();
		gTerrainWaterProgram.unload();
		return FALSE;
	}

	if (success)
	{
		// load water shader
		gWaterProgram.mName = "Water Shader";
		gWaterProgram.mFeatures.calculatesAtmospherics = true;
		gWaterProgram.mFeatures.hasGamma = true;
		gWaterProgram.mFeatures.hasTransport = true;
		gWaterProgram.mShaderFiles.clear();
		gWaterProgram.mShaderFiles.push_back(make_pair("environment/waterV.glsl", GL_VERTEX_SHADER_ARB));
		gWaterProgram.mShaderFiles.push_back(make_pair("environment/waterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_WATER];
		success = gWaterProgram.createShader(NULL, &mWaterUniforms);
	}

	if (success)
	{
		//load under water vertex shader
		gUnderWaterProgram.mName = "Underwater Shader";
		gUnderWaterProgram.mFeatures.calculatesAtmospherics = true;
		gUnderWaterProgram.mShaderFiles.clear();
		gUnderWaterProgram.mShaderFiles.push_back(make_pair("environment/waterV.glsl", GL_VERTEX_SHADER_ARB));
		gUnderWaterProgram.mShaderFiles.push_back(make_pair("environment/underWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gUnderWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_WATER];
		gUnderWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		
		success = gUnderWaterProgram.createShader(NULL, &mWaterUniforms);
	}

	if (success)
	{
		//load terrain water shader
		gTerrainWaterProgram.mName = "Terrain Water Shader";
		gTerrainWaterProgram.mFeatures.calculatesLighting = true;
		gTerrainWaterProgram.mFeatures.calculatesAtmospherics = true;
		gTerrainWaterProgram.mFeatures.hasAtmospherics = true;
		gTerrainWaterProgram.mFeatures.hasWaterFog = true;
		gTerrainWaterProgram.mShaderFiles.clear();
		gTerrainWaterProgram.mShaderFiles.push_back(make_pair("environment/terrainV.glsl", GL_VERTEX_SHADER_ARB));
		gTerrainWaterProgram.mShaderFiles.push_back(make_pair("environment/terrainWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gTerrainWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_ENVIRONMENT];
		gTerrainWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		terrainWaterSuccess = gTerrainWaterProgram.createShader(NULL, &mTerrainUniforms);
	}	

	/// Keep track of water shader levels
	if (gWaterProgram.mShaderLevel != mVertexShaderLevel[SHADER_WATER]
		|| gUnderWaterProgram.mShaderLevel != mVertexShaderLevel[SHADER_WATER])
	{
		mVertexShaderLevel[SHADER_WATER] = llmin(gWaterProgram.mShaderLevel, gUnderWaterProgram.mShaderLevel);
	}

	if (!success)
	{
		mVertexShaderLevel[SHADER_WATER] = 0;
		return FALSE;
	}

	// if we failed to load the terrain water shaders and we need them (using class2 water),
	// then drop down to class1 water.
	if (mVertexShaderLevel[SHADER_WATER] > 1 && !terrainWaterSuccess)
	{
		mVertexShaderLevel[SHADER_WATER]--;
		return loadShadersWater();
	}
	
	LLWorld::getInstance()->updateWaterObjects();

	return TRUE;
}

BOOL LLViewerShaderMgr::loadShadersEffects()
{
	BOOL success = TRUE;

	U32 samples = gGLManager.getNumFBOFSAASamples(gSavedSettings.getU32("RenderFSAASamples"));
	bool multisample = samples > 1 && LLPipeline::sRenderDeferred && gGLManager.mHasTextureMultisample;

	if (mVertexShaderLevel[SHADER_EFFECT] == 0)
	{
		gGlowProgram.unload();
		gGlowExtractProgram.unload();
		gPostColorFilterProgram.unload();	
		gPostNightVisionProgram.unload();
		return FALSE;
	}

	if (success)
	{
		gGlowProgram.mName = "Glow Shader (Post)";
		gGlowProgram.mShaderFiles.clear();
		gGlowProgram.mShaderFiles.push_back(make_pair("effects/glowV.glsl", GL_VERTEX_SHADER_ARB));
		gGlowProgram.mShaderFiles.push_back(make_pair("effects/glowF.glsl", GL_FRAGMENT_SHADER_ARB));
		gGlowProgram.mShaderLevel = mVertexShaderLevel[SHADER_EFFECT];
		success = gGlowProgram.createShader(NULL, &mGlowUniforms);
		if (!success)
		{
			LLPipeline::sRenderGlow = FALSE;
		}
	}
	
	if (success)
	{
		std::string fragment;

		if (multisample)
		{
			fragment = "effects/glowExtractMSF.glsl";
		}
		else
		{
			fragment = "effects/glowExtractF.glsl";
		}

		gGlowExtractProgram.mName = "Glow Extract Shader (Post)";
		gGlowExtractProgram.mShaderFiles.clear();
		gGlowExtractProgram.mShaderFiles.push_back(make_pair("effects/glowExtractV.glsl", GL_VERTEX_SHADER_ARB));
		gGlowExtractProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gGlowExtractProgram.mShaderLevel = mVertexShaderLevel[SHADER_EFFECT];
		success = gGlowExtractProgram.createShader(NULL, &mGlowExtractUniforms);
		if (!success)
		{
			LLPipeline::sRenderGlow = FALSE;
		}
	}
	
#if 0
	// disabling loading of postprocess shaders until we fix
	// ATI sampler2DRect compatibility.
	
	//load Color Filter Shader
	if (success)
	{
		vector<string> shaderUniforms;
		shaderUniforms.reserve(7);
		shaderUniforms.push_back("RenderTexture");
		shaderUniforms.push_back("gamma");
		shaderUniforms.push_back("brightness");
		shaderUniforms.push_back("contrast");
		shaderUniforms.push_back("contrastBase");
		shaderUniforms.push_back("saturation");
		shaderUniforms.push_back("lumWeights");

		gPostColorFilterProgram.mName = "Color Filter Shader (Post)";
		gPostColorFilterProgram.mShaderFiles.clear();
		gPostColorFilterProgram.mShaderFiles.push_back(make_pair("effects/colorFilterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gPostColorFilterProgram.mShaderFiles.push_back(make_pair("effects/drawQuadV.glsl", GL_VERTEX_SHADER_ARB));
		gPostColorFilterProgram.mShaderLevel = mVertexShaderLevel[SHADER_EFFECT];
		success = gPostColorFilterProgram.createShader(NULL, &shaderUniforms);
	}

	//load Night Vision Shader
	if (success)
	{
		vector<string> shaderUniforms;
		shaderUniforms.reserve(5);
		shaderUniforms.push_back("RenderTexture");
		shaderUniforms.push_back("NoiseTexture");
		shaderUniforms.push_back("brightMult");
		shaderUniforms.push_back("noiseStrength");
		shaderUniforms.push_back("lumWeights");

		gPostNightVisionProgram.mName = "Night Vision Shader (Post)";
		gPostNightVisionProgram.mShaderFiles.clear();
		gPostNightVisionProgram.mShaderFiles.push_back(make_pair("effects/nightVisionF.glsl", GL_FRAGMENT_SHADER_ARB));
		gPostNightVisionProgram.mShaderFiles.push_back(make_pair("effects/drawQuadV.glsl", GL_VERTEX_SHADER_ARB));
		gPostNightVisionProgram.mShaderLevel = mVertexShaderLevel[SHADER_EFFECT];
		success = gPostNightVisionProgram.createShader(NULL, &shaderUniforms);
	}
	#endif

	return success;

}

BOOL LLViewerShaderMgr::loadShadersDeferred()
{
	if (mVertexShaderLevel[SHADER_DEFERRED] == 0)
	{
		gDeferredTreeProgram.unload();
		gDeferredDiffuseProgram.unload();
		gDeferredDiffuseAlphaMaskProgram.unload();
		gDeferredNonIndexedDiffuseAlphaMaskProgram.unload();
		gDeferredNonIndexedDiffuseProgram.unload();
		gDeferredSkinnedDiffuseProgram.unload();
		gDeferredSkinnedBumpProgram.unload();
		gDeferredSkinnedAlphaProgram.unload();
		gDeferredBumpProgram.unload();
		gDeferredImpostorProgram.unload();
		gDeferredTerrainProgram.unload();
		gDeferredLightProgram.unload();
		gDeferredMultiLightProgram.unload();
		gDeferredSpotLightProgram.unload();
		gDeferredMultiSpotLightProgram.unload();
		gDeferredSunProgram.unload();
		gDeferredBlurLightProgram.unload();
		gDeferredSoftenProgram.unload();
		gDeferredShadowProgram.unload();
		gDeferredShadowAlphaMaskProgram.unload();
		gDeferredAvatarShadowProgram.unload();
		gDeferredAttachmentShadowProgram.unload();
		gDeferredAvatarProgram.unload();
		gDeferredAvatarAlphaProgram.unload();
		gDeferredAlphaProgram.unload();
		gDeferredFullbrightProgram.unload();
		gDeferredAvatarEyesProgram.unload();
		gDeferredPostGIProgram.unload();		
		gDeferredEdgeProgram.unload();		
		gDeferredPostProgram.unload();		
		gLuminanceGatherProgram.unload();
		gDeferredGIProgram.unload();
		gDeferredGIFinalProgram.unload();
		gDeferredWaterProgram.unload();
		gDeferredWLSkyProgram.unload();
		gDeferredWLCloudProgram.unload();
		gDeferredStarProgram.unload();
		return TRUE;
	}

	mVertexShaderLevel[SHADER_AVATAR] = 1;

	BOOL success = TRUE;

	U32 samples = gGLManager.getNumFBOFSAASamples(gSavedSettings.getU32("RenderFSAASamples"));
	bool multisample = samples > 1 && gGLManager.mHasTextureMultisample;

	if (success)
	{
		gDeferredDiffuseProgram.mName = "Deferred Diffuse Shader";
		gDeferredDiffuseProgram.mShaderFiles.clear();
		gDeferredDiffuseProgram.mShaderFiles.push_back(make_pair("deferred/diffuseV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredDiffuseProgram.mShaderFiles.push_back(make_pair("deferred/diffuseIndexedF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredDiffuseProgram.mFeatures.mIndexedTextureChannels = gGLManager.mNumTextureImageUnits;
		gDeferredDiffuseProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredDiffuseProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredDiffuseAlphaMaskProgram.mName = "Deferred Diffuse Alpha Mask Shader";
		gDeferredDiffuseAlphaMaskProgram.mShaderFiles.clear();
		gDeferredDiffuseAlphaMaskProgram.mShaderFiles.push_back(make_pair("deferred/diffuseV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredDiffuseAlphaMaskProgram.mShaderFiles.push_back(make_pair("deferred/diffuseAlphaMaskIndexedF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredDiffuseAlphaMaskProgram.mFeatures.mIndexedTextureChannels = gGLManager.mNumTextureImageUnits;
		gDeferredDiffuseAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredDiffuseAlphaMaskProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredNonIndexedDiffuseAlphaMaskProgram.mName = "Deferred Diffuse Non-Indexed Alpha Mask Shader";
		gDeferredNonIndexedDiffuseAlphaMaskProgram.mShaderFiles.clear();
		gDeferredNonIndexedDiffuseAlphaMaskProgram.mShaderFiles.push_back(make_pair("deferred/diffuseV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredNonIndexedDiffuseAlphaMaskProgram.mShaderFiles.push_back(make_pair("deferred/diffuseAlphaMaskF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredNonIndexedDiffuseAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredNonIndexedDiffuseAlphaMaskProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredNonIndexedDiffuseProgram.mName = "Non Indexed Deferred Diffuse Shader";
		gDeferredNonIndexedDiffuseProgram.mShaderFiles.clear();
		gDeferredNonIndexedDiffuseProgram.mShaderFiles.push_back(make_pair("deferred/diffuseV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredNonIndexedDiffuseProgram.mShaderFiles.push_back(make_pair("deferred/diffuseF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredNonIndexedDiffuseProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredNonIndexedDiffuseProgram.createShader(NULL, NULL);
	}
		

	if (success)
	{
		gDeferredSkinnedDiffuseProgram.mName = "Deferred Skinned Diffuse Shader";
		gDeferredSkinnedDiffuseProgram.mFeatures.hasObjectSkinning = true;
		gDeferredSkinnedDiffuseProgram.mShaderFiles.clear();
		gDeferredSkinnedDiffuseProgram.mShaderFiles.push_back(make_pair("deferred/diffuseSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredSkinnedDiffuseProgram.mShaderFiles.push_back(make_pair("deferred/diffuseF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredSkinnedDiffuseProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredSkinnedDiffuseProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredSkinnedBumpProgram.mName = "Deferred Skinned Bump Shader";
		gDeferredSkinnedBumpProgram.mFeatures.hasObjectSkinning = true;
		gDeferredSkinnedBumpProgram.mShaderFiles.clear();
		gDeferredSkinnedBumpProgram.mShaderFiles.push_back(make_pair("deferred/bumpSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredSkinnedBumpProgram.mShaderFiles.push_back(make_pair("deferred/bumpF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredSkinnedBumpProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredSkinnedBumpProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredSkinnedAlphaProgram.mName = "Deferred Skinned Alpha Shader";
		gDeferredSkinnedAlphaProgram.mFeatures.hasObjectSkinning = true;
		gDeferredSkinnedAlphaProgram.mFeatures.calculatesAtmospherics = true;
		gDeferredSkinnedAlphaProgram.mFeatures.hasGamma = true;
		gDeferredSkinnedAlphaProgram.mFeatures.hasAtmospherics = true;
		gDeferredSkinnedAlphaProgram.mFeatures.calculatesLighting = true;
		gDeferredSkinnedAlphaProgram.mFeatures.hasLighting = true;
		gDeferredSkinnedAlphaProgram.mFeatures.isAlphaLighting = true;
		gDeferredSkinnedAlphaProgram.mFeatures.disableTextureIndex = true;
		gDeferredSkinnedAlphaProgram.mShaderFiles.clear();
		gDeferredSkinnedAlphaProgram.mShaderFiles.push_back(make_pair("deferred/alphaSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredSkinnedAlphaProgram.mShaderFiles.push_back(make_pair("deferred/alphaNonIndexedF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredSkinnedAlphaProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredSkinnedAlphaProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredBumpProgram.mName = "Deferred Bump Shader";
		gDeferredBumpProgram.mShaderFiles.clear();
		gDeferredBumpProgram.mShaderFiles.push_back(make_pair("deferred/bumpV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredBumpProgram.mShaderFiles.push_back(make_pair("deferred/bumpF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredBumpProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredBumpProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredTreeProgram.mName = "Deferred Tree Shader";
		gDeferredTreeProgram.mShaderFiles.clear();
		gDeferredTreeProgram.mShaderFiles.push_back(make_pair("deferred/treeV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredTreeProgram.mShaderFiles.push_back(make_pair("deferred/treeF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredTreeProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredTreeProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredImpostorProgram.mName = "Deferred Impostor Shader";
		gDeferredImpostorProgram.mShaderFiles.clear();
		gDeferredImpostorProgram.mShaderFiles.push_back(make_pair("deferred/impostorV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredImpostorProgram.mShaderFiles.push_back(make_pair("deferred/impostorF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredImpostorProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredImpostorProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;

		if (multisample)
		{
			fragment = "deferred/pointLightMSF.glsl";
		}
		else
		{
			fragment = "deferred/pointLightF.glsl";
		}

		gDeferredLightProgram.mName = "Deferred Light Shader";
		gDeferredLightProgram.mShaderFiles.clear();
		gDeferredLightProgram.mShaderFiles.push_back(make_pair("deferred/pointLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredLightProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredLightProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredLightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;
		if (multisample)
		{
			fragment = "deferred/multiPointLightMSF.glsl";
		}
		else
		{
			fragment = "deferred/multiPointLightF.glsl";
		}

		gDeferredMultiLightProgram.mName = "Deferred MultiLight Shader";
		gDeferredMultiLightProgram.mShaderFiles.clear();
		gDeferredMultiLightProgram.mShaderFiles.push_back(make_pair("deferred/multiPointLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredMultiLightProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredMultiLightProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredMultiLightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;

		if (multisample)
		{
			fragment = "deferred/spotLightMSF.glsl";
		}
		else
		{
			fragment = "deferred/multiSpotLightF.glsl";
		}

		gDeferredSpotLightProgram.mName = "Deferred SpotLight Shader";
		gDeferredSpotLightProgram.mShaderFiles.clear();
		gDeferredSpotLightProgram.mShaderFiles.push_back(make_pair("deferred/pointLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredSpotLightProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredSpotLightProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredSpotLightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;

		if (multisample)
		{
			fragment = "deferred/multiSpotLightMSF.glsl";
		}
		else
		{
			fragment = "deferred/multiSpotLightF.glsl";
		}

		gDeferredMultiSpotLightProgram.mName = "Deferred MultiSpotLight Shader";
		gDeferredMultiSpotLightProgram.mShaderFiles.clear();
		gDeferredMultiSpotLightProgram.mShaderFiles.push_back(make_pair("deferred/pointLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredMultiSpotLightProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredMultiSpotLightProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredMultiSpotLightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;

		if (gSavedSettings.getBOOL("RenderDeferredSSAO"))
		{
			if (multisample)
			{
				fragment = "deferred/sunLightSSAOMSF.glsl";
			}
			else
			{
				fragment = "deferred/sunLightSSAOF.glsl";
			}
		}
		else
		{
			if (multisample)
			{
				fragment = "deferred/sunLightMSF.glsl";
			}
			else
			{
				fragment = "deferred/sunLightF.glsl";
			}
		}

		gDeferredSunProgram.mName = "Deferred Sun Shader";
		gDeferredSunProgram.mShaderFiles.clear();
		gDeferredSunProgram.mShaderFiles.push_back(make_pair("deferred/sunLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredSunProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredSunProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredSunProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;

		if (multisample)
		{
			fragment = "deferred/blurLightMSF.glsl";
		}
		else
		{
			fragment = "deferred/blurLightF.glsl";
		}

		gDeferredBlurLightProgram.mName = "Deferred Blur Light Shader";
		gDeferredBlurLightProgram.mShaderFiles.clear();
		gDeferredBlurLightProgram.mShaderFiles.push_back(make_pair("deferred/blurLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredBlurLightProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredBlurLightProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredBlurLightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredAlphaProgram.mName = "Deferred Alpha Shader";
		gDeferredAlphaProgram.mFeatures.calculatesLighting = true;
		gDeferredAlphaProgram.mFeatures.calculatesAtmospherics = true;
		gDeferredAlphaProgram.mFeatures.hasGamma = true;
		gDeferredAlphaProgram.mFeatures.hasAtmospherics = true;
		gDeferredAlphaProgram.mFeatures.hasLighting = true;
		gDeferredAlphaProgram.mFeatures.isAlphaLighting = true;
		gDeferredAlphaProgram.mFeatures.disableTextureIndex = true; //hack to disable auto-setup of texture channels
		if (mVertexShaderLevel[SHADER_DEFERRED] < 1)
		{
			gDeferredAlphaProgram.mFeatures.mIndexedTextureChannels = gGLManager.mNumTextureImageUnits;
		}
		else
		{ //shave off some texture units for shadow maps
			gDeferredAlphaProgram.mFeatures.mIndexedTextureChannels = gGLManager.mNumTextureImageUnits - 6;
		}
			
		gDeferredAlphaProgram.mShaderFiles.clear();
		gDeferredAlphaProgram.mShaderFiles.push_back(make_pair("deferred/alphaV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredAlphaProgram.mShaderFiles.push_back(make_pair("deferred/alphaF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredAlphaProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredAlphaProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredAvatarEyesProgram.mName = "Deferred Avatar Eyes Shader";
		gDeferredAvatarEyesProgram.mFeatures.calculatesAtmospherics = true;
		gDeferredAvatarEyesProgram.mFeatures.hasGamma = true;
		gDeferredAvatarEyesProgram.mFeatures.hasTransport = true;
		gDeferredAvatarEyesProgram.mFeatures.disableTextureIndex = true;
		gDeferredAvatarEyesProgram.mShaderFiles.clear();
		gDeferredAvatarEyesProgram.mShaderFiles.push_back(make_pair("deferred/avatarEyesV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredAvatarEyesProgram.mShaderFiles.push_back(make_pair("deferred/diffuseF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredAvatarEyesProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredAvatarEyesProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredFullbrightProgram.mName = "Deferred Fullbright Shader";
		gDeferredFullbrightProgram.mFeatures.calculatesAtmospherics = true;
		gDeferredFullbrightProgram.mFeatures.hasGamma = true;
		gDeferredFullbrightProgram.mFeatures.hasTransport = true;
		gDeferredFullbrightProgram.mFeatures.mIndexedTextureChannels = gGLManager.mNumTextureImageUnits;
		gDeferredFullbrightProgram.mShaderFiles.clear();
		gDeferredFullbrightProgram.mShaderFiles.push_back(make_pair("deferred/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredFullbrightProgram.mShaderFiles.push_back(make_pair("deferred/fullbrightF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredFullbrightProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredFullbrightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		// load water shader
		gDeferredWaterProgram.mName = "Deferred Water Shader";
		gDeferredWaterProgram.mFeatures.calculatesAtmospherics = true;
		gDeferredWaterProgram.mFeatures.hasGamma = true;
		gDeferredWaterProgram.mFeatures.hasTransport = true;
		gDeferredWaterProgram.mShaderFiles.clear();
		gDeferredWaterProgram.mShaderFiles.push_back(make_pair("deferred/waterV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredWaterProgram.mShaderFiles.push_back(make_pair("deferred/waterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredWaterProgram.createShader(NULL, &mWaterUniforms);
	}

	if (success)
	{
		std::string fragment;

		if (multisample)
		{
			fragment = "deferred/softenLightMSF.glsl";
		}
		else
		{
			fragment = "deferred/softenLightF.glsl";
		}

		gDeferredSoftenProgram.mName = "Deferred Soften Shader";
		gDeferredSoftenProgram.mShaderFiles.clear();
		gDeferredSoftenProgram.mShaderFiles.push_back(make_pair("deferred/softenLightV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredSoftenProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));

		gDeferredSoftenProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];

		if (gSavedSettings.getBOOL("RenderDeferredSSAO"))
		{ //if using SSAO, take screen space light map into account as if shadows are enabled
			gDeferredSoftenProgram.mShaderLevel = llmax(gDeferredSoftenProgram.mShaderLevel, 2);
		}
				
		success = gDeferredSoftenProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredShadowProgram.mName = "Deferred Shadow Shader";
		gDeferredShadowProgram.mShaderFiles.clear();
		gDeferredShadowProgram.mShaderFiles.push_back(make_pair("deferred/shadowV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredShadowProgram.mShaderFiles.push_back(make_pair("deferred/shadowF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredShadowProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredShadowProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredShadowAlphaMaskProgram.mName = "Deferred Shadow Alpha Mask Shader";
		gDeferredShadowAlphaMaskProgram.mShaderFiles.clear();
		gDeferredShadowAlphaMaskProgram.mShaderFiles.push_back(make_pair("deferred/shadowAlphaMaskV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredShadowAlphaMaskProgram.mShaderFiles.push_back(make_pair("deferred/shadowAlphaMaskF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredShadowAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredShadowAlphaMaskProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredAvatarShadowProgram.mName = "Deferred Avatar Shadow Shader";
		gDeferredAvatarShadowProgram.mFeatures.hasSkinning = true;
		gDeferredAvatarShadowProgram.mShaderFiles.clear();
		gDeferredAvatarShadowProgram.mShaderFiles.push_back(make_pair("deferred/avatarShadowV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredAvatarShadowProgram.mShaderFiles.push_back(make_pair("deferred/avatarShadowF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredAvatarShadowProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredAvatarShadowProgram.createShader(&mAvatarAttribs, &mAvatarUniforms);
	}

	if (success)
	{
		gDeferredAttachmentShadowProgram.mName = "Deferred Attachment Shadow Shader";
		gDeferredAttachmentShadowProgram.mFeatures.hasObjectSkinning = true;
		gDeferredAttachmentShadowProgram.mShaderFiles.clear();
		gDeferredAttachmentShadowProgram.mShaderFiles.push_back(make_pair("deferred/attachmentShadowV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredAttachmentShadowProgram.mShaderFiles.push_back(make_pair("deferred/attachmentShadowF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredAttachmentShadowProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredAttachmentShadowProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gTerrainProgram.mName = "Deferred Terrain Shader";
		gDeferredTerrainProgram.mShaderFiles.clear();
		gDeferredTerrainProgram.mShaderFiles.push_back(make_pair("deferred/terrainV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredTerrainProgram.mShaderFiles.push_back(make_pair("deferred/terrainF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredTerrainProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredTerrainProgram.createShader(NULL, &mTerrainUniforms);
	}

	if (success)
	{
		gDeferredAvatarProgram.mName = "Avatar Shader";
		gDeferredAvatarProgram.mFeatures.hasSkinning = true;
		gDeferredAvatarProgram.mShaderFiles.clear();
		gDeferredAvatarProgram.mShaderFiles.push_back(make_pair("deferred/avatarV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredAvatarProgram.mShaderFiles.push_back(make_pair("deferred/avatarF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredAvatarProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredAvatarProgram.createShader(&mAvatarAttribs, &mAvatarUniforms);
	}

	if (success)
	{
		gDeferredAvatarAlphaProgram.mName = "Avatar Alpha Shader";
		gDeferredAvatarAlphaProgram.mFeatures.hasSkinning = true;
		gDeferredAvatarAlphaProgram.mFeatures.calculatesLighting = true;
		gDeferredAvatarAlphaProgram.mFeatures.calculatesAtmospherics = true;
		gDeferredAvatarAlphaProgram.mFeatures.hasGamma = true;
		gDeferredAvatarAlphaProgram.mFeatures.hasAtmospherics = true;
		gDeferredAvatarAlphaProgram.mFeatures.hasLighting = true;
		gDeferredAvatarAlphaProgram.mFeatures.isAlphaLighting = true;
		gDeferredAvatarAlphaProgram.mFeatures.disableTextureIndex = true;
		gDeferredAvatarAlphaProgram.mShaderFiles.clear();
		gDeferredAvatarAlphaProgram.mShaderFiles.push_back(make_pair("deferred/avatarAlphaV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredAvatarAlphaProgram.mShaderFiles.push_back(make_pair("deferred/alphaNonIndexedF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredAvatarAlphaProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredAvatarAlphaProgram.createShader(&mAvatarAttribs, &mAvatarUniforms);
	}

	if (success)
	{
		std::string fragment;
		if (multisample)
		{
			fragment = "deferred/postDeferredMSF.glsl";
		}
		else
		{
			fragment = "deferred/postDeferredF.glsl";
		}

		gDeferredPostProgram.mName = "Deferred Post Shader";
		gDeferredPostProgram.mShaderFiles.clear();
		gDeferredPostProgram.mShaderFiles.push_back(make_pair("deferred/postDeferredV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredPostProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredPostProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredPostProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		std::string fragment;
		if (multisample)
		{
			fragment = "deferred/postDeferredNoDoFMSF.glsl";
		}
		else
		{
			fragment = "deferred/postDeferredNoDoFF.glsl";
		}

		gDeferredPostNoDoFProgram.mName = "Deferred Post Shader";
		gDeferredPostNoDoFProgram.mShaderFiles.clear();
		gDeferredPostNoDoFProgram.mShaderFiles.push_back(make_pair("deferred/postDeferredV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredPostNoDoFProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
		gDeferredPostNoDoFProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		success = gDeferredPostNoDoFProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gDeferredWLSkyProgram.mName = "Deferred Windlight Sky Shader";
		//gWLSkyProgram.mFeatures.hasGamma = true;
		gDeferredWLSkyProgram.mShaderFiles.clear();
		gDeferredWLSkyProgram.mShaderFiles.push_back(make_pair("deferred/skyV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredWLSkyProgram.mShaderFiles.push_back(make_pair("deferred/skyF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredWLSkyProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		gDeferredWLSkyProgram.mShaderGroup = LLGLSLShader::SG_SKY;
		success = gDeferredWLSkyProgram.createShader(NULL, &mWLUniforms);
	}

	if (success)
	{
		gDeferredWLCloudProgram.mName = "Deferred Windlight Cloud Program";
		gDeferredWLCloudProgram.mShaderFiles.clear();
		gDeferredWLCloudProgram.mShaderFiles.push_back(make_pair("deferred/cloudsV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredWLCloudProgram.mShaderFiles.push_back(make_pair("deferred/cloudsF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredWLCloudProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		gDeferredWLCloudProgram.mShaderGroup = LLGLSLShader::SG_SKY;
		success = gDeferredWLCloudProgram.createShader(NULL, &mWLUniforms);
	}

	if (success)
	{
		gDeferredStarProgram.mName = "Deferred Star Program";
		gDeferredStarProgram.mShaderFiles.clear();
		gDeferredStarProgram.mShaderFiles.push_back(make_pair("deferred/starsV.glsl", GL_VERTEX_SHADER_ARB));
		gDeferredStarProgram.mShaderFiles.push_back(make_pair("deferred/starsF.glsl", GL_FRAGMENT_SHADER_ARB));
		gDeferredStarProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
		gDeferredStarProgram.mShaderGroup = LLGLSLShader::SG_SKY;
		success = gDeferredStarProgram.createShader(NULL, &mWLUniforms);
	}

	if (mVertexShaderLevel[SHADER_DEFERRED] > 1)
	{
		if (success)
		{
			std::string fragment;
			if (multisample)
			{
				fragment = "deferred/edgeMSF.glsl";
			}
			else
			{
				fragment = "deferred/edgeF.glsl";
			}

			gDeferredEdgeProgram.mName = "Deferred Edge Shader";
			gDeferredEdgeProgram.mShaderFiles.clear();
			gDeferredEdgeProgram.mShaderFiles.push_back(make_pair("deferred/edgeV.glsl", GL_VERTEX_SHADER_ARB));
			gDeferredEdgeProgram.mShaderFiles.push_back(make_pair(fragment, GL_FRAGMENT_SHADER_ARB));
			gDeferredEdgeProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
			success = gDeferredEdgeProgram.createShader(NULL, NULL);
		}
	}

	if (mVertexShaderLevel[SHADER_DEFERRED] > 2)
	{
		if (success)
		{
			gDeferredPostGIProgram.mName = "Deferred Post GI Shader";
			gDeferredPostGIProgram.mShaderFiles.clear();
			gDeferredPostGIProgram.mShaderFiles.push_back(make_pair("deferred/postgiV.glsl", GL_VERTEX_SHADER_ARB));
			gDeferredPostGIProgram.mShaderFiles.push_back(make_pair("deferred/postgiF.glsl", GL_FRAGMENT_SHADER_ARB));
			gDeferredPostGIProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
			success = gDeferredPostGIProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gDeferredGIProgram.mName = "Deferred GI Shader";
			gDeferredGIProgram.mShaderFiles.clear();
			gDeferredGIProgram.mShaderFiles.push_back(make_pair("deferred/giV.glsl", GL_VERTEX_SHADER_ARB));
			gDeferredGIProgram.mShaderFiles.push_back(make_pair("deferred/giF.glsl", GL_FRAGMENT_SHADER_ARB));
			gDeferredGIProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
			success = gDeferredGIProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gDeferredGIFinalProgram.mName = "Deferred GI Final Shader";
			gDeferredGIFinalProgram.mShaderFiles.clear();
			gDeferredGIFinalProgram.mShaderFiles.push_back(make_pair("deferred/giFinalV.glsl", GL_VERTEX_SHADER_ARB));
			gDeferredGIFinalProgram.mShaderFiles.push_back(make_pair("deferred/giFinalF.glsl", GL_FRAGMENT_SHADER_ARB));
			gDeferredGIFinalProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
			success = gDeferredGIFinalProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gLuminanceGatherProgram.mName = "Luminance Gather Shader";
			gLuminanceGatherProgram.mShaderFiles.clear();
			gLuminanceGatherProgram.mShaderFiles.push_back(make_pair("deferred/luminanceV.glsl", GL_VERTEX_SHADER_ARB));
			gLuminanceGatherProgram.mShaderFiles.push_back(make_pair("deferred/luminanceF.glsl", GL_FRAGMENT_SHADER_ARB));
			gLuminanceGatherProgram.mShaderLevel = mVertexShaderLevel[SHADER_DEFERRED];
			success = gLuminanceGatherProgram.createShader(NULL, NULL);
		}
	}

	return success;
}

BOOL LLViewerShaderMgr::loadShadersObject()
{
	BOOL success = TRUE;
	
	if (mVertexShaderLevel[SHADER_OBJECT] == 0)
	{
		gObjectShinyProgram.unload();
		gObjectFullbrightShinyProgram.unload();
		gObjectFullbrightShinyWaterProgram.unload();
		gObjectShinyWaterProgram.unload();
		gObjectSimpleProgram.unload();
		gImpostorProgram.unload();
		gObjectSimpleAlphaMaskProgram.unload();
		gObjectBumpProgram.unload();
		gObjectSimpleWaterProgram.unload();
		gObjectSimpleWaterAlphaMaskProgram.unload();
		gObjectFullbrightProgram.unload();
		gObjectFullbrightAlphaMaskProgram.unload();
		gObjectFullbrightWaterProgram.unload();
		gObjectFullbrightWaterAlphaMaskProgram.unload();
		gObjectShinyNonIndexedProgram.unload();
		gObjectFullbrightShinyNonIndexedProgram.unload();
		gObjectFullbrightShinyNonIndexedWaterProgram.unload();
		gObjectShinyNonIndexedWaterProgram.unload();
		gObjectSimpleNonIndexedProgram.unload();
		gObjectSimpleNonIndexedWaterProgram.unload();
		gObjectAlphaMaskNonIndexedProgram.unload();
		gObjectAlphaMaskNonIndexedWaterProgram.unload();
		gObjectFullbrightNonIndexedProgram.unload();
		gObjectFullbrightNonIndexedWaterProgram.unload();
		gSkinnedObjectSimpleProgram.unload();
		gSkinnedObjectFullbrightProgram.unload();
		gSkinnedObjectFullbrightShinyProgram.unload();
		gSkinnedObjectShinySimpleProgram.unload();
		gSkinnedObjectSimpleWaterProgram.unload();
		gSkinnedObjectFullbrightWaterProgram.unload();
		gSkinnedObjectFullbrightShinyWaterProgram.unload();
		gSkinnedObjectShinySimpleWaterProgram.unload();
	
		return TRUE;
	}

	if (success)
	{
		gObjectSimpleNonIndexedProgram.mName = "Non indexed Shader";
		gObjectSimpleNonIndexedProgram.mFeatures.calculatesLighting = true;
		gObjectSimpleNonIndexedProgram.mFeatures.calculatesAtmospherics = true;
		gObjectSimpleNonIndexedProgram.mFeatures.hasGamma = true;
		gObjectSimpleNonIndexedProgram.mFeatures.hasAtmospherics = true;
		gObjectSimpleNonIndexedProgram.mFeatures.hasLighting = true;
		gObjectSimpleNonIndexedProgram.mFeatures.disableTextureIndex = true;
		gObjectSimpleNonIndexedProgram.mShaderFiles.clear();
		gObjectSimpleNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectSimpleNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/simpleF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectSimpleNonIndexedProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectSimpleNonIndexedProgram.createShader(NULL, NULL);
	}
	
	if (success)
	{
		gObjectSimpleNonIndexedWaterProgram.mName = "Non indexed Water Shader";
		gObjectSimpleNonIndexedWaterProgram.mFeatures.calculatesLighting = true;
		gObjectSimpleNonIndexedWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectSimpleNonIndexedWaterProgram.mFeatures.hasWaterFog = true;
		gObjectSimpleNonIndexedWaterProgram.mFeatures.hasAtmospherics = true;
		gObjectSimpleNonIndexedWaterProgram.mFeatures.hasLighting = true;
		gObjectSimpleNonIndexedWaterProgram.mFeatures.disableTextureIndex = true;
		gObjectSimpleNonIndexedWaterProgram.mShaderFiles.clear();
		gObjectSimpleNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectSimpleNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectSimpleNonIndexedWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectSimpleNonIndexedWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectSimpleNonIndexedWaterProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectAlphaMaskNonIndexedProgram.mName = "Non indexed alpha mask Shader";
		gObjectAlphaMaskNonIndexedProgram.mFeatures.calculatesLighting = true;
		gObjectAlphaMaskNonIndexedProgram.mFeatures.calculatesAtmospherics = true;
		gObjectAlphaMaskNonIndexedProgram.mFeatures.hasGamma = true;
		gObjectAlphaMaskNonIndexedProgram.mFeatures.hasAtmospherics = true;
		gObjectAlphaMaskNonIndexedProgram.mFeatures.hasLighting = true;
		gObjectAlphaMaskNonIndexedProgram.mFeatures.disableTextureIndex = true;
		gObjectAlphaMaskNonIndexedProgram.mFeatures.hasAlphaMask = true;
		gObjectAlphaMaskNonIndexedProgram.mShaderFiles.clear();
		gObjectAlphaMaskNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectAlphaMaskNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/simpleF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectAlphaMaskNonIndexedProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectAlphaMaskNonIndexedProgram.createShader(NULL, NULL);
	}
	
	if (success)
	{
		gObjectAlphaMaskNonIndexedWaterProgram.mName = "Non indexed alpha mask Water Shader";
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.calculatesLighting = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.hasWaterFog = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.hasAtmospherics = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.hasLighting = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.disableTextureIndex = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mFeatures.hasAlphaMask = true;
		gObjectAlphaMaskNonIndexedWaterProgram.mShaderFiles.clear();
		gObjectAlphaMaskNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectAlphaMaskNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectAlphaMaskNonIndexedWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectAlphaMaskNonIndexedWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectAlphaMaskNonIndexedWaterProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectFullbrightNonIndexedProgram.mName = "Non Indexed Fullbright Shader";
		gObjectFullbrightNonIndexedProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightNonIndexedProgram.mFeatures.hasGamma = true;
		gObjectFullbrightNonIndexedProgram.mFeatures.hasTransport = true;
		gObjectFullbrightNonIndexedProgram.mFeatures.isFullbright = true;
		gObjectFullbrightNonIndexedProgram.mFeatures.disableTextureIndex = true;
		gObjectFullbrightNonIndexedProgram.mShaderFiles.clear();
		gObjectFullbrightNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/fullbrightF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightNonIndexedProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectFullbrightNonIndexedProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectFullbrightNonIndexedWaterProgram.mName = "Non Indexed Fullbright Water Shader";
		gObjectFullbrightNonIndexedWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightNonIndexedWaterProgram.mFeatures.isFullbright = true;
		gObjectFullbrightNonIndexedWaterProgram.mFeatures.hasWaterFog = true;		
		gObjectFullbrightNonIndexedWaterProgram.mFeatures.hasTransport = true;
		gObjectFullbrightNonIndexedWaterProgram.mFeatures.disableTextureIndex = true;
		gObjectFullbrightNonIndexedWaterProgram.mShaderFiles.clear();
		gObjectFullbrightNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightNonIndexedWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectFullbrightNonIndexedWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectFullbrightNonIndexedWaterProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectShinyNonIndexedProgram.mName = "Non Indexed Shiny Shader";
		gObjectShinyNonIndexedProgram.mFeatures.calculatesAtmospherics = true;
		gObjectShinyNonIndexedProgram.mFeatures.calculatesLighting = true;
		gObjectShinyNonIndexedProgram.mFeatures.hasGamma = true;
		gObjectShinyNonIndexedProgram.mFeatures.hasAtmospherics = true;
		gObjectShinyNonIndexedProgram.mFeatures.isShiny = true;
		gObjectShinyNonIndexedProgram.mFeatures.disableTextureIndex = true;
		gObjectShinyNonIndexedProgram.mShaderFiles.clear();
		gObjectShinyNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/shinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectShinyNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/shinyF.glsl", GL_FRAGMENT_SHADER_ARB));		
		gObjectShinyNonIndexedProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectShinyNonIndexedProgram.createShader(NULL, &mShinyUniforms);
	}

	if (success)
	{
		gObjectShinyNonIndexedWaterProgram.mName = "Non Indexed Shiny Water Shader";
		gObjectShinyNonIndexedWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectShinyNonIndexedWaterProgram.mFeatures.calculatesLighting = true;
		gObjectShinyNonIndexedWaterProgram.mFeatures.isShiny = true;
		gObjectShinyNonIndexedWaterProgram.mFeatures.hasWaterFog = true;
		gObjectShinyNonIndexedWaterProgram.mFeatures.hasAtmospherics = true;
		gObjectShinyNonIndexedWaterProgram.mFeatures.disableTextureIndex = true;
		gObjectShinyNonIndexedWaterProgram.mShaderFiles.clear();
		gObjectShinyNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/shinyWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectShinyNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/shinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectShinyNonIndexedWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectShinyNonIndexedWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectShinyNonIndexedWaterProgram.createShader(NULL, &mShinyUniforms);
	}
	
	if (success)
	{
		gObjectFullbrightShinyNonIndexedProgram.mName = "Non Indexed Fullbright Shiny Shader";
		gObjectFullbrightShinyNonIndexedProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightShinyNonIndexedProgram.mFeatures.isFullbright = true;
		gObjectFullbrightShinyNonIndexedProgram.mFeatures.isShiny = true;
		gObjectFullbrightShinyNonIndexedProgram.mFeatures.hasGamma = true;
		gObjectFullbrightShinyNonIndexedProgram.mFeatures.hasTransport = true;
		gObjectFullbrightShinyNonIndexedProgram.mFeatures.disableTextureIndex = true;
		gObjectFullbrightShinyNonIndexedProgram.mShaderFiles.clear();
		gObjectFullbrightShinyNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightShinyNonIndexedProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightShinyNonIndexedProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectFullbrightShinyNonIndexedProgram.createShader(NULL, &mShinyUniforms);
	}

	if (success)
	{
		gObjectFullbrightShinyNonIndexedWaterProgram.mName = "Non Indexed Fullbright Shiny Water Shader";
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.isFullbright = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.isShiny = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.hasGamma = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.hasTransport = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.hasWaterFog = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mFeatures.disableTextureIndex = true;
		gObjectFullbrightShinyNonIndexedWaterProgram.mShaderFiles.clear();
		gObjectFullbrightShinyNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightShinyNonIndexedWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightShinyNonIndexedWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectFullbrightShinyNonIndexedWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectFullbrightShinyNonIndexedWaterProgram.createShader(NULL, &mShinyUniforms);
	}

	if (success)
	{
		gImpostorProgram.mName = "Impostor Shader";
		gImpostorProgram.mFeatures.disableTextureIndex = true;
		gImpostorProgram.mShaderFiles.clear();
		gImpostorProgram.mShaderFiles.push_back(make_pair("objects/impostorV.glsl", GL_VERTEX_SHADER_ARB));
		gImpostorProgram.mShaderFiles.push_back(make_pair("objects/impostorF.glsl", GL_FRAGMENT_SHADER_ARB));
		gImpostorProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gImpostorProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectSimpleProgram.mName = "Simple Shader";
		gObjectSimpleProgram.mFeatures.calculatesLighting = true;
		gObjectSimpleProgram.mFeatures.calculatesAtmospherics = true;
		gObjectSimpleProgram.mFeatures.hasGamma = true;
		gObjectSimpleProgram.mFeatures.hasAtmospherics = true;
		gObjectSimpleProgram.mFeatures.hasLighting = true;
		gObjectSimpleProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectSimpleProgram.mShaderFiles.clear();
		gObjectSimpleProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectSimpleProgram.mShaderFiles.push_back(make_pair("objects/simpleF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectSimpleProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectSimpleProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectSimpleAlphaMaskProgram.mName = "Simple Alpha Mask Shader";
		gObjectSimpleAlphaMaskProgram.mFeatures.calculatesLighting = true;
		gObjectSimpleAlphaMaskProgram.mFeatures.calculatesAtmospherics = true;
		gObjectSimpleAlphaMaskProgram.mFeatures.hasGamma = true;
		gObjectSimpleAlphaMaskProgram.mFeatures.hasAtmospherics = true;
		gObjectSimpleAlphaMaskProgram.mFeatures.hasLighting = true;
		gObjectSimpleAlphaMaskProgram.mFeatures.hasAlphaMask = true;
		gObjectSimpleAlphaMaskProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectSimpleAlphaMaskProgram.mShaderFiles.clear();
		gObjectSimpleAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectSimpleAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/simpleF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectSimpleAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectSimpleAlphaMaskProgram.createShader(NULL, NULL);
	}
	
	if (success)
	{
		gObjectBumpProgram.mName = "Bump Shader";
		/*gObjectBumpProgram.mFeatures.calculatesLighting = true;
		gObjectBumpProgram.mFeatures.calculatesAtmospherics = true;
		gObjectBumpProgram.mFeatures.hasGamma = true;
		gObjectBumpProgram.mFeatures.hasAtmospherics = true;
		gObjectBumpProgram.mFeatures.hasLighting = true;
		gObjectBumpProgram.mFeatures.mIndexedTextureChannels = 0;*/
		gObjectBumpProgram.mShaderFiles.clear();
		gObjectBumpProgram.mShaderFiles.push_back(make_pair("objects/bumpV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectBumpProgram.mShaderFiles.push_back(make_pair("objects/bumpF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectBumpProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectBumpProgram.createShader(NULL, NULL);
	}
	
	if (success)
	{
		gObjectSimpleWaterProgram.mName = "Simple Water Shader";
		gObjectSimpleWaterProgram.mFeatures.calculatesLighting = true;
		gObjectSimpleWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectSimpleWaterProgram.mFeatures.hasWaterFog = true;
		gObjectSimpleWaterProgram.mFeatures.hasAtmospherics = true;
		gObjectSimpleWaterProgram.mFeatures.hasLighting = true;
		gObjectSimpleWaterProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectSimpleWaterProgram.mShaderFiles.clear();
		gObjectSimpleWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectSimpleWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectSimpleWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectSimpleWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectSimpleWaterProgram.createShader(NULL, NULL);
	}
	
	if (success)
	{
		gObjectSimpleWaterAlphaMaskProgram.mName = "Simple Water Alpha Mask Shader";
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.calculatesLighting = true;
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.calculatesAtmospherics = true;
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.hasWaterFog = true;
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.hasAtmospherics = true;
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.hasLighting = true;
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.hasAlphaMask = true;
		gObjectSimpleWaterAlphaMaskProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectSimpleWaterAlphaMaskProgram.mShaderFiles.clear();
		gObjectSimpleWaterAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/simpleV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectSimpleWaterAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/simpleWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectSimpleWaterAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectSimpleWaterAlphaMaskProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectSimpleWaterAlphaMaskProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectFullbrightProgram.mName = "Fullbright Shader";
		gObjectFullbrightProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightProgram.mFeatures.hasGamma = true;
		gObjectFullbrightProgram.mFeatures.hasTransport = true;
		gObjectFullbrightProgram.mFeatures.isFullbright = true;
		gObjectFullbrightProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectFullbrightProgram.mShaderFiles.clear();
		gObjectFullbrightProgram.mShaderFiles.push_back(make_pair("objects/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightProgram.mShaderFiles.push_back(make_pair("objects/fullbrightF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectFullbrightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectFullbrightWaterProgram.mName = "Fullbright Water Shader";
		gObjectFullbrightWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightWaterProgram.mFeatures.isFullbright = true;
		gObjectFullbrightWaterProgram.mFeatures.hasWaterFog = true;		
		gObjectFullbrightWaterProgram.mFeatures.hasTransport = true;
		gObjectFullbrightWaterProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectFullbrightWaterProgram.mShaderFiles.clear();
		gObjectFullbrightWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectFullbrightWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectFullbrightWaterProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectFullbrightAlphaMaskProgram.mName = "Fullbright Alpha Mask Shader";
		gObjectFullbrightAlphaMaskProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightAlphaMaskProgram.mFeatures.hasGamma = true;
		gObjectFullbrightAlphaMaskProgram.mFeatures.hasTransport = true;
		gObjectFullbrightAlphaMaskProgram.mFeatures.isFullbright = true;
		gObjectFullbrightAlphaMaskProgram.mFeatures.hasAlphaMask = true;
		gObjectFullbrightAlphaMaskProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectFullbrightAlphaMaskProgram.mShaderFiles.clear();
		gObjectFullbrightAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/fullbrightF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectFullbrightAlphaMaskProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectFullbrightWaterAlphaMaskProgram.mName = "Fullbright Water Shader";
		gObjectFullbrightWaterAlphaMaskProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightWaterAlphaMaskProgram.mFeatures.isFullbright = true;
		gObjectFullbrightWaterAlphaMaskProgram.mFeatures.hasWaterFog = true;		
		gObjectFullbrightWaterAlphaMaskProgram.mFeatures.hasTransport = true;
		gObjectFullbrightWaterAlphaMaskProgram.mFeatures.hasAlphaMask = true;
		gObjectFullbrightWaterAlphaMaskProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectFullbrightWaterAlphaMaskProgram.mShaderFiles.clear();
		gObjectFullbrightWaterAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/fullbrightV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightWaterAlphaMaskProgram.mShaderFiles.push_back(make_pair("objects/fullbrightWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightWaterAlphaMaskProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectFullbrightWaterAlphaMaskProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectFullbrightWaterAlphaMaskProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gObjectShinyProgram.mName = "Shiny Shader";
		gObjectShinyProgram.mFeatures.calculatesAtmospherics = true;
		gObjectShinyProgram.mFeatures.calculatesLighting = true;
		gObjectShinyProgram.mFeatures.hasGamma = true;
		gObjectShinyProgram.mFeatures.hasAtmospherics = true;
		gObjectShinyProgram.mFeatures.isShiny = true;
		gObjectShinyProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectShinyProgram.mShaderFiles.clear();
		gObjectShinyProgram.mShaderFiles.push_back(make_pair("objects/shinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectShinyProgram.mShaderFiles.push_back(make_pair("objects/shinyF.glsl", GL_FRAGMENT_SHADER_ARB));		
		gObjectShinyProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectShinyProgram.createShader(NULL, &mShinyUniforms);
	}

	if (success)
	{
		gObjectShinyWaterProgram.mName = "Shiny Water Shader";
		gObjectShinyWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectShinyWaterProgram.mFeatures.calculatesLighting = true;
		gObjectShinyWaterProgram.mFeatures.isShiny = true;
		gObjectShinyWaterProgram.mFeatures.hasWaterFog = true;
		gObjectShinyWaterProgram.mFeatures.hasAtmospherics = true;
		gObjectShinyWaterProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectShinyWaterProgram.mShaderFiles.clear();
		gObjectShinyWaterProgram.mShaderFiles.push_back(make_pair("objects/shinyWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectShinyWaterProgram.mShaderFiles.push_back(make_pair("objects/shinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectShinyWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectShinyWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectShinyWaterProgram.createShader(NULL, &mShinyUniforms);
	}
	
	if (success)
	{
		gObjectFullbrightShinyProgram.mName = "Fullbright Shiny Shader";
		gObjectFullbrightShinyProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightShinyProgram.mFeatures.isFullbright = true;
		gObjectFullbrightShinyProgram.mFeatures.isShiny = true;
		gObjectFullbrightShinyProgram.mFeatures.hasGamma = true;
		gObjectFullbrightShinyProgram.mFeatures.hasTransport = true;
		gObjectFullbrightShinyProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectFullbrightShinyProgram.mShaderFiles.clear();
		gObjectFullbrightShinyProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightShinyProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightShinyProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		success = gObjectFullbrightShinyProgram.createShader(NULL, &mShinyUniforms);
	}

	if (success)
	{
		gObjectFullbrightShinyWaterProgram.mName = "Fullbright Shiny Water Shader";
		gObjectFullbrightShinyWaterProgram.mFeatures.calculatesAtmospherics = true;
		gObjectFullbrightShinyWaterProgram.mFeatures.isFullbright = true;
		gObjectFullbrightShinyWaterProgram.mFeatures.isShiny = true;
		gObjectFullbrightShinyWaterProgram.mFeatures.hasGamma = true;
		gObjectFullbrightShinyWaterProgram.mFeatures.hasTransport = true;
		gObjectFullbrightShinyWaterProgram.mFeatures.hasWaterFog = true;
		gObjectFullbrightShinyWaterProgram.mFeatures.mIndexedTextureChannels = 0;
		gObjectFullbrightShinyWaterProgram.mShaderFiles.clear();
		gObjectFullbrightShinyWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyV.glsl", GL_VERTEX_SHADER_ARB));
		gObjectFullbrightShinyWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
		gObjectFullbrightShinyWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
		gObjectFullbrightShinyWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
		success = gObjectFullbrightShinyWaterProgram.createShader(NULL, &mShinyUniforms);
	}

	if (mVertexShaderLevel[SHADER_AVATAR] > 0)
	{ //load hardware skinned attachment shaders
		if (success)
		{
			gSkinnedObjectSimpleProgram.mName = "Skinned Simple Shader";
			gSkinnedObjectSimpleProgram.mFeatures.calculatesLighting = true;
			gSkinnedObjectSimpleProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectSimpleProgram.mFeatures.hasGamma = true;
			gSkinnedObjectSimpleProgram.mFeatures.hasAtmospherics = true;
			gSkinnedObjectSimpleProgram.mFeatures.hasLighting = true;
			gSkinnedObjectSimpleProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectSimpleProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectSimpleProgram.mShaderFiles.clear();
			gSkinnedObjectSimpleProgram.mShaderFiles.push_back(make_pair("objects/simpleSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectSimpleProgram.mShaderFiles.push_back(make_pair("objects/simpleF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectSimpleProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectSimpleProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gSkinnedObjectFullbrightProgram.mName = "Skinned Fullbright Shader";
			gSkinnedObjectFullbrightProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectFullbrightProgram.mFeatures.hasGamma = true;
			gSkinnedObjectFullbrightProgram.mFeatures.hasTransport = true;
			gSkinnedObjectFullbrightProgram.mFeatures.isFullbright = true;
			gSkinnedObjectFullbrightProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectFullbrightProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectFullbrightProgram.mShaderFiles.clear();
			gSkinnedObjectFullbrightProgram.mShaderFiles.push_back(make_pair("objects/fullbrightSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectFullbrightProgram.mShaderFiles.push_back(make_pair("objects/fullbrightF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectFullbrightProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectFullbrightProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gSkinnedObjectFullbrightShinyProgram.mName = "Skinned Fullbright Shiny Shader";
			gSkinnedObjectFullbrightShinyProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectFullbrightShinyProgram.mFeatures.hasGamma = true;
			gSkinnedObjectFullbrightShinyProgram.mFeatures.hasTransport = true;
			gSkinnedObjectFullbrightShinyProgram.mFeatures.isShiny = true;
			gSkinnedObjectFullbrightShinyProgram.mFeatures.isFullbright = true;
			gSkinnedObjectFullbrightShinyProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectFullbrightShinyProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectFullbrightShinyProgram.mShaderFiles.clear();
			gSkinnedObjectFullbrightShinyProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinySkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectFullbrightShinyProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectFullbrightShinyProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectFullbrightShinyProgram.createShader(NULL, &mShinyUniforms);
		}

		if (success)
		{
			gSkinnedObjectShinySimpleProgram.mName = "Skinned Shiny Simple Shader";
			gSkinnedObjectShinySimpleProgram.mFeatures.calculatesLighting = true;
			gSkinnedObjectShinySimpleProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectShinySimpleProgram.mFeatures.hasGamma = true;
			gSkinnedObjectShinySimpleProgram.mFeatures.hasAtmospherics = true;
			gSkinnedObjectShinySimpleProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectShinySimpleProgram.mFeatures.isShiny = true;
			gSkinnedObjectShinySimpleProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectShinySimpleProgram.mShaderFiles.clear();
			gSkinnedObjectShinySimpleProgram.mShaderFiles.push_back(make_pair("objects/shinySimpleSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectShinySimpleProgram.mShaderFiles.push_back(make_pair("objects/shinyF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectShinySimpleProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectShinySimpleProgram.createShader(NULL, &mShinyUniforms);
		}

		if (success)
		{
			gSkinnedObjectSimpleWaterProgram.mName = "Skinned Simple Water Shader";
			gSkinnedObjectSimpleWaterProgram.mFeatures.calculatesLighting = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.hasGamma = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.hasAtmospherics = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.hasLighting = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.hasWaterFog = true;
			gSkinnedObjectSimpleWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
			gSkinnedObjectSimpleWaterProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectSimpleWaterProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectSimpleWaterProgram.mShaderFiles.clear();
			gSkinnedObjectSimpleWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectSimpleWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectSimpleWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectSimpleWaterProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gSkinnedObjectFullbrightWaterProgram.mName = "Skinned Fullbright Water Shader";
			gSkinnedObjectFullbrightWaterProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectFullbrightWaterProgram.mFeatures.hasGamma = true;
			gSkinnedObjectFullbrightWaterProgram.mFeatures.hasTransport = true;
			gSkinnedObjectFullbrightWaterProgram.mFeatures.isFullbright = true;
			gSkinnedObjectFullbrightWaterProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectFullbrightWaterProgram.mFeatures.hasWaterFog = true;
			gSkinnedObjectFullbrightWaterProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectFullbrightWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
			gSkinnedObjectFullbrightWaterProgram.mShaderFiles.clear();
			gSkinnedObjectFullbrightWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectFullbrightWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectFullbrightWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectFullbrightWaterProgram.createShader(NULL, NULL);
		}

		if (success)
		{
			gSkinnedObjectFullbrightShinyWaterProgram.mName = "Skinned Fullbright Shiny Water Shader";
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.hasGamma = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.hasTransport = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.isShiny = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.isFullbright = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.hasWaterFog = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectFullbrightShinyWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
			gSkinnedObjectFullbrightShinyWaterProgram.mShaderFiles.clear();
			gSkinnedObjectFullbrightShinyWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinySkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectFullbrightShinyWaterProgram.mShaderFiles.push_back(make_pair("objects/fullbrightShinyWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectFullbrightShinyWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectFullbrightShinyWaterProgram.createShader(NULL, &mShinyUniforms);
		}

		if (success)
		{
			gSkinnedObjectShinySimpleWaterProgram.mName = "Skinned Shiny Simple Water Shader";
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.calculatesLighting = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.calculatesAtmospherics = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.hasGamma = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.hasAtmospherics = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.hasObjectSkinning = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.isShiny = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.hasWaterFog = true;
			gSkinnedObjectShinySimpleWaterProgram.mFeatures.disableTextureIndex = true;
			gSkinnedObjectShinySimpleWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;
			gSkinnedObjectShinySimpleWaterProgram.mShaderFiles.clear();
			gSkinnedObjectShinySimpleWaterProgram.mShaderFiles.push_back(make_pair("objects/shinySimpleSkinnedV.glsl", GL_VERTEX_SHADER_ARB));
			gSkinnedObjectShinySimpleWaterProgram.mShaderFiles.push_back(make_pair("objects/shinyWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
			gSkinnedObjectShinySimpleWaterProgram.mShaderLevel = mVertexShaderLevel[SHADER_OBJECT];
			success = gSkinnedObjectShinySimpleWaterProgram.createShader(NULL, &mShinyUniforms);
		}
	}

	if( !success )
	{
		mVertexShaderLevel[SHADER_OBJECT] = 0;
		return FALSE;
	}
	
	return TRUE;
}

BOOL LLViewerShaderMgr::loadShadersAvatar()
{
	BOOL success = TRUE;

	if (mVertexShaderLevel[SHADER_AVATAR] == 0)
	{
		gAvatarProgram.unload();
		gAvatarWaterProgram.unload();
		gAvatarEyeballProgram.unload();
		gAvatarPickProgram.unload();
		return FALSE;
	}

	if (success)
	{
		gAvatarProgram.mName = "Avatar Shader";
		gAvatarProgram.mFeatures.hasSkinning = true;
		gAvatarProgram.mFeatures.calculatesAtmospherics = true;
		gAvatarProgram.mFeatures.calculatesLighting = true;
		gAvatarProgram.mFeatures.hasGamma = true;
		gAvatarProgram.mFeatures.hasAtmospherics = true;
		gAvatarProgram.mFeatures.hasLighting = true;
		gAvatarProgram.mFeatures.hasAlphaMask = true;
		gAvatarProgram.mFeatures.disableTextureIndex = true;
		gAvatarProgram.mShaderFiles.clear();
		gAvatarProgram.mShaderFiles.push_back(make_pair("avatar/avatarV.glsl", GL_VERTEX_SHADER_ARB));
		gAvatarProgram.mShaderFiles.push_back(make_pair("avatar/avatarF.glsl", GL_FRAGMENT_SHADER_ARB));
		gAvatarProgram.mShaderLevel = mVertexShaderLevel[SHADER_AVATAR];
		success = gAvatarProgram.createShader(&mAvatarAttribs, &mAvatarUniforms);
			
		if (success)
		{
			gAvatarWaterProgram.mName = "Avatar Water Shader";
			gAvatarWaterProgram.mFeatures.hasSkinning = true;
			gAvatarWaterProgram.mFeatures.calculatesAtmospherics = true;
			gAvatarWaterProgram.mFeatures.calculatesLighting = true;
			gAvatarWaterProgram.mFeatures.hasWaterFog = true;
			gAvatarWaterProgram.mFeatures.hasAtmospherics = true;
			gAvatarWaterProgram.mFeatures.hasLighting = true;
			gAvatarWaterProgram.mFeatures.hasAlphaMask = true;
			gAvatarWaterProgram.mFeatures.disableTextureIndex = true;
			gAvatarWaterProgram.mShaderFiles.clear();
			gAvatarWaterProgram.mShaderFiles.push_back(make_pair("avatar/avatarV.glsl", GL_VERTEX_SHADER_ARB));
			gAvatarWaterProgram.mShaderFiles.push_back(make_pair("objects/simpleWaterF.glsl", GL_FRAGMENT_SHADER_ARB));
			// Note: no cloth under water:
			gAvatarWaterProgram.mShaderLevel = llmin(mVertexShaderLevel[SHADER_AVATAR], 1);	
			gAvatarWaterProgram.mShaderGroup = LLGLSLShader::SG_WATER;				
			success = gAvatarWaterProgram.createShader(&mAvatarAttribs, &mAvatarUniforms);
		}

		/// Keep track of avatar levels
		if (gAvatarProgram.mShaderLevel != mVertexShaderLevel[SHADER_AVATAR])
		{
			mMaxAvatarShaderLevel = mVertexShaderLevel[SHADER_AVATAR] = gAvatarProgram.mShaderLevel;
		}
	}

	if (success)
	{
		gAvatarPickProgram.mName = "Avatar Pick Shader";
		gAvatarPickProgram.mFeatures.hasSkinning = true;
		gAvatarPickProgram.mFeatures.disableTextureIndex = true;
		gAvatarPickProgram.mShaderFiles.clear();
		gAvatarPickProgram.mShaderFiles.push_back(make_pair("avatar/pickAvatarV.glsl", GL_VERTEX_SHADER_ARB));
		gAvatarPickProgram.mShaderFiles.push_back(make_pair("avatar/pickAvatarF.glsl", GL_FRAGMENT_SHADER_ARB));
		gAvatarPickProgram.mShaderLevel = mVertexShaderLevel[SHADER_AVATAR];
		success = gAvatarPickProgram.createShader(&mAvatarAttribs, &mAvatarUniforms);
	}

	if (success)
	{
		gAvatarEyeballProgram.mName = "Avatar Eyeball Program";
		gAvatarEyeballProgram.mFeatures.calculatesLighting = true;
		gAvatarEyeballProgram.mFeatures.isSpecular = true;
		gAvatarEyeballProgram.mFeatures.calculatesAtmospherics = true;
		gAvatarEyeballProgram.mFeatures.hasGamma = true;
		gAvatarEyeballProgram.mFeatures.hasAtmospherics = true;
		gAvatarEyeballProgram.mFeatures.hasLighting = true;
		gAvatarEyeballProgram.mFeatures.hasAlphaMask = true;
		gAvatarEyeballProgram.mFeatures.disableTextureIndex = true;
		gAvatarEyeballProgram.mShaderFiles.clear();
		gAvatarEyeballProgram.mShaderFiles.push_back(make_pair("avatar/eyeballV.glsl", GL_VERTEX_SHADER_ARB));
		gAvatarEyeballProgram.mShaderFiles.push_back(make_pair("avatar/eyeballF.glsl", GL_FRAGMENT_SHADER_ARB));
		gAvatarEyeballProgram.mShaderLevel = mVertexShaderLevel[SHADER_AVATAR];
		success = gAvatarEyeballProgram.createShader(NULL, NULL);
	}

	if( !success )
	{
		mVertexShaderLevel[SHADER_AVATAR] = 0;
		mMaxAvatarShaderLevel = 0;
		return FALSE;
	}
	
	return TRUE;
}

BOOL LLViewerShaderMgr::loadShadersInterface()
{
	BOOL success = TRUE;

	if (mVertexShaderLevel[SHADER_INTERFACE] == 0)
	{
		gHighlightProgram.unload();
		return FALSE;
	}
	
	if (success)
	{
		gHighlightProgram.mName = "Highlight Shader";
		gHighlightProgram.mShaderFiles.clear();
		gHighlightProgram.mShaderFiles.push_back(make_pair("interface/highlightV.glsl", GL_VERTEX_SHADER_ARB));
		gHighlightProgram.mShaderFiles.push_back(make_pair("interface/highlightF.glsl", GL_FRAGMENT_SHADER_ARB));
		gHighlightProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];		
		success = gHighlightProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gUIProgram.mName = "UI Shader";
		gUIProgram.mShaderFiles.clear();
		gUIProgram.mShaderFiles.push_back(make_pair("interface/uiV.glsl", GL_VERTEX_SHADER_ARB));
		gUIProgram.mShaderFiles.push_back(make_pair("interface/uiF.glsl", GL_FRAGMENT_SHADER_ARB));
		gUIProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];
		success = gUIProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gCustomAlphaProgram.mName = "Custom Alpha Shader";
		gCustomAlphaProgram.mShaderFiles.clear();
		gCustomAlphaProgram.mShaderFiles.push_back(make_pair("interface/customalphaV.glsl", GL_VERTEX_SHADER_ARB));
		gCustomAlphaProgram.mShaderFiles.push_back(make_pair("interface/customalphaF.glsl", GL_FRAGMENT_SHADER_ARB));
		gCustomAlphaProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];
		success = gCustomAlphaProgram.createShader(NULL, NULL);
	}

	if (success)
	{
		gGlowCombineProgram.mName = "Glow Combine Shader";
		gGlowCombineProgram.mShaderFiles.clear();
		gGlowCombineProgram.mShaderFiles.push_back(make_pair("interface/glowcombineV.glsl", GL_VERTEX_SHADER_ARB));
		gGlowCombineProgram.mShaderFiles.push_back(make_pair("interface/glowcombineF.glsl", GL_FRAGMENT_SHADER_ARB));
		gGlowCombineProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];
		success = gGlowCombineProgram.createShader(NULL, NULL);
		if (success)
		{
			gGlowCombineProgram.bind();
			gGlowCombineProgram.uniform1i("glowMap", 0);
			gGlowCombineProgram.uniform1i("screenMap", 1);
			gGlowCombineProgram.unbind();
		}
	}

	if (success)
	{
		gTwoTextureAddProgram.mName = "Two Texture Add Shader";
		gTwoTextureAddProgram.mShaderFiles.clear();
		gTwoTextureAddProgram.mShaderFiles.push_back(make_pair("interface/twotextureaddV.glsl", GL_VERTEX_SHADER_ARB));
		gTwoTextureAddProgram.mShaderFiles.push_back(make_pair("interface/twotextureaddF.glsl", GL_FRAGMENT_SHADER_ARB));
		gTwoTextureAddProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];
		success = gTwoTextureAddProgram.createShader(NULL, NULL);
		if (success)
		{
			gTwoTextureAddProgram.bind();
			gTwoTextureAddProgram.uniform1i("tex0", 0);
			gTwoTextureAddProgram.uniform1i("tex1", 1);
		}
	}

	if (success)
	{
		gSolidColorProgram.mName = "Solid Color Shader";
		gSolidColorProgram.mShaderFiles.clear();
		gSolidColorProgram.mShaderFiles.push_back(make_pair("interface/solidcolorV.glsl", GL_VERTEX_SHADER_ARB));
		gSolidColorProgram.mShaderFiles.push_back(make_pair("interface/solidcolorF.glsl", GL_FRAGMENT_SHADER_ARB));
		gSolidColorProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];
		success = gSolidColorProgram.createShader(NULL, NULL);
		if (success)
		{
			gSolidColorProgram.bind();
			gSolidColorProgram.uniform1i("tex0", 0);
			gSolidColorProgram.unbind();
		}
	}

	if (success)
	{
		gOcclusionProgram.mName = "Occlusion Shader";
		gOcclusionProgram.mShaderFiles.clear();
		gOcclusionProgram.mShaderFiles.push_back(make_pair("interface/occlusionV.glsl", GL_VERTEX_SHADER_ARB));
		gOcclusionProgram.mShaderFiles.push_back(make_pair("interface/occlusionF.glsl", GL_FRAGMENT_SHADER_ARB));
		gOcclusionProgram.mShaderLevel = mVertexShaderLevel[SHADER_INTERFACE];
		success = gOcclusionProgram.createShader(NULL, NULL);
	}

	if( !success )
	{
		mVertexShaderLevel[SHADER_INTERFACE] = 0;
		return FALSE;
	}
	
	return TRUE;
}

BOOL LLViewerShaderMgr::loadShadersWindLight()
{	
	BOOL success = TRUE;

	if (mVertexShaderLevel[SHADER_WINDLIGHT] < 2)
	{
		gWLSkyProgram.unload();
		gWLCloudProgram.unload();
		return FALSE;
	}

	if (success)
	{
		gWLSkyProgram.mName = "Windlight Sky Shader";
		//gWLSkyProgram.mFeatures.hasGamma = true;
		gWLSkyProgram.mShaderFiles.clear();
		gWLSkyProgram.mShaderFiles.push_back(make_pair("windlight/skyV.glsl", GL_VERTEX_SHADER_ARB));
		gWLSkyProgram.mShaderFiles.push_back(make_pair("windlight/skyF.glsl", GL_FRAGMENT_SHADER_ARB));
		gWLSkyProgram.mShaderLevel = mVertexShaderLevel[SHADER_WINDLIGHT];
		gWLSkyProgram.mShaderGroup = LLGLSLShader::SG_SKY;
		success = gWLSkyProgram.createShader(NULL, &mWLUniforms);
	}

	if (success)
	{
		gWLCloudProgram.mName = "Windlight Cloud Program";
		//gWLCloudProgram.mFeatures.hasGamma = true;
		gWLCloudProgram.mShaderFiles.clear();
		gWLCloudProgram.mShaderFiles.push_back(make_pair("windlight/cloudsV.glsl", GL_VERTEX_SHADER_ARB));
		gWLCloudProgram.mShaderFiles.push_back(make_pair("windlight/cloudsF.glsl", GL_FRAGMENT_SHADER_ARB));
		gWLCloudProgram.mShaderLevel = mVertexShaderLevel[SHADER_WINDLIGHT];
		gWLCloudProgram.mShaderGroup = LLGLSLShader::SG_SKY;
		success = gWLCloudProgram.createShader(NULL, &mWLUniforms);
	}

	return success;
}

std::string LLViewerShaderMgr::getShaderDirPrefix(void)
{
	return gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "shaders/class");
}

void LLViewerShaderMgr::updateShaderUniforms(LLGLSLShader * shader)
{
	LLWLParamManager::getInstance()->updateShaderUniforms(shader);
	LLWaterParamManager::getInstance()->updateShaderUniforms(shader);
}

LLViewerShaderMgr::shader_iter LLViewerShaderMgr::beginShaders() const
{
	return mShaderList.begin();
}

LLViewerShaderMgr::shader_iter LLViewerShaderMgr::endShaders() const
{
	return mShaderList.end();
}
