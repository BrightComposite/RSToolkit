//---------------------------------------------------------------------------

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Graphics);

	void Graphics::bind(const Handle<Surface> & surface)
	{
		if(_surface == surface)
			return;

		_surface = surface;
		_surface->apply();

		clip(viewport());
	}

	void Graphics::bind(const Handle<Font> & font)
	{
		_font = font;
	}

	const Handle<Surface> & Graphics::surface()
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
		draw<string>(this, text, x, y);
	}

	void Graphics::draw(const wstring & text, int x, int y)
	{
		draw<wstring>(this, text, x, y);
	}

	void Graphics::draw(const string & text, const IntPoint & pt)
	{
		draw(text, pt.x, pt.y);
	}

	void Graphics::draw(const wstring & text, const IntPoint & pt)
	{
		draw(text, pt.x, pt.y);
	}

	IntSize Graphics::getTextSize(const string & text)
	{
		return textSize<string>(this, text);
	}

	IntSize Graphics::getTextSize(const wstring & text)
	{
		return textSize<wstring>(this, text);
	}

	State<Color> * Graphics::colorState()
	{
		return _color;
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

	template<class string_t>
	IntSize Graphics::textSize(Graphics * graphics, const string_t & text)
	{
		if(graphics->_font == nullptr)
			return {0, 0};

		Handle<Symbol> symbol;
		IntSize size;

		graphics->_font->getSymbol(symbol, graphics, graphics->_fontSize->get(), ' ');
		int space = symbol->_advance.x;
		int tab = space * 4;

		for(size_t i = 0; i < text.length(); ++i)
		{
			auto & ch = text[i];

			switch(ch)
			{
			case '\t':
				size.x = ((size.x + space) / tab + 1) * tab;
				break;

			case ' ':
				size.x += space;
				break;

			default:
				if(ch < ' ')
					break;

				graphics->_font->getSymbol(symbol, graphics, graphics->_fontSize->get(), ch);
				size.x += symbol->_advance.x;

				if(symbol->_image != nullptr)
				{
					int h = symbol->_image->height() + symbol->_top;

					if(h > size.y)
						size.y = h;

					if(i == text.length() - 1)
						size.x += std::max(0, static_cast<int>(symbol->_image->width()) - symbol->_advance.x);
				}
			}
		}

		return size;
	}

	template<class string_t>
	void Graphics::draw(Graphics * graphics, const string_t & text, int x, int y)
	{
		if(graphics->_font == nullptr)
			return;

		Handle<Symbol> symbol;

		graphics->_font->getSymbol(symbol, graphics, graphics->_fontSize->get(), ' ');
		int space = symbol->_advance.x;
		int tab = space * 4;
		int offset = 0;

		for(size_t i = 0; i < text.length(); ++i)
		{
			auto & ch = text[i];

			switch(ch)
			{
			case '\t':
				offset = ((offset + space) / tab + 1) * tab;
				break;

			case ' ':
				offset += space;
				break;

			default:
				if(ch < ' ')
					break;

				graphics->_font->getSymbol(symbol, graphics, graphics->_fontSize->get(), ch);
				graphics->draw(symbol, x + offset, y);

				offset += symbol->_advance.x;
			}
		}
	}

	template IntSize Graphics::textSize<string>(Graphics * graphics, const string & text);
	template IntSize Graphics::textSize<wstring>(Graphics * graphics, const wstring & text);

	template void Graphics::draw<string>(Graphics * graphics, const string & text, int x, int y);
	template void Graphics::draw<wstring>(Graphics * graphics, const wstring & text, int x, int y);

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