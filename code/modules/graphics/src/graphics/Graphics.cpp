//---------------------------------------------------------------------------

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<> const GenericColor<ColorType::Float, ColorFormat::rgb> GenericColor<ColorType::Float, ColorFormat::rgb>::transparent = {0, 0, 0, 0};
	template<> const GenericColor<ColorType::Byte, ColorFormat::rgb> GenericColor<ColorType::Byte, ColorFormat::rgb>::transparent = {0, 0, 0, 0};
	template<> const GenericColor<ColorType::Float, ColorFormat::hsv> GenericColor<ColorType::Float, ColorFormat::hsv>::transparent = {0, 0, 0, 0};
	template<> const GenericColor<ColorType::Byte, ColorFormat::hsv> GenericColor<ColorType::Byte, ColorFormat::hsv>::transparent = {0, 0, 0, 0};
	
	template<> const GenericColor<ColorType::Float, ColorFormat::rgb> GenericColor<ColorType::Float, ColorFormat::rgb>::white = {1.0f, 1.0f, 1.0f, 1.0f};
	template<> const GenericColor<ColorType::Byte, ColorFormat::rgb> GenericColor<ColorType::Byte, ColorFormat::rgb>::white = {255, 255, 255, 255};
	template<> const GenericColor<ColorType::Float, ColorFormat::hsv> GenericColor<ColorType::Float, ColorFormat::hsv>::white = {0.0f, 0.0f, 1.0f, 1.0f};
	template<> const GenericColor<ColorType::Byte, ColorFormat::hsv> GenericColor<ColorType::Byte, ColorFormat::hsv>::white = {0, 0, 255, 255};
	
	template<> const GenericColor<ColorType::Float, ColorFormat::rgb> GenericColor<ColorType::Float, ColorFormat::rgb>::black = {0, 0, 0, 1.0f};
	template<> const GenericColor<ColorType::Byte, ColorFormat::rgb> GenericColor<ColorType::Byte, ColorFormat::rgb>::black = {0, 0, 0, 255};
	template<> const GenericColor<ColorType::Float, ColorFormat::hsv> GenericColor<ColorType::Float, ColorFormat::hsv>::black = {0, 0, 0, 1.0f};
	template<> const GenericColor<ColorType::Byte, ColorFormat::hsv> GenericColor<ColorType::Byte, ColorFormat::hsv>::black = {0, 0, 0, 255};
	
	void graphics::bind(asd::surface * surface) {
		if(_surface == surface) {
			return;
		}
		
		_surface = surface;
		
		if(_surface != nullptr) {
			_surface->apply();
			updateSurface();
		}
	}
	
	void graphics::bind(const handle<Font> & font) {
		_font = font;
	}
	
	asd::surface * graphics::surface() {
		return _surface;
	}
	
	const asd::viewport & graphics::viewport() const {
		return _surface->_viewport;
	}
	
	const int_rect & graphics::clipRect() const {
		return _clipRect;
	}
	
	const colorf & graphics::clearColor() const {
		return _clearColor->get();
	}
	
	const colorf & graphics::color() const {
		return _color->get();
	}
	
	int graphics::fontSize() const {
		return _fontSize->get();
	}
	
	int graphics::lineWidth() const {
		return _lineWidth->get();
	}
	
	FillMode graphics::fillMode() const {
		return _fillMode->get();
	}
	
	void graphics::setFontSize(int size) {
		_fontSize->set(size);
	}
	
	void graphics::setLineWidth(int size) {
		_lineWidth->set(size);
	}
	
	void graphics::setFillMode(FillMode mode) {
		_fillMode->set(mode);
	}
	
	void graphics::draw(const image * image, const int_point & pt) {
		draw(image, pt.x, pt.y);
	}
	
	void graphics::draw(const image * image, const int_point & pt, const int_size & sz) {
		draw(image, pt.x, pt.y, sz.x, sz.y);
	}
	
	void graphics::draw(const image * image, int x, int y) {
		draw(image, int_rect {x, y, x + image->width(), y + image->height()});
	}
	
	void graphics::draw(const image * image, int x, int y, int width, int height) {
		draw(image, int_rect {x, y, x + width, y + height});
	}
	
	void graphics::draw(const Symbol * symbol, const int_point & pt) {
		draw(symbol, pt.x, pt.y);
	}
	
	void graphics::draw(const string & text, int x, int y) {
		int newx;
		draw<string>(this, text, x, y, newx);
	}
	
	void graphics::draw(const wstring & text, int x, int y) {
		int newx;
		draw<wstring>(this, text, x, y, newx);
	}
	
	void graphics::draw(const string & text, const int_point & pt) {
		int newx;
		draw<string>(this, text, pt.x, pt.y, newx);
	}
	
	void graphics::draw(const wstring & text, const int_point & pt) {
		int newx;
		draw<wstring>(this, text, pt.x, pt.y, newx);
	}
	
	void graphics::draw(const string & text, int x, int y, int & newx) {
		draw<string>(this, text, x, y, newx);
	}
	
	void graphics::draw(const wstring & text, int x, int y, int & newx) {
		draw<wstring>(this, text, x, y, newx);
	}
	
	void graphics::draw(const string & text, const int_point & pt, int & newx) {
		draw<string>(this, text, pt.x, pt.y, newx);
	}
	
	void graphics::draw(const wstring & text, const int_point & pt, int & newx) {
		draw<wstring>(this, text, pt.x, pt.y, newx);
	}
	
	int_size graphics::getTextSize(const string & text) {
		return _font != nullptr ? _font->getTextSize(this, text, fontSize()) : int_size{0, 0};
	}
	
	int_size graphics::getTextSize(const wstring & text) {
		return _font != nullptr ? _font->getTextSize(this, text, fontSize()) : int_size{0, 0};
	}
	
	state<Color> * graphics::colorState() {
		return _color;
	}
	
	state<Color> * graphics::clearColorState() {
		return _clearColor;
	}
	
	state<int> * graphics::fontSizeState() {
		return _fontSize;
	}
	
	state<int> * graphics::lineWidthState() {
		return _lineWidth;
	}
	
	state<FillMode> * graphics::fillModeState() {
		return _fillMode;
	}
	
	handle<image_data> graphics::requestSurfaceData() const {
		auto data = make::handle<image_data>();
		_surface->requestData(data);
		
		return data;
	}
	
	void graphics::updateSurface() {
		if(_surface != nullptr) {
			clip(_surface->_viewport);
		}
	}
	
	template<class string_t>
	void graphics::draw(graphics * graphics, const string_t & text, int x, int y, int & newx) {
		if(graphics->_font == nullptr) {
			return;
		}
		
		handle<Symbol> symbol;
		newx = x;
		
		auto font = graphics->_font;
		int fontSize = graphics->_fontSize->get();
		
		font->getSymbol(symbol, graphics, fontSize, ' ');
		int space = symbol->_advance.x;
		int tab = space * 4;
		
		for(auto & ch : text) {
			switch(ch) {
				case '\t':
					newx = ((newx + space) / tab + 1) * tab;
					break;
				
				case ' ':
					newx += space;
					break;
				
				default:
					if(ch < ' ') {
						break;
					}
					
					font->getSymbol(symbol, graphics, fontSize, ch);
					graphics->draw(symbol, newx, y);
					
					newx += symbol->_advance.x;
			}
		}
	}
	
	template void graphics::draw<string>(graphics * graphics, const string & text, int x, int y, int & newx);
	template void graphics::draw<wstring>(graphics * graphics, const wstring & text, int x, int y, int & newx);
	
	void ScreenCoord::toRel(const int_rect & rectAbs, const int_size & viewport, float_point & posRel, float_size & sizeRel) {
		toRel(rectAbs.minPos(), viewport, posRel);
		toRel(rectAbs.size(), viewport, sizeRel);
		
		posRel[0] += sizeRel[0];
		posRel[1] -= sizeRel[1];
	}
	
	void ScreenCoord::toRel(const int_rect & rectAbs, const int_size & viewport, float_rect & rectRel) {
		float_point pos;
		float_size size;
		toRel(rectAbs, viewport, pos, size);
		rectRel.setPlacement(pos, size);
	}
	
	void ScreenCoord::toRel(const int_point & posAbs, const int_size & viewport, float_point & posRel) {
		posRel[0] = 2.0f * static_cast<float>(posAbs[0]) / static_cast<float>(viewport.x) - 1.0f;
		posRel[1] = 1.0f - 2.0f * static_cast<float>(posAbs[1]) / static_cast<float>(viewport.y);
	}
	
	void ScreenCoord::toRel(const int_size & sizeAbs, const int_size & viewport, float_size & sizeRel) {
		sizeRel[0] = static_cast<float>(sizeAbs.x) / static_cast<float>(viewport.x);
		sizeRel[1] = static_cast<float>(sizeAbs.y) / static_cast<float>(viewport.y);
	}
	
	rel_rect ScreenCoord::toRel(const int_rect & rectAbs, const int_size & viewport) {
		float_point pos;
		float_size size;
		toRel(rectAbs, viewport, pos, size);
		return {pos, size};
	}
	
	rel_point ScreenCoord::toRel(const int_point & posAbs, const int_size & viewport) {
		return {
			2.0f * static_cast<float>(posAbs[0]) / static_cast<float>(viewport.x) - 1.0f,
			1.0f - 2.0f * static_cast<float>(posAbs[1]) / static_cast<float>(viewport.y)
		};
	}
	
	rel_size ScreenCoord::toRel(const int_size & sizeAbs, const int_size & viewport) {
		return {
			static_cast<float>(sizeAbs.x) / static_cast<float>(viewport.x),
			static_cast<float>(sizeAbs.y) / static_cast<float>(viewport.y)
		};
	}
	
	void ScreenCoord::toAbs(const float_rect & rectRel, const int_size & viewport, int_point & posAbs, int_size & sizeAbs) {
		auto posRel = rectRel.minPos();
		auto sizeRel = rectRel.size();
		
		posRel[0] -= sizeRel[0];
		posRel[1] += sizeRel[1];
		
		toAbs(sizeRel, viewport, sizeAbs);
		toAbs(posRel, viewport, posAbs);
	}
	
	void ScreenCoord::toAbs(const float_rect & rectRel, const int_size & viewport, int_rect & rectAbs) {
		int_point pos;
		int_size size;
		toAbs(rectRel, viewport, pos, size);
		rectAbs.setPlacement(pos, size);
	}
	
	void ScreenCoord::toAbs(const float_point & posRel, const int_size & viewport, int_point & posAbs) {
		posAbs[0] = static_cast<int>((posRel.x + 1.0f) * 0.5 * viewport.x);
		posAbs[1] = static_cast<int>((1.0f - posRel.y) * 0.5 * viewport.y);
	}
	
	void ScreenCoord::toAbs(const float_size & sizeRel, const int_size & viewport, int_size & sizeAbs) {
		sizeAbs[0] = static_cast<int>(sizeRel.x * viewport.x);
		sizeAbs[1] = static_cast<int>(sizeRel.y * viewport.y);
	}
	
	void ScreenCoord::toSq(const int_rect & rectAbs, const viewport & viewport, float_point & posSq, float_size & sizeSq) {
		toSq(rectAbs.minPos(), viewport, posSq);
		toSq(rectAbs.size(), viewport, sizeSq);
		
		posSq[0] += sizeSq[0];
		posSq[1] -= sizeSq[1];
	}
	
	void ScreenCoord::toSq(const int_rect & rectAbs, const viewport & viewport, float_rect & rectSq) {
		sq_point pos;
		sq_size size;
		toSq(rectAbs, viewport, pos, size);
		rectSq.setPlacement(pos, size);
	}
	
	void ScreenCoord::toSq(const int_point & posAbs, const viewport & viewport, float_point & posSq) {
		posSq[0] = posAbs.x * 2.0f / viewport.width() - 1.0f;
		posSq[1] = viewport.ratio() - posAbs.y * 2.0f / viewport.width();
	}
	
	void ScreenCoord::toSq(const int_size & sizeAbs, const viewport & viewport, float_size & sizeSq) {
		sizeSq[0] = float(sizeAbs.x) / viewport.width();
		sizeSq[1] = float(sizeAbs.y) / viewport.width();
	}
	
	sq_rect ScreenCoord::toSq(const int_rect & rectAbs, const viewport & viewport) {
		sq_point pos;
		sq_size size;
		toSq(rectAbs, viewport, pos, size);
		return {pos, size};
	}
	
	sq_point ScreenCoord::toSq(const int_point & posAbs, const viewport & viewport) {
		return {
			posAbs.x * 2.0f / viewport.width() - 1.0f,
			viewport.ratio() - posAbs.y * 2.0f / viewport.width()
		};
	}
	
	sq_size ScreenCoord::toSq(const int_size & sizeAbs, const viewport & viewport) {
		return {
			float(sizeAbs.x) / viewport.width(),
			float(sizeAbs.y) / viewport.width()
		};
	}
	
	void ScreenCoord::toRel(const float_rect & sqRect, const viewport & viewport, float_point & relPos, float_size & relSize) {
		auto p = sqRect.pos();
		auto s = sqRect.size();
		
		toRel(s, viewport, relSize);
		toRel({p.x, p.y}, viewport, relPos);
	}
	
	void ScreenCoord::toRel(const float_rect & sqRect, const viewport & viewport, float_rect & relRect) {
		sq_point pos;
		sq_size size;
		toRel(sqRect, viewport, pos, size);
		relRect.setPlacement(pos, size);
	}
	
	void ScreenCoord::toRel(const float_point & sqPos, const viewport & viewport, float_point & relPos) {
		relPos[0] = sqPos.x;
		relPos[1] = sqPos.y * viewport.invratio();
	}
	
	void ScreenCoord::toRel(const float_size & sqSize, const viewport & viewport, float_size & relSize) {
		relSize[0] = sqSize.x;
		relSize[1] = sqSize.y * viewport.invratio();
	}
	
	sq_rect ScreenCoord::toRel(const float_rect & sqRect, const viewport & viewport) {
		sq_point pos;
		sq_size size;
		toRel(sqRect, viewport, pos, size);
		return {pos, size};
	}
	
	sq_point ScreenCoord::toRel(const float_point & sqPos, const viewport & viewport) {
		return {
			sqPos.x,
			sqPos.y * viewport.invratio()
		};
	}
	
	sq_size ScreenCoord::toRel(const float_size & sqSize, const viewport & viewport) {
		return {
			sqSize.x,
			sqSize.y * viewport.invratio()
		};
	}
}
