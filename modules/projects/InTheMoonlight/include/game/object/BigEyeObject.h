//---------------------------------------------------------------------------

#ifndef BIG_EYE_OBJECT_H
#define BIG_EYE_OBJECT_H

//---------------------------------------------------------------------------

#include <scene/Scene.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	link_class(BigEyeObject, MetaClass<SceneObject, Drawable>);

	class BigEyeObject : public SceneObject, public Drawable
	{
	public:
		BigEyeObject(Scene * scene) : SceneObject(scene), Drawable(scene)
		{
			setclass(BigEyeObject);
		}

		virtual ~BigEyeObject() {}

	protected:
		virtual void update(ticks_t ticks) override
		{
			/*
			static std::default_random_engine rndm;
			static std::bernoulli_distribution dist;
			static auto isBlinking = std::bind(dist, rndm);

			if(timer == 720 || timer == 0)
			{
			if(blinking || ticks % 256 == 0)
			blinking = isBlinking() && isBlinking();

			timer = 0;
			eyeScale = 1.0f;
			}

			if(blinking)
			eyeScale = 0.5f * (FloatMath::cos(FloatMath::dtor(float(++timer) * 0.5f)) + 1.0f);
			*/
		}

		virtual void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const override
		{
			auto dt = hold(graphics->depthTestModeState(), false);
			auto lw = hold(graphics->lineSizeState(), 3);
			auto fm = hold(graphics->fillModeState(), FillMode::Solid);

			const FloatPoint zero {0.0f, 0.0f};
			const auto eyeZoom = zoom * 15;
			const float eyeMin = 0.1f;

			//FloatSize eyeSize {eyeZoom * 1.1f, eyeZoom * std::min(std::max(eyeScale, eyeMin), 0.6f) * 1.1f};
			//SqRect eyeOuter {centeredRect(zero, FloatSize{eyeZoom * 2.0f, eyeZoom * std::min(std::max(eyeScale * 0.6f + 0.4f, eyeMin * 5), 0.8f) * 2.0f})};

			graphics->setColor({0.0f, 0.0f, 0.0f});
			//graphics->ellipse(centeredRect(zero, eyeSize));
			graphics->rectangle(SqRect {centeredSquare(zero, 2.0f)});

			static const int irisAcc = 200;
			/*
			for(int i = 0; i < irisAcc; ++i)
			{
			float r = float(i) / (irisAcc - 1);
			float h = 1.0f - r * r;
			graphics->setColor({h, h, h});
			graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (0.7f - 0.1f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float a = std::abs(r * 2.0f - 1.0f);
				float h = 1.0f - a * a * a;

				graphics->setColor({0.7f * h, 0.0f, 0.1f * h});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (2.4f - 0.2f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float a = std::abs(r * 2.0f - 1.0f);
				float h = 1.0f - a * a * a;

				graphics->setColor({0.8f * h, 0.0f, 0.5f * h});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (2.0f - 0.2f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float a = std::abs(r * 2.0f - 1.0f);
				float h = 1.0f - a * a * a;

				graphics->setColor({0.8f * h, 0.0f, 0.8f * h});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (1.6f - 0.2f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float a = std::abs(r * 2.0f - 1.0f);
				float h = 1.0f - a * a * a;

				graphics->setColor({0.6f * h, 0.0f, 0.9f * h});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (1.2f - 0.2f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float a = std::abs(r * 2.0f - 1.0f);
				float h = 1.0f - a * a * a;

				graphics->setColor({0.4f * h, 0.0f, 0.9f * h});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (0.8f - 0.2f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float a = std::abs(r * 2.0f - 1.0f);
				float h = 1.0f - a * a * a;

				graphics->setColor({0.0f, 0.0f, 1.0f * h});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (0.4f - 0.2f * r))});
			}

			graphics->setColor({0.0f, 0.0f, 0.0f});
			graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * 0.4f)});

			fm = FillMode::Wireframe;
			lw = int(eyeZoom * viewport.width() * 0.5f);

			graphics->setColor({0.0f, 0.0f, 0.0f});
			graphics->ellipse(eyeOuter);

			lw = 3;

			for(int i = 0; i < irisAcc; ++i)
			{
			float ratio = float(i) / (irisAcc - 1);
			float h = FloatMath::sin(ratio * FloatMath::pi);

			graphics->setColor({0.12f * (0.5f + 1.0f * h), 0.0f, 0.2f * (0.5f + 1.0f * h), h});

			float d = ratio * 0.1f * eyeSize.x;
			graphics->ellipse(centeredRect(zero, {eyeSize.x - d, eyeSize.y - d}));
			}
			*/

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float h = r * r;

				graphics->setColor({1.0f * h, 0.6f * h, 0.0f});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (2.4f - 0.2f * r))});
			}

			for(int i = 0; i < irisAcc; ++i)
			{
				float r = float(i) / (irisAcc - 1);
				float h = r * r;

				graphics->setColor({1.0f, 0.6f + 0.4f * h, 0.0f});
				graphics->ellipse(SqRect {centeredSquare(zero, eyeZoom * (1.2f - 0.2f * r))});
			}
		}

		int timer = 0;
		bool blinking = false;
		float eyeScale = 1.0f;
	};
}

//---------------------------------------------------------------------------
#endif
