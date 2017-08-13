//---------------------------------------------------------------------------

#include <scene/DeferredShadingScene.h>

//---------------------------------------------------------------------------

namespace asd
{
	void Light::apply(Graphics3D & graphics) const
	{
		graphics.updateUniform<Uniforms::PointLight>(*position, color, float4 {attenuation.constant, attenuation.linear, attenuation.exponential, ambientFactor});
	}

	DeferredShadingScene::DeferredShadingScene(Widget * widget) : Scene(widget)
	{
		_surface = graphics().createSurface(widget->size());

		_surface->addBuffer(_positions, ImageFormat::rgb16f);
		_surface->addBuffer(_normals, ImageFormat::rgba16f);
		_surface->addBuffer(_colors, ImageFormat::rgba);

		_lightTech.global.init(graphics().getShaderProgram("3d/deferred/light/global"));
		_lightTech.point.init(graphics().getShaderProgram("3d/deferred/light/point"));
	}

	void DeferredShadingScene::onWidgetResize(handle<WidgetResizeMessage> & msg, Widget & w)
	{
		Scene::onWidgetResize(msg, w);
		_surface->setSize(widget().size());
	}

	handle<Light> DeferredShadingScene::addLight()
	{
		return *_lights.emplace(_lights.end(), default_init);
	}

	void DeferredShadingScene::setGlobalLightColor(const colorf & color)
	{
		_globalLightColor = color;
	}

	void DeferredShadingScene::draw(Graphics3D & g, const IntRect & viewport) const
	{
		auto dt = hold(g.depthTestState(), true);
		auto clearColor = g.clearColor();
		g.updateUniformBuffers();

		g.bind(_surface);

		g.setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		_surface->clear();	// clear positions, normals and depth
		g.setClearColor(clearColor);
		_surface->clear(bitmask<2>::value);	// clear colors

		for(auto & drawable : _opaque)
		{
			drawable->draw(g, viewport, 1.0f);
		}

		g.bind(space().surface());

		g.setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		space().surface()->clear();
		g.setClearColor(clearColor);

		auto dt2 = hold(g.depthTestState(), false);

		g.bind(_positions, 0);
		g.bind(_normals, 1);
		g.bind(_colors, 2);

		g.setArea(_widget->region());

		_lightTech.global->apply(0);

		g.updateUniform<Uniforms::GlobalLight>(_globalLightColor);
		g.meshes2d.quad->draw();

		auto am = hold(g.accumulationState(), true);

		_lightTech.point->apply(0);

		for(auto & light : _lights)
		{
			light->apply(g);
			g.meshes2d.quad->draw();
		}
	}
}

//---------------------------------------------------------------------------
