#include"EventSystem.h"

namespace nautilus {

	namespace core {


		EventManager* EventManager::g_pEventManager = nullptr;

		std::string eventToString(eEventType eType) {

			std::string ret;

			switch (eType) {
			default:
				ret = "Invalid";
				break;
			}


			return ret;
		}




		std::string eventClassToString(eEventClass eClass) {

			std::string ret;

			switch (eClass) {
			default:
				ret = "Invalid";
				break;

			case eEventClass::Application_Event:
				ret = "Application_Event";
				break;

			case eEventClass::Network_Event:
				ret = "Network_Event";
				break;

			case eEventClass::InputDevice_Pressed_Event:
				ret = "InputDevice_Pressed_Event";
				break;

			case eEventClass::InputDevice_Held_Event:
				ret = "InputDevice_Held_Event";
				break;

			case eEventClass::InputDevice_Released_Event:
				ret = "InputDevice_Released_Event";
				break;

			case eEventClass::Audio_Event:
				ret = "Audio_Event";
				break;

			case eEventClass::Graphics_Event:
				ret = "Graphics_Event";
				break;
			}


			return ret;
		}


		// Dispatch an event that is associated with an entity.
		void EventDispatcher::dispatchEvent(float timeStamp, eEventType eType, eEventClass eClass, entt::entity entity) {

			EventManager::get()->queueEvent(new Event(timeStamp, eType, eClass, entity));
		}



		void EventDispatcher::dispatchEvent(float timeStamp, eEventType eType, eEventClass eClass, entt::entity entity, void* data, std::string datatype) {

			EventManager::get()->queueEvent(new Event(timeStamp, eType, eClass, entity, data, datatype));
		}

		// Dispatch a general event not associated with an entity.
		void EventDispatcher::dispatchEvent(float timeStamp, eEventType eType, eEventClass eClass) {

			EventManager::get()->queueEvent(new Event(timeStamp, eType, eClass));
		}




		// Register a delegate function for a specific event type.
		void DelegateRegister::registerDelegate(eEventType eType, IEventDelegate* eDelegate) {

			EventManager::get()->addDelegate(eType, eDelegate);
		}


		// Register a delegate for all of event types.
		void DelegateRegister::registerDelegate(eEventClass eClass, IEventDelegate* eDelegate) {

			EventManager::get()->addDelegate(eClass, eDelegate);
		}

	}
}
