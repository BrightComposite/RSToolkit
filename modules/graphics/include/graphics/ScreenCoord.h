//---------------------------------------------------------------------------

#ifndef SCREEN_COORD_H
#define SCREEN_COORD_H

//---------------------------------------------------------------------------

#include <math/Rect.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	subclass(AbsRect, IntRect);
	subclass(RelRect, FloatRect);
	subclass(SqRect,  FloatRect);

	subclass(AbsPoint, IntPoint);
	subclass(RelPoint, FloatPoint);
	subclass(SqPoint,  FloatPoint);

	subclass(AbsSize, IntSize);
	subclass(RelSize, FloatSize);
	subclass(SqSize,  FloatSize);

	class Viewport
	{
	public:
		Viewport(int width, int height) : _size(width, height), _ratio(float(height) / float(width)), _invratio(float(width) / float(height)) {}
		Viewport(const IntSize & size) : _size(size), _ratio(float(size.y) / float(size.x)), _invratio(float(size.x) / float(size.y)) {}

		Viewport & operator = (const IntSize & size)
		{
			set(size);
			return *this;
		}

		operator const IntSize & () const
		{
			return _size;
		}

		operator IntRect () const
		{
			return {_size};
		}

		const IntSize & size() const
		{
			return _size;
		}

		int width() const
		{
			return _size.x;
		}

		int height() const
		{
			return _size.y;
		}

		float ratio() const
		{
			return _ratio;
		}

		float invratio() const
		{
			return _invratio;
		}

		void set(int x, int y)
		{
			_size.x = x;
			_size.y = y;
			updateRatio();
		}

		void set(const IntSize & size)
		{
			_size = size;
			updateRatio();
		}

		void setX(int value)
		{
			_size.x = value;
			updateRatio();
		}

		void setY(int value)
		{
			_size.y = value;
			updateRatio();
		}

	protected:
		void updateRatio()
		{
			_ratio = float(_size.y) / float(_size.x);
			_invratio = float(_size.x) / float(_size.y);
		}

		float _ratio;
		float _invratio;

		IntSize _size;
	};

	struct api(graphics) ScreenCoord
	{
		static void toRel(const IntRect & rectAbs, const IntSize & viewport, FloatPoint & posRel, FloatSize & sizeRel);
		static void toRel(const IntRect & rectAbs, const IntSize & viewport, FloatRect & rectRel);
		static void toRel(const IntPoint & posAbs, const IntSize & viewport, FloatPoint & posRel);
		static void toRel(const IntSize & sizeAbs, const IntSize & viewport, FloatSize & sizeRel);
		static RelRect  toRel(const IntRect & rectAbs, const IntSize & viewport);
		static RelPoint toRel(const IntPoint & posAbs, const IntSize & viewport);
		static RelSize  toRel(const IntSize & sizeAbs, const IntSize & viewport);

		static void toAbs(const FloatRect & rectRel, const IntSize & viewport, IntPoint & posAbs, IntSize & sizeAbs);
		static void toAbs(const FloatRect & rectRel, const IntSize & viewport, IntRect & rectAbs);
		static void toAbs(const FloatPoint & posRel, const IntSize & viewport, IntPoint & posAbs);
		static void toAbs(const FloatSize & sizeRel, const IntSize & viewport, IntSize & sizeAbs);
		static AbsRect  toAbs(const FloatRect & rectRel, const IntSize & viewport);
		static AbsPoint toAbs(const FloatPoint & posRel, const IntSize & viewport);
		static AbsSize  toAbs(const FloatSize & sizeRel, const IntSize & viewport);

		static void toSq(const IntRect & rectAbs, const Viewport & viewport, FloatPoint & posSq, FloatSize & sizeSq);
		static void toSq(const IntRect & rectAbs, const Viewport & viewport, FloatRect & rectSq);
		static void toSq(const IntPoint & posAbs, const Viewport & viewport, FloatPoint & posSq);
		static void toSq(const IntSize & sizeAbs, const Viewport & viewport, FloatSize & sizeSq);
		static SqRect  toSq(const IntRect & rectAbs, const Viewport & viewport);
		static SqPoint toSq(const IntPoint & posAbs, const Viewport & viewport);
		static SqSize  toSq(const IntSize & sizeAbs, const Viewport & viewport);

		static void toRel(const FloatRect & rectSq, const Viewport & viewport, FloatPoint & posRel, FloatSize & sizeRel);
		static void toRel(const FloatRect & rectSq, const Viewport & viewport, FloatRect & rectRel);
		static void toRel(const FloatPoint & posSq, const Viewport & viewport, FloatPoint & posRel);
		static void toRel(const FloatSize & sizeSq, const Viewport & viewport, FloatSize & sizeRel);
		static SqRect  toRel(const FloatRect & rectSq, const Viewport & viewport);
		static SqPoint toRel(const FloatPoint & posSq, const Viewport & viewport);
		static SqSize  toRel(const FloatSize & sizeSq, const Viewport & viewport);
	};
}

//---------------------------------------------------------------------------
#endif
