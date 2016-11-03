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
		friend class Scene;

		using OwnHandle = Handle<DeferredShadingScene, DeferredShadingScene>;

	public:
		virtual ~DeferredShadingScene() {}

		virtual Handle<Scene> clone(Widget * w) const override
		{
			return OwnHandle(w);
		}

		api(scene) Handle<Light> addLight();
		api(scene) void setGlobalLightColor(const colorf & color);

		static Handle<DeferredShadingScene> create(Widget * widget)
		{
			OwnHandle h(widget);
			Scene::construct(h);

			return h;
		}

	protected:
		friend_owned_handle(DeferredShadingScene, DeferredShadingScene);

		api(scene) DeferredShadingScene(Widget * widget);

		virtual api(scene) void draw(Graphics3D & graphics, const IntRect & viewport) const override;
		virtual api(scene) void onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & w) override;

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
