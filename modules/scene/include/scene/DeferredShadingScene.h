//---------------------------------------------------------------------------

#pragma once

#ifndef DEFERRED_SHADING_SCENE_H
#define DEFERRED_SHADING_SCENE_H

//---------------------------------------------------------------------------

#include "Scene.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	struct Light : Shared
	{
		fvec position = {0.0f, 0.0f, 0.0f};
		colorf color = {1.0f, 1.0f, 1.0f};
		
		struct Attenuation
		{
			float constant;
			float linear;
			float exponential;
		} attenuation = {0.0f, 0.0f, 0.0f};

		float ambientFactor = {1.0f};

		api(scene) void apply(Graphics3D & graphics) const;
	};

	class DeferredShadingScene : public Scene
	{
	public:
		api(scene) DeferredShadingScene(Widget * widget, const string & name = "unknown scene");
		virtual ~DeferredShadingScene() {}

		api(scene) Handle<Light> addLight();
		api(scene) void setGlobalLightColor(const colorf & color);

	protected:
		virtual api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const override;

		Handle<TextureSurface> _surface = nullptr;
		Handle<Texture> _positions = nullptr;
		Handle<Texture> _normals = nullptr;
		Handle<Texture> _colors = nullptr;

		ArrayList<Light> _lights;
		colorf _globalLightColor = {0.0f, 0.0f, 0.0f};

		struct
		{
			Handle<FxTechnique> global = nullptr;
			Handle<FxTechnique> point = nullptr;
		} _lightTech;
	};
}

//---------------------------------------------------------------------------
#endif
