//---------------------------------------------------------------------------

#include <opengl/vertex_layout.h>
#include <container/map.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		namespace vertex_layouts 
		{
			asd::map<std::string, const vertex_layout *> map;

			struct Iterator
			{
				template<class T>
				static void iterate() {
					map.insert_or_assign(T::key(), &T::get());
				}
			};

			storage::storage() {
				foreach_t<p2, p3, p2c3, p3c3, p2c4, p3c4, p2t, p3t>
					::iterate<Iterator>();
			}

			const vertex_layout & storage::get(const std::string & key) {
				return *map.at(key);
			}
		}
	}
}

//---------------------------------------------------------------------------
