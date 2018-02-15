//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

//---------------------------------------------------------------------------

#include <meta/useif.h>
#include <core/handle.h>
#include <morph/morph.h>
#include <container/map.h>
#include <container/array_list.h>
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
            morph_origin(component)
        };
        
        template <class T, useif<is_base_of<component, T>::value>>
        morph_id_t component_id = morph_id<T>;
        
        class context
        {
            deny_copy(context);
            
        public:
            context() {}
            virtual ~context() {}
            
            template <class T>
            boost::optional<T &> component() const {
                auto i = _components.find(component_id<T>);
                return i != _components.end() ? static_cast<T &>(i.value()) : boost::optional<T &>{};
            }
            
            virtual void flush() {}
        
        protected:
            template <class Gfx>
            friend
            class driver;
            
            template <class T, class ... A>
            void register_component(A && ... args) {
                _components.try_emplace(
                    component_id<T>,
                    *this, std::forward<A>(args)...
                );
            }
            
            map<morph_id_t, gfx::component> _components;
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
                is_base_of<gfx::component, Component>::value,
                is_base_of<Component, Implementation>::value,
                can_construct<Implementation, context_type &, A...>::value
            >>
            void register_component(A && ... args) {
                auto ext = [=](context_type & ctx) {
                    ctx.register_component<Implementation>(std::forward<A>(args)...);
                };
                
                for (auto & ctx : _contexts) {
                    ext(ctx);
                }
                
                _extenders.emplace(morph_id<Component>, ext);
            }
            
            template <class Ctx, class ... A, useif <is_base_of<context_type, Ctx>::value>>
            Ctx & create_context(A && ... args) {
                return static_cast<Ctx &>(*_contexts.emplace<Ctx>(static_cast<Gfx &>(*this), std::forward<A>(args)...));
            }

        protected:
            boost::base_collection<context_type> _contexts;
            map<morph_id_t, extender> _extenders;
        };
    }
    
    using graphics = gfx::context;
    
    exception_class(graphics_exception, "Graphics exception");
    exception_subclass(context_creation_exception, graphics_exception, "Exception occurred while creating graphic context");
    exception_subclass(component_not_found_exception, graphics_exception, "Requested component was not found at given context");
    
    template <class T, useif<is_base_of<gfx::component, T>::value>>
    T & get(const gfx::context & ctx) {
        auto component = ctx.component<T>();
        
        if (component) {
            return component.get();
        }
        
        throw component_not_found_exception();
    }
}

//---------------------------------------------------------------------------
#endif
