//---------------------------------------------------------------------------

#include <ui/widget_layers.h>
#include <graphics/graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	ColoredBackgroundDrawer::ColoredBackgroundDrawer(const array_list<unique<Color>> & colors) : BackgroundDrawer()
	{
		repeat(i, 3) if(colors[i] != nullptr) _colors.emplace_back(*colors[i]); else _colors.emplace_back(nullptr);
	}

	handle<WidgetLayer> ColoredBackgroundDrawer::clone(widget * w) const
	{
		return handle<ColoredBackgroundDrawer>(_colors);
	}

	void ColoredBackgroundDrawer::draw(widget * w)
	{
		Color * color = _colors[0];
		int state = 0;

		if(w->isPressed())
			state = int(WidgetState::Pressed);
		else
			if(w->isPointed())
				state = int(WidgetState::Pointed);

		if(_colors[state] != nullptr)
			color = _colors[state];

		if(color != nullptr)
		{
			w->graphics()->setColor(*color);
			w->graphics()->rectangle(w->absRegion());
		}
	}

	ColoredBoundaryDrawer::ColoredBoundaryDrawer(const Color & color, uint width) : BoundaryDrawer(), _color(color), _width(width) {}

	handle<WidgetLayer> ColoredBoundaryDrawer::clone(widget * w) const
	{
		return handle<ColoredBoundaryDrawer>(_color, _width);
	}

	void ColoredBoundaryDrawer::draw(widget * w)
	{
		auto lw = hold(w->graphics()->lineWidthState(), int(_width));
		auto fm = hold(w->graphics()->fillModeState(), FillMode::Wireframe);
		w->graphics()->setColor(_color);
		w->graphics()->rectangle(w->absRegion());
	}

	ColoredButtonDecorator::ColoredButtonDecorator()
	{
		repeat(i, 3) _colors.emplace_back(nullptr);
		reset();
	}

	const Color & ColoredButtonDecorator::background() const
	{
		return _colors[int(WidgetState::Normal)] != nullptr ? *_colors[int(WidgetState::Normal)] : Color::transparent;
	}

	const Color & ColoredButtonDecorator::pressed() const
	{
		return _colors[int(WidgetState::Pressed)] != nullptr ? *_colors[int(WidgetState::Pressed)] : Color::transparent;
	}

	const Color & ColoredButtonDecorator::hovered() const
	{
		return _colors[int(WidgetState::Pointed)] != nullptr ? *_colors[int(WidgetState::Pointed)] : Color::transparent;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::background(const Color & color)
	{
		_colors[int(WidgetState::Normal)].init(color);
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::pressed(const Color & color)
	{
		_colors[int(WidgetState::Pressed)].init(color);
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::hovered(const Color & color)
	{
		_colors[int(WidgetState::Pointed)].init(color);
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::background(nullptr_t)
	{
		_colors[int(WidgetState::Normal)] = nullptr;
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::pressed(nullptr_t)
	{
		_colors[int(WidgetState::Pressed)] = nullptr;
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::hovered(nullptr_t)
	{
		_colors[int(WidgetState::Pointed)] = nullptr;
		return *this;
	}

	const Color & ColoredButtonDecorator::boundary_color() const
	{
		return _boundary.color != nullptr ? *_boundary.color : Color::transparent;
	}

	uint ColoredButtonDecorator::boundary_width() const
	{
		return _boundary.width;
	}

	const Color & ColoredButtonDecorator::boundary() const
	{
		return _boundary.color != nullptr ? *_boundary.color : Color::transparent;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::boundary(const Color & color)
	{
		_boundary.color.init(color);
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::boundary(uint width)
	{
		_boundary.width = width;
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::boundary(const Color & color, uint width)
	{
		_boundary.color.init(color);
		_boundary.width = width;
		return *this;
	}

	ColoredButtonDecorator & ColoredButtonDecorator::boundary(nullptr_t)
	{
		_boundary.color = nullptr;
		return *this;
	}

	void ColoredButtonDecorator::reset()
	{
		repeat(i, 3) _colors[i] = nullptr;
		_boundary.color = nullptr;
		_boundary.width = 1;
	}

	void ColoredButtonDecorator::apply(widget * w)
	{
		w->components->require<BackgroundDrawerComponent>()->set(unique<ColoredBackgroundDrawer>(_colors));

		if(_boundary.color != nullptr)
			w->components->require<BoundaryDrawerComponent>()->set(unique<ColoredBoundaryDrawer>(*_boundary.color, _boundary.width));
	}
}

//---------------------------------------------------------------------------
