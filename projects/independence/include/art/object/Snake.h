//---------------------------------------------------------------------------

#ifndef SNAKE_H
#define SNAKE_H

//---------------------------------------------------------------------------

#include <core/container/Queue.h>
#include <scene/Scene.h>

#include <random>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Snake : public DrawableObject
	{
	public:
		Snake(Scene * scene, const colorf & color) : DrawableObject(scene)
		{
			_color = color;

			_initialPos.x = static_cast<float>(_scene->widget().width() / 2);
			_initialPos.y = static_cast<float>(_scene->widget().height() / 2);

			_oldPos = _pos = _initialPos;
			connect(_scene->widget(), this, &Snake::onWidgetResize);

			std::default_random_engine engine(static_cast<uint>(high_resolution_clock::now().time_since_epoch().count()));
			std::uniform_int_distribution<int> dist(5000000, 20000000);
			auto random = std::bind(dist, engine);

			for(int i = 0; i < 4; ++i)
				_q[i] = random();

			_timeOffset = random();
		}

		virtual ~Snake()
		{
			disconnect(_scene->widget(), this, &Snake::onWidgetResize);
		}

	protected:
		void onWidgetResize(Handle<WidgetResizeMessage> & msg, Widget & dest)
		{
			_initialPos.x = static_cast<float>(msg->width / 2);
			_initialPos.y = static_cast<float>(msg->height / 2);

			_parts.clear();
		}

		virtual void update(long long ticks) override
		{
			auto frequence = 8000 * (ticks + _timeOffset) * fmath::pi;

			FloatPoint offset = {
				std::sin(frequence / _q[0]),
				std::sin(frequence / _q[1])
			};

			FloatPoint scale = {
				std::sin(frequence / _q[2]) * (_initialPos.x - _size.x / 2),
				std::sin(frequence / _q[3]) * (_initialPos.y - _size.y / 2)
			};

			offset *= scale;

			if(_oldPos.distanceToSq(_pos) >= 100)
			{
				_oldPos = _pos;
				_parts.push_back(_oldPos);

				if(static_cast<int>(_parts.size()) > _length)
					_parts.pop_front();
			}

			setPos({_initialPos.x + offset.x, _initialPos.y + offset.y, _initialPos.z, _initialPos.w});
		}

		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const override
		{
			auto dt = hold(graphics.depthTestModeState(), true);

			FloatPoint offset(viewport.minPos());
			const FloatSize s0(_size / 2);

			if(_length > 0)
			{
				float ratio = 0.0f;
				float delta = 1.0f / (_length + 1);

				for(auto & p : _parts)
				{
					auto s1 = s0 * ratio;
					auto s2 = s1 + FloatSize {1.0f, 1.0f};
					auto c0 = _color * ratio;

					//graphics.setDepth(1.0f - ratio);
					graphics.setColor(c0 * FloatVector::half);
					graphics.ellipse(FloatRect {p.x - s2.x, p.y - s2.y, p.x + s2.x, p.y + s2.y} + offset);
					graphics.setColor(c0);
					graphics.ellipse(FloatRect {p.x - s1.x, p.y - s1.y, p.x + s1.x, p.y + s1.y} + offset);

					ratio += delta;
				}
			}

			auto s1 = s0 + FloatSize {2.0f, 2.0f};

			//graphics.setDepth(0.0f);
			graphics.setColor(_color * 0.5f);
			graphics.ellipse(FloatRect {_pos.x - s1.x, _pos.y - s1.y, _pos.x + s1.x, _pos.y + s1.y} + offset);
			graphics.setColor(_color);
			graphics.ellipse(FloatRect {_pos.x - s0.x, _pos.y - s0.y, _pos.x + s0.x, _pos.y + s0.y} + offset);
		}

		DoubleVector _initialPos;
		DoubleVector _oldPos;
		FloatSize _size = {30.0f, 30.0f};
		FloatVector _color = {1.0f, 1.0f, 1.0f, 1.0f};

		int _q[4];
		int _timeOffset;
		int _length = 24;

		deque<DoubleVector> _parts;
	};
}

//---------------------------------------------------------------------------
#endif
