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
		Snake(Scene * scene, const color4 & color) : DrawableObject(scene)
		{
			_color.set(color);

			_initialPos.x = static_cast<float>(_scene->widget()->width() / 2);
			_initialPos.y = static_cast<float>(_scene->widget()->height() / 2);

			_oldPos = _pos = _initialPos;
			connect(*_scene->widget(), share(this), &Snake::onWidgetResize);

			std::default_random_engine engine(static_cast<uint>(high_resolution_clock::now().time_since_epoch().count()));
			std::uniform_int_distribution<int> dist(5000000, 20000000);
			auto random = std::bind(dist, engine);

			for(int i = 0; i < 4; ++i)
				_q[i] = random();

			_timeOffset = random();
		}

	protected:
		void onWidgetResize(Handle<WidgetResizeMessage> & msg, WidgetRegion & dest, const Subject * source)
		{
			_initialPos.x = 0.5 * msg->width;
			_initialPos.y = 0.5 * msg->height;

			_parts.clear();
		}

		virtual void update(long long ticks) override
		{
			auto frequence = 8000 * (ticks + _timeOffset) * FloatMath::pi;

			auto s = DoubleVectorMath::sin(frequence / _q);

			auto offset = s.blend<0, 0, 1, 1>(DoubleVector::zero);
			offset *= s.shuffle<2, 3, 0, 0>() * (_initialPos - _size * DoubleVector::half);

			if(_oldPos.distanceToSq(_pos) >= 100)
			{
				_oldPos = _pos;
				_parts.push_back(_oldPos);

				if(_parts.size() > static_cast<size_t>(_length))
					_parts.pop_front();
			}

			setPos(_initialPos + offset);
		}

		virtual void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const override
		{
			auto dt = hold(graphics->depthTestModeState(), true);

			const FloatPoint & offset = viewport.minPos();
			const auto s0 = _size * DoubleVector::half;

			if(_length > 0)
			{
				float ratio = 0.0f;
				float delta = 1.0f / (_length + 1);

				for(auto & p : _parts)
				{
					auto s1 = s0 * ratio;
					auto s2 = s1 + DoubleVector::one;
					auto c0 = _color * ratio;

					graphics->setDepth(1.0f - ratio);
					graphics->setColor(c0 * FloatVector::half);
					graphics->ellipse(FloatRect {p.x - s2.x, p.y - s2.y, p.x + s2.x, p.y + s2.y} +offset);
					graphics->setColor(c0);
					graphics->ellipse(FloatRect {p.x - s1.x, p.y - s1.y, p.x + s1.x, p.y + s1.y} +offset);

					ratio += delta;
				}
			}

			auto s1 = s0 + DoubleVector::two;

			graphics->setDepth(0.0f);
			graphics->setColor(_color * FloatVector::half);
			graphics->ellipse(FloatRect {_pos.x - s1.x, _pos.y - s1.y, _pos.x + s1.x, _pos.y + s1.y} +offset);
			graphics->setColor(_color);
			graphics->ellipse(FloatRect {_pos.x - s0.x, _pos.y - s0.y, _pos.x + s0.x, _pos.y + s0.y} +offset);
		}

		DoubleVector _initialPos;
		DoubleVector _oldPos;
		DoubleVector _size = {30.0f, 30.0f, 0.0f};
		FloatVector _color = {1.0f, 1.0f, 1.0f, 1.0f};

		DoubleVector _q;
		int _timeOffset;
		int _length = 24;

		deque<DoubleVector> _parts;
	};
}

//---------------------------------------------------------------------------
#endif
