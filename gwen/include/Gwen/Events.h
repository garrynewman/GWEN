/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_EVENTS_H
#define GWEN_EVENTS_H

#include <list>
#include "Gwen/Exports.h"
#include "Gwen/Structures.h"
#include "Gwen/TextObject.h"
#include "Gwen/ControlList.h"

namespace Gwen
{

	namespace Controls
	{
		class Base;
	}

	namespace Event
	{
		/*

			Passed to an event hook

		*/
		struct Information
		{
			Information() { Init(); }
			Information( Gwen::Controls::Base* pctrl ) { Init(); Control = pctrl; }

			void Init()
			{
				ControlCaller	= NULL;
				Data			= NULL;
				Control			= NULL;
				Integer			= 0;
			}

			// This is set by the event caller, it will always be
			// the control which is calling the event.
			Gwen::Controls::Base*	ControlCaller;

			// This is set by the event hook
			void*					Data;

			//
			// These are set by the event and may or may not be set.
			//
			Gwen::Controls::Base*	Control;

			Gwen::ControlList		ControlList;
			Gwen::TextObject		String;
			Gwen::Point				Point;
			int						Integer;

		};

		typedef const Gwen::Event::Information & Info;


		class Caller;

		// A class must be derived from this
		class GWEN_EXPORT Handler
		{
			public:

				Handler();
				virtual ~Handler();

				void RegisterCaller( Caller* );
				void UnRegisterCaller( Caller* );

			protected:

				void CleanLinks();
				std::list<Caller*>	m_Callers;

			public:

				typedef void ( Handler::*Function )( Gwen::Controls::Base* pFromPanel );
				typedef void ( Handler::*FunctionBlank )();
				typedef void ( Handler::*FunctionWithInformation )( Gwen::Event::Info info );
				typedef void ( *GlobalFunction )( Gwen::Controls::Base* pFromPanel );
				typedef void ( *GlobalFunctionBlank )();
				typedef void ( *GlobalFunctionWithInformation )( Gwen::Event::Info info );

		};



		//
		//
		//
		class GWEN_EXPORT Caller
		{
			public:

				Caller();
				~Caller();

				void Call( Controls::Base* pThis );
				void Call( Controls::Base* pThis, Gwen::Event::Info info );

				template <typename T> void Add( Event::Handler* ob, T f ) {	AddInternal( ob, static_cast<Handler::Function>( f ) ); }
				template <typename T> void Add( Event::Handler* ob, void ( T::*f )( Gwen::Event::Info ) ) { AddInternal( ob, static_cast<Handler::FunctionWithInformation>( f ) ); }
				template <typename T> void Add( Event::Handler* ob, void ( T::*f )( Gwen::Event::Info ), void* data ) { AddInternal( ob, static_cast<Handler::FunctionWithInformation>( f ), data ); }
				template <typename T> void Add( Event::Handler* ob, void ( T::*f )() ) { AddInternal( ob, static_cast<Handler::FunctionBlank>( f ) ); }

				template <typename T> void GlobalAdd( Event::Handler* ob, T f ) { AddInternal( ob, static_cast<Handler::GlobalFunction>( f ) ); }
				void GlobalAdd( Event::Handler* ob, void ( *f )( Gwen::Event::Info ) ) { AddInternal( ob, static_cast<Handler::GlobalFunctionWithInformation>( f ) ); }
				void GlobalAdd( Event::Handler* ob, void ( *f )( Gwen::Event::Info ), void* data ) { AddInternal( ob, static_cast<Handler::GlobalFunctionWithInformation>( f ), data ); }
				void GlobalAdd( Event::Handler* ob, void ( *f )() ) { AddInternal( ob, static_cast<Handler::GlobalFunctionBlank>( f ) ); }

				void RemoveHandler( Event::Handler* pObject );

			protected:

				void CleanLinks();
				void AddInternal( Event::Handler* pObject, Handler::Function pFunction );
				void AddInternal( Event::Handler* pObject, Handler::FunctionWithInformation pFunction );
				void AddInternal( Event::Handler* pObject, Handler::FunctionWithInformation pFunction, void* data );
				void AddInternal( Event::Handler* pObject, Handler::FunctionBlank pFunction );
				void AddInternal( Event::Handler* pObject, Handler::GlobalFunction pFunction );
				void AddInternal( Event::Handler* pObject, Handler::GlobalFunctionWithInformation pFunction );
				void AddInternal( Event::Handler* pObject, Handler::GlobalFunctionWithInformation pFunction, void* data );
				void AddInternal( Event::Handler* pObject, Handler::GlobalFunctionBlank pFunction );

				struct handler
				{
					handler()
					{
						fnFunction				= NULL;
						fnFunctionInfo			= NULL;
						fnFunctionBlank			= NULL;
						fnGlobalFunction		= NULL;
						fnGlobalFunctionInfo	= NULL;
						fnGlobalFunctionBlank	= NULL;
						pObject					= NULL;
					}

					Handler::Function						fnFunction;
					Handler::FunctionWithInformation		fnFunctionInfo;
					Handler::FunctionBlank					fnFunctionBlank;
					Handler::GlobalFunction					fnGlobalFunction;
					Handler::GlobalFunctionWithInformation	fnGlobalFunctionInfo;
					Handler::GlobalFunctionBlank			fnGlobalFunctionBlank;

					Event::Handler*			pObject;
					void*					Data;
				};

				std::list<handler> m_Handlers;
		};

	}

}
#endif
