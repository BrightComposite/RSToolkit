//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/Application.h>
#include <application/ThreadLoop.h>

#include <core/action/Action.h>
#include <core/action/Thread.h>

#include <direct3d/Direct3D11.h>

#include <freeimage/FreeImageConverter.h>

#include <freetype/FreeTypeDecoder.h>

#include <io/FileSystem.h>

#include <math/Vector.h>
#include <math/Matrix.h>

#include <ui/Window.h>
#include <ui/Panel.h>

#include <graphics/text/Font.h>

#include <windows/Message.h>

#include <vld.h>

//---------------------------------------------------------------------------

using Rapture::FinalAction;
using Rapture::Thread;
using Rapture::Handle;
using Rapture::handle;
using Rapture::Object;
using Rapture::OwnedByteData;

using Rapture::Entrance;
using Rapture::ThreadLoop;

using Rapture::IntRect;
using Rapture::Matrix2x2;
using Rapture::Matrix;
using Rapture::vec;

using Rapture::color;
using Rapture::color4;
using Rapture::ImageConverter;
using Rapture::ImageData;
using Rapture::ImageIO;

using Rapture::Layer;
using Rapture::Panel;
using Rapture::Widget;
using Rapture::Window;
using Rapture::WindowAdapter;
using Rapture::WindowBackground;

using Rapture::WidgetChangedStateMessage;

using Rapture::FreeImageConverter;
using Rapture::FreeTypeDecoder;

using Rapture::Graphics;
using Rapture::Graphics3D;
using Rapture::Image;

using Rapture::print;
using Rapture::operator <<;

using std::bind;

using std::cout;
using std::endl;
using std::thread;

using boost::filesystem::path;

using namespace std::chrono;
using namespace DirectX;

typedef high_resolution_clock hrc;

//---------------------------------------------------------------------------

static int readNextMessage();
static int render(Handle<Window> & window);

//---------------------------------------------------------------------------

time_point<hrc> last, lastFrame, lastSecond;
long long t;
const size_t count = 999;

float foo(float n)
{
	return n;
}

class ChildWidget : public Panel
{
public:
	ChildWidget(Widget * parent) : ChildWidget(parent, *parent) {}
	ChildWidget(Widget * parent, const IntRect & region) : Panel(parent, region)
	{
		setFocusability(true);
	}
};

static void loadWindow()
{
	using namespace Rapture;
	using Rapture::Font;

	Graphics3D * graphics = D3DGraphics::initialize();
	FinalAction finally(D3DGraphics::free);

	Handle<Window> window(graphics, 0, 0, 1280, 480);

	Handle<WindowBackground> back(window);
	back->name = "Background";

	graphics->setClearColor({1.0f, 1.0f, 1.0f});

#define HOTKEY_FULLSCREEN 0x20

	window->registerHotkey(HOTKEY_FULLSCREEN, VK_RETURN, MOD_ALT);

	dest_connect(*window, WindowAdapter, WindowHotkeyMessage)
	{
		auto window = static_cast<Window *>(&dest);

		switch(msg->id)
		{
		case HOTKEY_FULLSCREEN:
			window->toggleFullscreen();
			break;
		}
	};

	global_connect(Widget, MouseDownMessage)
	{
		switch(msg->button)
		{
		case MouseButton::Left:
			cout << "Widget " << dest.name << " has been pressed!" << endl;
			break;
		}
	};

	dest_connect(*window, WindowAdapter, KeyUpMessage)
	{
		auto window = static_cast<Window *>(&dest);

		switch(msg->key)
		{
		case VK_RETURN:
			if(IsKeyPressed(VK_MENU))
				break;

			if(window->state() == WindowState::Maximized)
				window->restore();
			else
				window->maximize();

			break;

		case VK_ESCAPE:
			window->close();
			break;
		}
	};

	auto root = initial_path();

	if(root.filename() == "x86" || root.filename() == "x64")
	{
		root = root.parent_path().parent_path();
	}
	else
	{
		root = root.parent_path().parent_path().parent_path();
	}

	auto image_path = root / "data/images/witcher3";
	Handle<Image> image1, image2, image3;

	try
	{
		image1 = Image::load(graphics, image_path + ".png");
		image2 = Image::load(graphics, image_path + "a.png");
		image3 = Image::load(graphics, image_path + "b.png");
	}
	catch(...)
	{
		return;
	}

	auto panelDrawer = [](const Widget * widget, const IntRect & region)
	{
		IntRect inner(widget->absRegion());
		inner.resize(-2);

		auto g = widget->graphics();

		g->setColor({0.4f, 0.4f, 0.4f, 1.0f});
		g->rectangle(widget->absRegion());
		g->setColor({1.0f, 1.0f, 1.0f, 1.0f});
		g->rectangle(inner);
	};

	auto childDrawer = [](const Widget * widget, const IntRect & region)
	{
		IntRect inner(widget->absRegion());
		inner.resize(-2);

		auto g = widget->graphics();

		if(widget->isFocused())
			g->setColor({0.75f, 0.75f, 0.75f, 1.0f});
		else
			g->setColor({0.0f, 0.0f, 0.0f, 1.0f});
		
		g->rectangle(widget->absRegion());

		if(widget->isPressed())
			g->setColor({0.0f, 1.0f, 0.0f, 1.0f});
		else if(widget->isPointed())
			g->setColor({1.0f, 1.0f, 0.0f, 1.0f});
		else
			g->setColor({0.5f, 0.5f, 0.5f, 1.0f});

		g->rectangle(inner);
	};

	auto logoDrawer = [image1, image2, image3](const Widget * widget, const IntRect & region)
	{
		if(widget->isPressed())
			widget->graphics()->draw(image2, widget->absRegion());
		else if(widget->isPointed())
			widget->graphics()->draw(image3, widget->absRegion());
		else
			widget->graphics()->draw(image1,  widget->absRegion());
	};

	auto data = handle<ImageData>();
	image1->requestData(data);

	ImageIO::save(image_path + ".bmp", data);
	ImageIO::save(image_path + ".jpg", data);
	ImageIO::save(image_path + ".ico", data);
	ImageIO::save(image_path + ".tga", data);

	std::vector<IntRect> rects = {
		{10, 10, 30, 30},
		{40, 10, 60, 30},
		{10, 40, 30, 60},
		{40, 40, 60, 60}
	};

	const int dx = 10;
	const int dy = 10;
	const int w = 70;
	const int h = 70;

	std::default_random_engine rndm;
	std::bernoulli_distribution dist;
	auto isVisible = std::bind(dist, rndm);

	for(int iy = 0; iy < 4; ++iy)
	{
		for(int ix = 0; ix < 4; ++ix)
		{
			int x = (dx + w) * (ix + 1);
			int y = (dy + h) * (iy + 1);

			auto panel = back->append<Panel>(IntRect {x - w, y - h, x, y});
			panel->name = "panel("_s << ix << ',' << iy << ')';

			panel << panelDrawer;

			for(int ir = 0; ir < 4; ++ir)
			{
				auto child = panel->append<ChildWidget>(rects[ir]);
				child->name = "child("_s << ix << ',' << iy << ',' << ir << ')';
				child->setVisibility(isVisible());

				child << childDrawer;
			}
		}
	}

	Handle<Panel> logo(back);

	logo->name = "Logo";
	logo->setPlacement(ModelMask::RightTop, {0, 40}, {128, 128});

	logo << logoDrawer;

	try
	{
		FontCache::set("Times New Roman",
		{
			{FontStyle::Regular,	Font::load("times.ttf")},
			{FontStyle::Bold,		Font::load("timesbd.ttf")},
			{FontStyle::Italic,		Font::load("timesi.ttf")},
			{FontStyle::BoldItalic, Font::load("timesbi.ttf")}
		});

		FontCache::set("Arial",
		{
			{FontStyle::Regular,	Font::load("arial.ttf")},
			{FontStyle::Bold,		Font::load("arialbd.ttf")},
			{FontStyle::Italic,		Font::load("ariali.ttf")},
			{FontStyle::BoldItalic, Font::load("arialbi.ttf")}
		});

		FontCache::set("Trebuchet",
		{
			{FontStyle::Regular,	Font::load("trebuc.ttf")},
			{FontStyle::Bold,		Font::load("trebucbd.ttf")},
			{FontStyle::Italic,		Font::load("trebucit.ttf")},
			{FontStyle::BoldItalic, Font::load("trebucbi.ttf")}
		});

		FontCache::set("Pristina", Font::load("pristina.ttf"));
	}
	catch(...) {}

	auto arial_italic = FontCache::get("Arial", FontStyle::Italic);
	auto pristina_font = FontCache::get("Pristina", FontStyle::Regular);

	if(pristina_font != nullptr)
	{
		int fontSize = 24;
		Handle<Panel> label(back);
		Handle<Panel> english_label(back);

		Handle<String> engine_text(L"Rapture State Engine");
		Handle<String> english_text(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,-:;\" \'.?!\\/~`");

		graphics->bind(pristina_font);
		graphics->setFontSize(fontSize);

		label->name = "Engine Label";
		label->setPlacement(ModelMask::RightTop, {-10, 10}, graphics->getTextSize(*engine_text));

		english_label->name = "Text Label";
		english_label->setPlacement({10, 10}, graphics->getTextSize(*english_text));

		label << [engine_text, pristina_font, fontSize](const Widget * widget, const IntRect & region)
		{
			auto * graphics = widget->graphics();

			graphics->bind(pristina_font);
			graphics->setFontSize(fontSize);

			graphics->setColor({0.8f, 0.8f, 0.86f});
			graphics->draw(*engine_text, widget->absPos());
		};

		english_label << [english_text, pristina_font, fontSize](const Widget * widget, const IntRect & region)
		{
			auto * graphics = widget->graphics();

			graphics->bind(pristina_font);
			graphics->setFontSize(fontSize);

			graphics->setColor({0.0f, 0.0f, 0.0f});
			graphics->draw(*english_text, widget->absPos());
		};
	}

	if(arial_italic != nullptr)
	{
		int fontSize = 16;
		Handle<Panel> russian_label(back);
		Handle<WideString> russian_text(L"ÀÁÂÃÄÅ¨ÆÇÈÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäå¸æçèêëìíîïðñòóôõö÷øùúûüýþÿ");

		graphics->bind(arial_italic);
		graphics->setFontSize(fontSize);

		russian_label->name = "Russian Text Label";
		russian_label->setPlacement({10, 50}, graphics->getTextSize(*russian_text));

		russian_label << [russian_text, arial_italic, fontSize](const Widget * widget, const IntRect & region)
		{
			auto * graphics = widget->graphics();

			graphics->bind(arial_italic);
			graphics->setFontSize(fontSize);

			graphics->setColor({0.0f, 0.0f, 0.0f});
			graphics->draw(*russian_text, widget->absPos());
		};
	}

	window->setCaption(L"Rapture::Direct3D test");

	ThreadLoop::add(processWindowMessage);
	ThreadLoop::add(bind(render, window));

	window->show();
	window->centralize();

	lastFrame = lastSecond = hrc::now();
	ThreadLoop::run();
}

static int load()
{
	using namespace Rapture;

	try
	{
		FreeImageConverter::initialize();
		FreeTypeDecoder::initialize();

		Handle<Thread> th(loadWindow);
		
		Matrix2x2<float> mat1 {
			2.0f, 2.0f,
			1.0f, 5.0f
		};

		Matrix2x2<float> mat2 {mat1};

		FloatMatrix fmat {
			 2.0f, 2.0f, 2.0f, 2.0f,
			 1.0f, 5.0f, 9.0f, 3.0f,
			 0.0f, 8.0f, 6.0f, 4.0f,
			 8.0f, 1.0f, 3.0f, 5.0f
		};

		auto fmat2 = fmat;

		XMMATRIX MAT {
			2.0f, 2.0f, 2.0f, 2.0f,
			1.0f, 5.0f, 9.0f, 3.0f,
			0.0f, 8.0f, 6.0f, 4.0f,
			8.0f, 1.0f, 3.0f, 5.0f
		};

		auto MAT2 = MAT;

		auto v1 = vec(1.0f, 1.0f, 1.0f, 1.0f);
		auto v2 = v1;

		auto vec1 = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		auto vec2 = vec1;

		cout << "Math facilities comparison (Rapture vs DirectX)." << endl;
		cout << "Each operation is made " << count << " times." << endl;
		cout << "This test also shows the advantages of Matrix2x2." << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		cout << "Rapture::Vector: " << v1 << endl;
		cout << "DirectX::XMVECTOR: " << vec1 << endl;

		cout << "Rapture::Matrix2x2: " << mat1 << endl;
		cout << "DirectX::XMMATRIX: " << MAT << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			v2 = mat1 * v2;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture vector and matrix2x2 multiplication time: " << t << endl;

		cout << "Vector: " << v2 << endl;

		v2 = v1;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			v2 = mat1.transform(v2);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture vector transformation time (2x2): " << t << endl;

		cout << "Vector: " << v2 << endl;

		v2 = v1;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			vec2 = DirectX::XMVector2Transform(vec2, MAT);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX vector transformation time (2x2): " << t << endl;

		cout << "Vector: " << vec2 << endl;

		vec2 = vec1;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			mat2 = mat2 * mat1;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix2x2 multiplication time: " << t << endl;

		cout << "Matrix: " << mat2 << endl;

		mat2 = mat1;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			mat2 *= mat1;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix2x2 self-multiplication time: " << t << endl;

		cout << "Matrix: " << mat2 << endl;

		mat2 = mat1;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			MAT2 = MAT2 * MAT;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "XMMATRIX multiplication time: " << t << endl;

		cout << "Matrix: " << MAT2 << endl;

		MAT2 = MAT;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			MAT2 *= MAT;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "XMMATRIX self-multiplication time: " << t << endl;

		cout << "Matrix: " << MAT2 << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		MAT2 = MAT;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			volatile auto & m = mat2.transpose();
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix2x2 transpose time: " << t << endl;

		cout << "Matrix: " << mat2 << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			volatile auto & m = fmat2.transpose();
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 transpose time: " << t << endl;

		cout << "Matrix: " << fmat2 << endl;

		fmat2 = fmat;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			MAT2 = DirectX::XMMatrixTranspose(MAT2);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX matrix transpose time: " << t << endl;

		cout << "Matrix: " << MAT2 << endl;

		MAT2 = MAT;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		FloatVector sv, cv;
		v2 = {FloatMath::pi, -FloatMath::pi / 3, FloatMath::pi / 4, -FloatMath::pi / 6};

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			FloatVectorMath::sincos(v2, sv, cv);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture vector sin cos time: " << t << endl;

		cout << "Sin: " << sv << ". Cos: " << cv << endl;

		v2 = v1;

		XMVECTOR svec, cvec;
		vec2 = {FloatMath::pi, -FloatMath::pi / 3, FloatMath::pi / 4, -FloatMath::pi / 6};

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			XMVectorSinCos(&svec, &cvec, vec2);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX vector sin cos time: " << t << endl;

		cout << "Sin: " << svec << ". Cos: " << cvec << endl;

		vec2 = vec1;

		float vsc[4] = {FloatMath::pi, -FloatMath::pi / 3, FloatMath::pi / 4, -FloatMath::pi / 6};

		FloatVector sf = {std::sin(vsc[0]), std::sin(vsc[1]), std::sin(vsc[2]), std::sin(vsc[3])};
		FloatVector cf = {std::cos(vsc[0]), std::cos(vsc[1]), std::cos(vsc[2]), std::cos(vsc[3])};

		cout << "Sin: " << sf << ". Cos: " << cf << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			fmat2 = fmat2.rotationMatrix(fmat2[0].positiveX, FloatMath::pi);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 rotation time: " << t << endl;

		cout << "Matrix: " << fmat2 << endl;

		fmat2 = fmat;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			MAT2 = XMMatrixRotationNormal(FloatVector::positiveX, FloatMath::pi);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX matrix rotation time: " << t << endl;

		cout << "Matrix: " << MAT2 << endl;

		MAT2 = MAT;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			fmat2 = fmat2.rotationMatrix({0.0f, 0.0f, 1.0f});
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 rotation time from Euler: " << t << endl;

		cout << "Matrix: " << fmat2 << endl;

		fmat2 = fmat;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			MAT2 = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 1.0f);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX matrix rotation time from Euler: " << t << endl;

		cout << "Matrix: " << MAT2 << endl;

		MAT2 = MAT;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			mat2 = mat2.inverse();
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix2x2 inversion time: " << t << endl;

		cout << "Matrix: " << mat2 << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			fmat2.getInverse(fmat2);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 inversion time: " << t << endl;

		cout << "Matrix: " << fmat2 << endl;

		fmat2 = fmat;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			mat2.invert();
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix2x2 self-inversion time: " << t << endl;

		cout << "Matrix: " << mat2 << endl;

		mat2 = mat1;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			MAT2 = DirectX::XMMatrixInverse(nullptr, MAT2);
		}
		
		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "XMMATRIX inversion time: " << t << endl;

		cout << "Matrix: " << MAT2 << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		MAT2 = MAT;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			v1 = (v1 + v2) * v2;
			v2 = (v1 - v2) / v1;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture vectors' time: " << t << endl;

		cout << "Vector1: " << v1 << endl;
		cout << "Vector2: " << v2 << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			vec1 = XMVectorMultiply(XMVectorAdd(vec1, vec2), vec2);
			vec2 = XMVectorDivide(XMVectorSubtract(vec1, vec2), vec1);
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX vectors' time: " << t << endl;

		cout << "Vector1: " << vec1 << endl;
		cout << "Vector2: " << vec2 << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		Matrix<float> m4;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			m4.clear();
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 clear time: " << t << endl;
		cout << m4 << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		Matrix<float> m1r, m2r, mr;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			mr = m1r * m2r;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 multiplication time: " << t << endl;
		cout << "Matrix: " << mr << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		auto m1d = XMMatrixIdentity(), m2d = m1d, md = m1d;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			md *= m2d;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "XMMATRIX multiplication time: " << t << endl;

		cout << "Matrix: " << md << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		float det[count];

		mr = {
			1.0f, 3.0f, 4.0f, 1.0f,
			3.0f, 1.0f, 2.0f, 4.0f,
			4.0f, 2.0f, 1.0f, 3.0f,
			0.0f, 4.0f, 3.0f, 1.0f
		};

		md = XMMatrixSet(
			1.0f, 3.0f, 4.0f, 1.0f,
			3.0f, 1.0f, 2.0f, 4.0f,
			4.0f, 2.0f, 1.0f, 3.0f,
			0.0f, 4.0f, 3.0f, 1.0f
			);

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			det[i] = mr.determinant().x;
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "Rapture matrix4x4 determinant time: " << t << endl;
		cout << det[0] << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

		last = hrc::now();

		for(register int i = 0; i < count; ++i)
		{
			det[i] = XMMatrixDeterminant(md).m128_f32[0];
		}

		t = duration_cast<nanoseconds>(hrc::now() - last).count();
		cout << "DirectX matrix4x4 determinant time: " << t << endl;
		cout << det[0] << endl;

		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;

	}
	catch(const Rapture::Exception &)
	{
		return 1;
	}

	return 0;
}

static Entrance open(load);

static void makeScreenshot(Graphics * graphics)
{
	using namespace Rapture;

	auto data = handle<ImageData>();

	graphics->surface()->requestData(data);
	ImageIO::save(initial_path() / "screenshot.bmp", data);
}

static int readNextMessage()
{
	using namespace Rapture;

	static MSG msg = {0};

	if(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		switch(msg.message)
		{
			case WM_KEYUP:
			{
				switch(msg.wParam)
				{
				case VK_SNAPSHOT:
					makeScreenshot(D3DGraphics::instance());
					break;
				case VK_ESCAPE:
					SendMessageW(msg.hwnd, WM_CLOSE, 0, 0);
					break;
				}

				break;
			}
		}

		if(msg.message == WM_QUIT)
			ThreadLoop::stop();
	}

	return 0;
}

static int render(Handle<Window> & window)
{
	std::this_thread::sleep_for(1ms);

	static int frames = 0;
	static int maxframes = 0;

	if(hrc::now() - lastSecond >= 1s)
	{
		if(frames > maxframes)
			maxframes = frames;

		cout << frames << " / " << maxframes << endl;
		frames = 0;
		lastSecond = hrc::now();
	}

	//window->invalidate();
	window->validate();
	++frames;

	return 0;
}