//---------------------------------------------------------------------------

#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

//---------------------------------------------------------------------------

#include <physics/Physics.h>
#include <physics/PlaneObject.h>

#include <random>
#include <deque>

//---------------------------------------------------------------------------

namespace Rapture
{
	class PlayerObject;

	using std::deque;

	class Contact
	{
		friend class Contacter;

	public:
		Contact(DoubleVector pos, double force) : _pos {pos}, _force(std::min(1.0, force)) {}

	protected:
		void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const
		{
			auto pos = FloatPoint {_pos.x, _pos.y} *FloatPoint {zoom, zoom};
			graphics.setColor(1.0f, 1.0f, 1.0f, float((1.0f - float(_ticks) / _maxTicks) * _force));
			graphics.ellipse(SqRect {centeredSquare(pos, zoom * float(_ticks) * float(std::min(_force, 0.5)) * 0.04f)});
		}

		DoubleVector _pos;

		double _force = 1.0f;
		int _ticks = 0;
		int _maxTicks = 100;
	};

	class Contacter : public DrawableObject
	{
	public:
		Contacter(Scene * scene) : DrawableObject(scene) {}

		void addContact(DoubleVector pos, double force)
		{
			_contacts.emplace_back(pos, force);
		}

		int contactsCount() const
		{
			return int(_contacts.size());
		}

	protected:
		virtual void update(ticks_t ticks) override
		{
			if(ticks % 4 != 0 || _contacts.size() == 0)
				return;

			auto & first = _contacts[0];

			if(first._ticks >= first._maxTicks)
				_contacts.pop_front();

			for(auto & contact : _contacts)
				contact._ticks += 1;
		}

		virtual void draw(Graphics3D & graphics, const IntRect & viewport, float zoom) const override
		{
			auto dt = hold(graphics.depthTestModeState(), false);
			auto lw = hold(graphics.lineWidthState(), 2);
			auto fm = hold(graphics.fillModeState(), FillMode::Wireframe);

			for(auto & contact : _contacts)
				contact.draw(graphics, viewport, zoom);
		}

		deque<Contact> _contacts;
	};

	link_class(basicgame, PlayerObject, Class<SceneObject>);

	class PlayerObject :
		public SceneObject
	{
		friend class PlayerController;

	public:
		PlayerObject(Scene * scene) :
			SceneObject(scene),
			_camera(scene)
		{
			setclass(PlayerObject);

			_camera->position.z = 0.0f;
			_camera->update();
		}

		virtual ~PlayerObject() {}

	protected:
		virtual void update(ticks_t ticks) override
		{
			if(_keyMap.isPressed('W'))
			{
				_camera->moveZ(0.005f);
			}

			if(_keyMap.isPressed('S'))
			{
				_camera->moveZ(-0.005f);
			}

			if(_keyMap.isPressed('A'))
			{
				_camera->moveX(-0.005f);
			}

			if(_keyMap.isPressed('D'))
			{
				_camera->moveX(0.005f);
			}

			if(_keyMap.isPressed('Q'))
			{
				_camera->direction.rotateY(fmath::dtor(+0.4f));
			}

			if(_keyMap.isPressed('E'))
			{
				_camera->direction.rotateY(fmath::dtor(-0.4f));
			}

			_camera->update();
		}

		//Handle<Contacter> _contacter;
		//Handle<GraphicModel> _model;

		//bool _hasFoothold = false;

		Handle<Camera> _camera;
		KeyMap _keyMap;
	};

	class PlayerController : public Object
	{
	public:
		PlayerController(PlayerObject * object) : object(object)
		{
			connect(object->scene()->space(), this, &PlayerController::onKeyDown);
			connect(object->scene()->space(), this, &PlayerController::onKeyUp);
		}

		virtual ~PlayerController()
		{
			disconnect(object->scene()->space(), this, &PlayerController::onKeyDown);
			disconnect(object->scene()->space(), this, &PlayerController::onKeyUp);
		}

	protected:
		void onKeyDown(Handle<KeyDownMessage> & message, UISpace & dest)
		{
			if(message->isFirst)
				object->_keyMap.press(message->key);
		}

		void onKeyUp(Handle<KeyUpMessage> & message, UISpace & dest)
		{
			object->_keyMap.unpress(message->key);
		}

		PlayerObject * object;
	};
}

//---------------------------------------------------------------------------
#endif
