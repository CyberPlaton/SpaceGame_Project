#pragma once

#include"Base.h"


namespace nautilus {

	namespace core {


		enum class eEventType : int;
		enum class eEventClass : int;
		std::string eventToString(eEventType eType);
		std::string eventClassToString(eEventClass eClass);


		// Queue
		template<typename T>
		class tsqueue
		{
		public:
			tsqueue() = default;
			tsqueue(const tsqueue<T>&) = delete;
			virtual ~tsqueue() { clear(); }

		public:


			// Returns and maintains item at front of Queue
			const T& getAtIndex(int i)
			{
				std::scoped_lock lock(muxQueue);

				// Silently handle out of bounds.
				i = (i < 0) ? 0 : i;
				i = (i > deqQueue.size() - 1) ? deqQueue.size() - 1 : i;

				return deqQueue[i];
			}


			// Returns and maintains item at front of Queue
			const T& front()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.front();
			}

			// Returns and maintains item after front of Queue
			const T& after_front()
			{

				std::scoped_lock lock(muxQueue);
				if (deqQueue.size() > 1) {
					return deqQueue[1];
				}
				return nullptr;
			}

			// Returns and maintains item at back of Queue
			const T& back()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.back();
			}

			// Removes and returns item from front of Queue
			T pop_front()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.front());
				deqQueue.pop_front();
				return t;
			}

			// Removes and returns item from back of Queue
			T pop_back()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.back());
				deqQueue.pop_back();
				return t;
			}

			// Adds an item to back of Queue
			void push_back(const T& item)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_back(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			// Adds an item to front of Queue
			void push_front(const T& item)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_front(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			// Returns true if Queue has no items
			bool empty()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.empty();
			}

			// Returns number of items in Queue
			size_t count()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.size();
			}

			// Clears Queue
			void clear()
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.clear();
			}

			void wait()
			{
				while (empty())
				{
					std::unique_lock<std::mutex> ul(muxBlocking);
					cvBlocking.wait(ul);
				}
			}


		protected:
			std::mutex muxQueue;
			std::deque<T> deqQueue;
			std::condition_variable cvBlocking;
			std::mutex muxBlocking;
		};
	}

}






namespace nautilus {

	namespace core {


		enum class eEventType : int {
			Invalid = -1,

			// Definitions for Events go here.
			// Like "Player_Dead" = x etc.

			Game_Started,
			Game_End,


			Player_Move_Left,
			Player_Move_Right,
			Player_Move_Down,
			Player_Move_Up,
			Player_Turn_Left,
			Player_Turn_Right,
			Player_Action_Shoot,
			Player_Action_Activate_Shield,
			Player_Action_Activate_Special_Ability,
		};


		enum class eEventClass : int {
			Invalid = -1,

			Application_Event,
			Network_Event,
			InputDevice_Pressed_Event,
			InputDevice_Held_Event,
			InputDevice_Released_Event,

			Audio_Event,
			Graphics_Event,
		};


		
		struct IEvent {

			// Whether this event was created by a game entity.
			virtual bool wasDispatchedByEntity() const = 0;

			// At which system time this event happened.
			virtual float getTimeStamp() const = 0;

			// Return the entt::entity of the entity
			// who created this event.
			virtual entt::entity getDispatcherEntity() const = 0;

			// Retrieve the event type.
			virtual eEventType getEventType() const = 0;

			// Retrieve class of the event.
			virtual eEventClass getEventClass() const = 0;

			// Whether this Event and another are the "same"...
			virtual bool operator==(const IEvent* rhs) = 0;



		};


		class Event : public IEvent{
		public:

			Event(float timeStamp, eEventType eType, eEventClass eClass, entt::entity entity, void* data, std::string datatype) : m_TimeStamp(timeStamp),
																			m_EventType(eType), m_AssociatedEntityHandle(entity), m_EventClass(eClass),
																			m_Data(data), m_DataDatatype(datatype) {}
			

			Event(float timeStamp, eEventType eType, eEventClass eClass, entt::entity entity) : m_TimeStamp(timeStamp),
				m_EventType(eType), m_AssociatedEntityHandle(entity), m_EventClass(eClass) {}


			Event(float timeStamp, eEventType eType, eEventClass eClass) : m_TimeStamp(timeStamp), m_EventType(eType), m_EventClass(eClass) {}
			virtual ~Event(){}


			virtual eEventClass getEventClass() const override { return m_EventClass; }
			virtual bool wasDispatchedByEntity() const override { return m_AssociatedEntityHandle != entt::null; }
			virtual float getTimeStamp() const override { return m_TimeStamp; }
			virtual entt::entity getDispatcherEntity() const override { return m_AssociatedEntityHandle; }
			virtual bool operator==(const IEvent* rhs) override { return (this==rhs); }
			virtual eEventType getEventType() const override { return m_EventType; }



			// Example of how to transfer data in Event.
			// For this create an object of needed type and
			// get the address to the location of that object (void*).
			// 
			// We send data as locations in memory!
			// To know what data type the address holds, append a string describing it,
			// e.g. datatype = "string" 
			// or datatype = "vec2"
			// etc.
			//
			//void* data = static_cast<void*>(new std::string("Hello darkness, my friend!"));
			//std::string datatype = "string";
			//
			// or
			//
			// These 2 are both valid:
			// void* data = static_cast<void*>(new glm::vec2(-1.0f, 1.0f));
			// data = new glm::vec2(1265.0f, -1123.18f);
			//
			// std::string datatype = "vec2";
			//
			// EventDispatcher::dispatchEvent(11.9f, eEventType::Unit_Created, eEventClass::Audio_Event, entt::entity(0), data, datatype);
			//
			// It is also perfectly valid to transport other objects, e.g. Components like ComponentScript:
			// E.g.: to dispatch the event..
			// void* data = new ComponentScript();
			// std::string datatype = "ComponentScript";
			// 
			// and in some event handler...
			// ComponentScript* pData = userEvent->getData< ComponentScript >();
			// pData->init("simpleScript.lua");
			//
			// 
			// In the event handler, which will get the event with the dispatched data, must anticipate the datatype OR
			// check for each of them and then take defined steps.
			// E.g.: we check if the datatype of the incoming Event Data is an Integer.
			// 
			// if (COMPARE_STRINGS(userEvent->getDataDatatype(), "int") == 0) ... do something ...
			//
			//
			//
			//
			// Dispatching an Event with data should be done carefully,
			// as converting from void* to another datatype could lead to runtime errors or
			// even worse "undefined behaviour"...
			//
			template < typename T > 
			T* getData() const { return static_cast<T*>(m_Data); }

			std::string getDataDatatype() const { return m_DataDatatype; }

			// Convenience functions for securely converting an Events data to a 
			// certain data type.
			// "isEventDataOfDatatype" checks if data is of a given type and
			// "geEventDataAs" returns the data with given type (unsecured).
			// 
			// Make sure to test for a type before converting!
			//
			static bool isEventDataOfDatatype(std::string dataType, Event* eEvent) {

				return(COMPARE_STRINGS_2(dataType, eEvent->getDataDatatype()) == 0);
			}

			template < typename T >
			static T geEventDataAs(Event* eEvent) {
				return(*eEvent->getData<T>());
			}


		protected:

			eEventClass m_EventClass;
			eEventType m_EventType;
			
			float m_TimeStamp;
			entt::entity m_AssociatedEntityHandle;


			void* m_Data; // Pointer to a memory address which holds the data.
			std::string m_DataDatatype; // A string specifying the type of m_Data.
		};





		struct IEventDelegate {

			virtual void execute(Event* userEvent) = 0;

			virtual bool operator==(IEventDelegate* rhs) = 0;

			virtual void operator()(Event* userEvent) = 0;

			virtual std::string getDebugName() = 0;
		};


		struct BaseEventDelegate : public IEventDelegate {

			void execute(Event* userEvent) {}

			bool operator==(IEventDelegate* rhs) {

				// Check if correct...
				return *this == rhs;
			}

			void operator()(Event* userEvent) {
				execute(userEvent);
			}

			std::string getDebugName() {
				return m_DebugName;
			}


			std::string m_DebugName = "BaseEventDelegate";
		};





		class EventManager {

			// Store associated delegate function for each define event type.
			typedef std::map < eEventType, std::vector<IEventDelegate*> > EventTypeListenerMap;
			typedef std::map < eEventClass, std::vector<IEventDelegate*> > EventClassListenerMap;


		public:
			

			static EventManager* get() {
				if (!g_pEventManager) {
					g_pEventManager = new EventManager();
				}

				return g_pEventManager;
			}


			static void del() {
								
				delete g_pEventManager;
			}


			// Not implemented.
			void queueEvent(Event* userEvent) {

				std::scoped_lock lock(muxQueue);

				// Push new event to queue which is not active.
				if (m_ActiveQueue == 0) {
					m_TSQueue[1].push_back(userEvent);
				}
				else {
					m_TSQueue[0].push_back(userEvent);
				}
				
			}



			void update(float maxUpdateTime) {

				// Make sure only one thread at a time runs the update.
				if (m_UpdateThreadRunning) return;


				// Launch separate update thread.
				// Update time not implemented.
				std::thread updateThread(&EventManager::_update, this, 0.0f);
				updateThread.detach();
			}




			void addDelegate(eEventType nEvent, IEventDelegate* pDelegate) {

				std::scoped_lock lock(muxQueue);


				// Try find if event type already exists.
				for (auto it = m_EventTypeListenerMapping.begin(); it != m_EventTypeListenerMapping.end(); it++) {

					if (it->first == nEvent) {

						// Means a type already exists, thus push back to its associated vector.
						it->second.push_back(pDelegate);
						return;
					}
				}

				// Else we have not mapped this event type, this create a mapping and then push back to vector.
				std::vector<IEventDelegate*> vec; vec.push_back(pDelegate);
				m_EventTypeListenerMapping.emplace(nEvent, vec);
			}



			void addDelegate(eEventClass eClass, IEventDelegate* pDelegate) {

				std::scoped_lock lock(muxQueue);


				// Try find if event type already exists.
				for (auto it = m_EventClassListenerMapping.begin(); it != m_EventClassListenerMapping.end(); it++) {

					if (it->first == eClass) {

						// Means a type already exists, thus push back to its associated vector.
						it->second.push_back(pDelegate);
						return;
					}
				}

				// Else we have not mapped this event type, this create a mapping and then push back to vector.
				std::vector<IEventDelegate*> vec; vec.push_back(pDelegate);
				m_EventClassListenerMapping.emplace(eClass, vec);
			}



			// Not implemented.
			void removeDelegate() {

				std::scoped_lock lock(muxQueue);

			}



		private:
			EventManager() = default;


			static EventManager* g_pEventManager;

			bool m_UpdateThreadRunning = false;

			// Store events from application in a thread safe manner.
			tsqueue<Event*> m_TSQueue[2];
			uint32_t m_ActiveQueue = 0;


			std::mutex muxQueue; // Make class Thread safe.


			EventTypeListenerMap m_EventTypeListenerMapping;
			EventClassListenerMap m_EventClassListenerMapping;


		private:


			void _update(float maxTime) {

				m_UpdateThreadRunning = true;
				std::scoped_lock lock(muxQueue);


				// Get the active queue index.
				m_ActiveQueue = (m_ActiveQueue == 0) ? 1 : 0;

				// Do nothing if queue is empty.
				if (m_TSQueue[m_ActiveQueue].count() == 0) {
					m_UpdateThreadRunning = false;
					return;
				}			


				using namespace std;
				cout << color(colors::DARKYELLOW);
				cout << "EventManager::_update	--	Running updates. \n" << white;



				// Go through all events in queue..
				for (int i = 0; i < m_TSQueue[m_ActiveQueue].count(); i++) {


					/*
					cout << color(colors::YELLOW);
					cout << "Job: \"Remove Event Pollution.\"." << white << endl;
					while (true) {

						// Reset.
						Event* nowEvent = nullptr;
						Event* afterEvent = nullptr;

						// Do cleaning.
						nowEvent = m_TSQueue[m_ActiveQueue].front();
						afterEvent = m_TSQueue[m_ActiveQueue].after_front();
						if (nowEvent == nullptr || afterEvent == nullptr) break;

						if (nowEvent->getEventType() == afterEvent->getEventType()) {

							// By definition,
							// 2 Events are equal, if they are of the same type..
							//
							// This definition is not "perfect" and should be reworked at some time.
							// By now, I really dont know when 2 Event should be "the same"...
							//
							//
							// If 2 Events are the same, just remove one and let the second be.
							// We check for each of them for same condition.
							m_TSQueue[m_ActiveQueue].pop_front();
						}
						else {
							break;
						}
					}
					*/



					auto pEvent = m_TSQueue[m_ActiveQueue].pop_front(); // Get next event.

					// Find vector for event type.
					for (auto it = m_EventTypeListenerMapping.begin(); it != m_EventTypeListenerMapping.end(); it++) {

						eEventType eType = pEvent->getEventType();
						if (it->first == eType) {

							// Found needed vector.
							// Fire off all delegate excute functions of it..
							for (auto pDelegate : it->second) {

								pDelegate->execute(pEvent);
							}
						}

					}


					// Make the same for general event class listeners...
					for (auto it = m_EventClassListenerMapping.begin(); it != m_EventClassListenerMapping.end(); it++) {


						eEventClass eClass = pEvent->getEventClass();

						if (it->first == eClass) {

							for (auto pDelegate : it->second) {

								pDelegate->execute(pEvent);
							}

						}
					}

				}


				// After events where cycled through,
				// empty the event queue.
				m_TSQueue[m_ActiveQueue].clear();
				m_UpdateThreadRunning = false;
				return;
			}


		};




		class EventDispatcher {
		public:

			// Dispatch an event that is associated with an entity.
			static void dispatchEvent(float timeStamp, eEventType eType, eEventClass eClass, entt::entity entity);

			static void dispatchEvent(float timeStamp, eEventType eType, eEventClass eClass, entt::entity entity, void* data, std::string datatype);

			// Dispatch a general event not associated with an entity.
			static void dispatchEvent(float timeStamp, eEventType eType, eEventClass eClass);
		};


		class DelegateRegister {
		public:

			// Register a delegate function for a specific event type.
			static void registerDelegate(eEventType eType, IEventDelegate* eDelegate);


			// Register a delegate for all of event types.
			static void registerDelegate(eEventClass eClass, IEventDelegate* eDelegate);

		};
	}
}
