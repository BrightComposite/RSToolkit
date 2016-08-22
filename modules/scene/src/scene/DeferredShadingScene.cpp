//---------------------------------------------------------------------------

#include <scene/DeferredShadingScene.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	void Light::apply(Graphics3D & graphics) const
	{
		graphics.updateUniform<Uniforms::PointLight>(*position, color, float4 {attenuation.constant, attenuation.linear, attenuation.exponential, ambientFactor});
	}

	DeferredShadingScene::DeferredShadingScene(Widget * widget, const string & name) : Scene(widget, name)
	{
		_surface = graphics().createSurface(widget->size());

		_surface->addBuffer(_positions, ImageFormat::rgb16f);
		_surface->addBuffer(_normals, ImageFormat::rgb16f);
		_surface->addBuffer(_colors, ImageFormat::rgba);

		_lighttech.init(graphics().getShaderProgram("3d/deferred/light/point"));
	}

	Handle<Light> DeferredShadingScene::addLight()
	{
		return *_lights.emplace(_lights.end(), default_init);
	}

	void DeferredShadingScene::draw(Graphics3D & g, const IntRect & viewport) const
	{
		g.updateUniformBuffers();
		g.bind(_surface);
		_surface->clear();

		for(auto & drawable : _opaque)
		{
			drawable->draw(g, viewport, 1.0f);
		}
		
		g.bind(space().surface());
		_lighttech->apply(0);

		g.bind(_positions, 0);
		g.bind(_normals, 1);
		g.bind(_colors, 2);

		auto am = hold(g.accumulationState(), true);

		for(auto & light : _lights)
		{
			light->apply(g);
			g.setArea(_widget->region());
			g.meshes2d.quad->draw();
		}
	}
}

//---------------------------------------------------------------------------
