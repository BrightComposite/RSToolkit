//---------------------------------------------------------------------------

#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

//---------------------------------------------------------------------------

#include <scene/Scene.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class FpsCounter : public Object
	{
	public:
		typedef std::chrono::high_resolution_clock clock;
		typedef time_point<clock> time_marker;

		void next()
		{
			auto now = clock::now();
			++_frames;

			if(now - _last >= 1s)
			{
				_lastFrames = _frames;

				if(_lastFrames > _maxframes)
					_maxframes = _lastFrames;

				_frames = 0;
				_last = now;
			}
		}

		int get() const
		{
			return _lastFrames;
		}

	protected:
		int _frames;
		int _lastFrames;
		int _maxframes;
		time_marker _last;
	};
}

//---------------------------------------------------------------------------
#endif
