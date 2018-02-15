//---------------------------------------------------------------------------

#include <opengl/uniform_layout.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace opengl
    {
        void uniform_layout::add(const uniform_scheme & e)
        {
            _elements.emplace_back(_current_offset, e.type);
            _indices.insert({ e.name, _elements.size() - 1 });
        }

        void uniform_layout::add(uniform_scheme && e)
        {
            _elements.emplace_back(_current_offset, e.type);
            _indices.insert({ std::move(e.name), _elements.size() - 1 });
        }
    }
}

//---------------------------------------------------------------------------

