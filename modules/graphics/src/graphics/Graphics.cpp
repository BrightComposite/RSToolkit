//---------------------------------------------------------------------------

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Graphics);

	void Graphics::bind(Surface * surface)
	{
		if(_surface == surface)
			return;

		_surface = surface;

		if(_surface != nullptr)
		{
			_surface->apply();
			updateSurface();
		}
	}

	void Graphics::bind(const Handle<Font> & font)
	{
		_font = font;
	}

	Surface * Graphics::surface()
	{
		return _surface;
	}

	const Viewport & Graphics::viewport() const
	{
		return _surface->_viewport;
	}

	const IntRect & Graphics::clipRect() const
	{
		return _clipRect;
	}

	const Color & Graphics::clearColor() const
	{
		return *_clearColor;
	}

	const Color & Graphics::color() const
	{
		return *_color;
	}

	int Graphics::fontSize() const
	{
		return *_fontSize;
	}

	int Graphics::lineWidth() const
	{
		return *_lineWidth;
	}

	FillMode Graphics::fillMode() const
	{
		return *_fillMode;
	}

	void Graphics::setFontSize(int size)
	{
		_fontSize->set(size);
	}

	void Graphics::setLineWidth(int size)
	{
		_lineWidth->set(size);
	}

	void Graphics::setFillMode(FillMode mode)
	{
		_fillMode->set(mode);
	}

	void Graphics::draw(const Image * image, const IntPoint & pt)
	{
		draw(image, pt.x, pt.y);
	}

	void Graphics::draw(const Image * image, const IntPoint & pt, const IntSize & sz)
	{
		draw(image, pt.x, pt.y, sz.x, sz.y);
	}

	void Graphics::draw(const Image * image, int x, int y)
	{
		draw(image, IntRect {x, y, x + image->width(), y + image->height()});
	}

	void Graphics::draw(const Image * image, int x, int y, int width, int height)
	{
		draw(image, IntRect {x, y, x + width, y + height});
	}

	void Graphics::draw(const Symbol * symbol, const IntPoint & pt)
	{
		draw(symbol, pt.x, pt.y);
	}

	void Graphics::draw(const string & text, int x, int y)
	{
		int newx;
		draw<string>(this, text, x, y, newx);
	}

	void Graphics::draw(const wstring & text, int x, int y)
	{
		int newx;
		draw<wstring>(this, text, x, y, newx);
	}

	void Graphics::draw(const string & text, const IntPoint & pt)
	{
		int newx;
		draw<string>(this, text, pt.x, pt.y, newx);
	}

	void Graphics::draw(const wstring & text, const IntPoint & pt)
	{
		int newx;
		draw<wstring>(this, text, pt.x, pt.y, newx);
	}

	void Graphics::draw(const string & text, int x, int y, int & newx)
	{
		draw<string>(this, text, x, y, newx);
	}

	void Graphics::draw(const wstring & text, int x, int y, int & newx)
	{
		draw<wstring>(this, text, x, y, newx);
	}

	void Graphics::draw(const string & text, const IntPoint & pt, int & newx)
	{
		draw<string>(this, text, pt.x, pt.y, newx);
	}

	void Graphics::draw(const wstring & text, const IntPoint & pt, int & newx)
	{
		draw<wstring>(this, text, pt.x, pt.y, newx);
	}

	IntSize Graphics::getTextSize(const string & text)
	{
		return _font != nullptr ? _font->getTextSize(this, text, fontSize()) : IntSize{0, 0};
	}

	IntSize Graphics::getTextSize(const wstring & text)
	{
		return _font != nullptr ? _font->getTextSize(this, text, fontSize()) : IntSize{0, 0};
	}

	State<Color> * Graphics::colorState()
	{
		return _color;
	}

	State<Color> * Graphics::clearColorState()
	{
		return _clearColor;
	}

	State<int> * Graphics::fontSizeState()
	{
		return _fontSize;
	}

	State<int> * Graphics::lineWidthState()
	{
		return _lineWidth;
	}

	State<FillMode> * Graphics::fillModeState()
	{
		return _fillMode;
	}

	Handle<ImageData> Graphics::requestSurfaceData() const
	{
		auto data = handle<ImageData>();
		_surface->requestData(data);

		return data;
	}

	void Graphics::updateSurface()
	{
		if(_surface != nullptr)
			clip(_surface->_viewport);
	}

	template<class string_t>
	void Graphics::draw(Graphics * graphics, const string_t & text, int x, int y, int & newx)
	{
		if(graphics->_font == nullptr)
			return;

		Handle<Symbol> symbol;
		newx = x;

		auto font = graphics->_font;
		int fontSize = graphics->_fontSize->get();

		font->getSymbol(symbol, graphics, fontSize, ' ');
		int space = symbol->_advance.x;
		int tab = space * 4;

		for(auto & ch : text)
		{
			switch(ch)
			{
				case '\t':
					newx = ((newx + space) / tab + 1) * tab;
					break;

				case ' ':
					newx += space;
					break;

				default:
					if(ch < ' ')
						break;

					font->getSymbol(symbol, graphics, fontSize, ch);
					graphics->draw(symbol, newx, y);

					newx += symbol->_advance.x;
			}
		}
	}

	template void Graphics::draw<string>(Graphics * graphics, const string & text, int x, int y, int & newx);
	template void Graphics::draw<wstring>(Graphics * graphics, const wstring & text, int x, int y, int & newx);

	void ScreenCoord::toRel(const IntRect & rectAbs, const IntSize & viewport, FloatPoint & posRel, FloatSize & sizeRel)
	{
		toRel(rectAbs.minPos(), viewport, posRel);
		toRel(rectAbs.size(), viewport, sizeRel);

		posRel[0] += sizeRel[0];
		posRel[1] -= sizeRel[1];
	}

	void ScreenCoord::toRel(const IntRect & rectAbs, const IntSize & viewport, FloatRect & rectRel)
	{
		FloatPoint pos;
		FloatSize size;
		toRel(rectAbs, viewport, pos, size);
		rectRel.setPlacement(pos, size);
	}

	void ScreenCoord::toRel(const IntPoint & posAbs, const IntSize & viewport, FloatPoint & posRel)
	{
		posRel[0] = 2.0f * static_cast<float>(posAbs[0]) / static_cast<float>(viewport.x) - 1.0f;
		posRel[1] = 1.0f - 2.0f * static_cast<float>(posAbs[1]) / static_cast<float>(viewport.y);
	}

	void ScreenCoord::toRel(const IntSize & sizeAbs, const IntSize & viewport, FloatSize & sizeRel)
	{
		sizeRel[0] = static_cast<float>(sizeAbs.x) / static_cast<float>(viewport.x);
		sizeRel[1] = static_cast<float>(sizeAbs.y) / static_cast<float>(viewport.y);
	}

	RelRect ScreenCoord::toRel(const IntRect & rectAbs, const IntSize & viewport)
	{
		FloatPoint pos;
		FloatSize size;
		toRel(rectAbs, viewport, pos, size);
		return {pos, size};
	}

	RelPoint ScreenCoord::toRel(const IntPoint & posAbs, const IntSize & viewport)
	{
		return {
			2.0f * static_cast<float>(posAbs[0]) / static_cast<float>(viewport.x) - 1.0f,
			1.0f - 2.0f * static_cast<float>(posAbs[1]) / static_cast<float>(viewport.y)
		};
	}

	RelSize ScreenCoord::toRel(const IntSize & sizeAbs, const IntSize & viewport)
	{
		return {
			static_cast<float>(sizeAbs.x) / static_cast<float>(viewport.x),
			static_cast<float>(sizeAbs.y) / static_cast<float>(viewport.y)
		};
	}

	void ScreenCoord::toAbs(const FloatRect & rectRel, const IntSize & viewport, IntPoint & posAbs, IntSize & sizeAbs)
	{
		auto posRel = rectRel.minPos();
		auto sizeRel = rectRel.size();

		posRel[0] -= sizeRel[0];
		posRel[1] += sizeRel[1];

		toAbs(sizeRel, viewport, sizeAbs);
		toAbs(posRel, viewport, posAbs);
	}

	void ScreenCoord::toAbs(const FloatRect & rectRel, const IntSize & viewport, IntRect & rectAbs)
	{
		IntPoint pos;
		IntSize size;
		toAbs(rectRel, viewport, pos, size);
		rectAbs.setPlacement(pos, size);
	}

	void ScreenCoord::toAbs(const FloatPoint & posRel, const IntSize & viewport, IntPoint & posAbs)
	{
		posAbs[0] = static_cast<int>((posRel.x + 1.0f) * 0.5 * viewport.x);
		posAbs[1] = static_cast<int>((1.0f - posRel.y) * 0.5 * viewport.y);
	}

	void ScreenCoord::toAbs(const FloatSize & sizeRel, const IntSize & viewport, IntSize & sizeAbs)
	{
		sizeAbs[0] = static_cast<int>(sizeRel.x * viewport.x);
		sizeAbs[1] = static_cast<int>(sizeRel.y * viewport.y);
	}

	void ScreenCoord::toSq(const IntRect & rectAbs, const Viewport & viewport, FloatPoint & posSq, FloatSize & sizeSq)
	{
		toSq(rectAbs.minPos(), viewport, posSq);
		toSq(rectAbs.size(), viewport, sizeSq);

		posSq[0] += sizeSq[0];
		posSq[1] -= sizeSq[1];
	}

	void ScreenCoord::toSq(const IntRect & rectAbs, const Viewport & viewport, FloatRect & rectSq)
	{
		SqPoint pos;
		SqSize size;
		toSq(rectAbs, viewport, pos, size);
		rectSq.setPlacement(pos, size);
	}

	void ScreenCoord::toSq(const IntPoint & posAbs, const Viewport & viewport, FloatPoint & posSq)
	{
		posSq[0] = posAbs.x * 2.0f / viewport.width() - 1.0f;
		posSq[1] = viewport.ratio() - posAbs.y * 2.0f / viewport.width();
	}

	void ScreenCoord::toSq(const IntSize & sizeAbs, const Viewport & viewport, FloatSize & sizeSq)
	{
		sizeSq[0] = float(sizeAbs.x) / viewport.width();
		sizeSq[1] = float(sizeAbs.y) / viewport.width();
	}

	SqRect ScreenCoord::toSq(const IntRect & rectAbs, const Viewport & viewport)
	{
		SqPoint pos;
		SqSize size;
		toSq(rectAbs, viewport, pos, size);
		return {pos, size};
	}

	SqPoint ScreenCoord::toSq(const IntPoint & posAbs, const Viewport & viewport)
	{
		return {
			posAbs.x * 2.0f / viewport.width() - 1.0f,
			viewport.ratio() - posAbs.y * 2.0f / viewport.width()
		};
	}

	SqSize ScreenCoord::toSq(const IntSize & sizeAbs, const Viewport & viewport)
	{
		return {
			float(sizeAbs.x) / viewport.width(),
			float(sizeAbs.y) / viewport.width()
		};
	}

	void ScreenCoord::toRel(const FloatRect & sqRect, const Viewport & viewport, FloatPoint & relPos, FloatSize & relSize)
	{
		auto p = sqRect.pos();
		auto s = sqRect.size();

		toRel(s, viewport, relSize);
		toRel({p.x, p.y}, viewport, relPos);
	}

	void ScreenCoord::toRel(const FloatRect & sqRect, const Viewport & viewport, FloatRect & relRect)
	{
		SqPoint pos;
		SqSize size;
		toRel(sqRect, viewport, pos, size);
		relRect.setPlacement(pos, size);
	}

	void ScreenCoord::toRel(const FloatPoint & sqPos, const Viewport & viewport, FloatPoint & relPos)
	{
		relPos[0] = sqPos.x;
		relPos[1] = sqPos.y * viewport.invratio();
	}

	void ScreenCoord::toRel(const FloatSize & sqSize, const Viewport & viewport, FloatSize & relSize)
	{
		relSize[0] = sqSize.x;
		relSize[1] = sqSize.y * viewport.invratio();
	}

	SqRect ScreenCoord::toRel(const FloatRect & sqRect, const Viewport & viewport)
	{
		SqPoint pos;
		SqSize size;
		toRel(sqRect, viewport, pos, size);
		return {pos, size};
	}

	SqPoint ScreenCoord::toRel(const FloatPoint & sqPos, const Viewport & viewport)
	{
		return {
			sqPos.x,
			sqPos.y * viewport.invratio()
		};
	}

	SqSize ScreenCoord::toRel(const FloatSize & sqSize, const Viewport & viewport)
	{
		return {
			sqSize.x,
			sqSize.y * viewport.invratio()
		};
	}
}