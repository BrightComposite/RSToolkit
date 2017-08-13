//---------------------------------------------------------------------------

#pragma once

#ifndef WIDGET_LAYERS_H
#define WIDGET_LAYERS_H

//---------------------------------------------------------------------------

#include <ui/widget.h>
#include <graphics/Color.h>

//---------------------------------------------------------------------------

namespace asd
{
	class BackgroundDrawer : public WidgetLayer
	{
	public:
		BackgroundDrawer() : WidgetLayer(0) {}
	};

	class BoundaryDrawer : public WidgetLayer
	{
	public:
		BoundaryDrawer() : WidgetLayer(1) {}
	};

	using BackgroundDrawerComponent = WidgetLayerComponent<BackgroundDrawer>;
	using BoundaryDrawerComponent = WidgetLayerComponent<BoundaryDrawer>;

	create_layer_component(ui, BackgroundDrawer);
	create_layer_component(ui, BoundaryDrawer);

	class ColoredBackgroundDrawer : public BackgroundDrawer
	{
	public:
		api(ui) ColoredBackgroundDrawer(const array_list<unique<Color>> & colors);

		virtual ~ColoredBackgroundDrawer() {}

		virtual api(ui) handle<WidgetLayer> clone(widget * w) const override;

	protected:
		virtual api(ui) void draw(widget * w) override;

		array_list<unique<Color>> _colors;
	};
	
	class ColoredBoundaryDrawer : public BoundaryDrawer
	{
	public:
		api(ui) ColoredBoundaryDrawer(const Color & color, uint width);

		virtual ~ColoredBoundaryDrawer() {}

		virtual api(ui) handle<WidgetLayer> clone(widget * widget) const override;

	protected:
		virtual api(ui) void draw(widget * w) override;

		Color _color;
		uint _width;
	};
	
	class ColoredButtonDecorator
	{
	public:
		api(ui) ColoredButtonDecorator();

		api(ui) const Color & background() const;
		api(ui) const Color & pressed() const;
		api(ui) const Color & hovered() const;

		api(ui) ColoredButtonDecorator & background(const Color & color);
		api(ui) ColoredButtonDecorator & pressed(const Color & color);
		api(ui) ColoredButtonDecorator & hovered(const Color & color);

		api(ui) ColoredButtonDecorator & background(nullptr_t);
		api(ui) ColoredButtonDecorator & pressed(nullptr_t);
		api(ui) ColoredButtonDecorator & hovered(nullptr_t);

		api(ui) const Color & boundary_color() const;
		api(ui) uint boundary_width() const;
		api(ui) const Color & boundary() const;

		api(ui) ColoredButtonDecorator & boundary(const Color & color);
		api(ui) ColoredButtonDecorator & boundary(uint width);
		api(ui) ColoredButtonDecorator & boundary(const Color & color, uint width);
		api(ui) ColoredButtonDecorator & boundary(nullptr_t);

		api(ui) void reset();

		api(ui) void apply(widget * w);

	protected:
		array_list<unique<Color>> _colors;

		struct {
			unique<Color> color;
			uint width;
		} _boundary;
	};
}

//---------------------------------------------------------------------------
#endif
