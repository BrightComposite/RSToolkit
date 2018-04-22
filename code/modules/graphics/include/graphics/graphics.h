//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

//---------------------------------------------------------------------------

#include <meta/useif.h>
#include <core/handle.h>
#include <meta/class_id.h>
#include <container/map.h>
#include <container/array_list.h>
#include <container/any_list.h>
#include <boost/poly_collection/base_collection.hpp>
#include <boost/optional.hpp>

#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace asd
{
    class window;
    
    namespace gfx
    {
        class component
        {
            origin_class(component);

        public:
            virtual ~component() {}
        };
        
        template <class T>
        const class_id_t & component_id = class_id<T>;
        
        class context
        {
            deny_copy(context);
            
        public:
            context() {}
            virtual ~context() {}
            
            template <class T>
            boost::optional<T &> component() {
                auto i = _offsets.find(component_id<T>);
                return i != _offsets.end() ? _components.get_by_offset<T>(i->second) : boost::optional<T &>{};
            }
            
            virtual void flush() {}
        
        protected:
            template <class Gfx>
            friend class driver;
            
            template <class Context, class Component, class Implementation, class ... A>
            void register_component(A && ... args) {
                _components.emplace_back<Implementation>(static_cast<Context &>(*this), std::forward<A>(args)...);
                _offsets.try_emplace(component_id<Component>, _components.offset(_components.size() - 1));
            }

            map<class_id_t, size_t> _offsets;
            any_list _components;
        };
        
        template <class Gfx>
        class driver_context : public context {};
        
        template <class Gfx>
        class driver
        {
            deny_copy(driver);
        
        public:
            using context_type = driver_context<Gfx>;
            using extender = function<void(context_type &)>;
            
            driver() {}
            
            template <class Component, class Implementation, class ... A, useif<
                is_base_of<Component, Implementation>::value,
                can_construct<Implementation, context_type &, A...>::value
            >>
            void register_component(A && ... args) {
                auto ext = [=](context_type & ctx) {
                    ctx.register_component<context_type, Component, Implementation>(std::forward<A>(args)...);
                };
                
                for (auto & ctx : _contexts) {
                    ext(ctx);
                }
                
                _extenders.emplace_back(std::move(ext));
            }
            
            template <class Ctx, class ... A, useif <is_base_of<context_type, Ctx>::value>>
            Ctx & create_context(A && ... args) {
                Ctx & context = static_cast<Ctx &>(*_contexts.emplace<Ctx>(static_cast<Gfx &>(*this), std::forward<A>(args)...));

                for (auto & extender : _extenders) {
                    extender(context);
                }

                return context;
            }

        protected:
            boost::base_collection<context_type> _contexts;
            asd::array_list<extender> _extenders;
        };
    }
    
    using graphics = gfx::context;
    
    exception_class(graphics_exception,
        "Graphics exception occured");
    exception_subclass(context_creation_exception, graphics_exception,
        "An exception occurred while creating a graphic context");
    exception_subclass(component_not_found_exception, graphics_exception,
        "The requested component was not found in the given context");
    
    template <class T, useif<is_base_of<gfx::component, T>::value>>
    T & get(gfx::context & ctx) {
        auto component = ctx.component<T>();
        
        if (component) {
            return component.get();
        }
        
        throw component_not_found_exception();
    }
}

//---------------------------------------------------------------------------
#endif
