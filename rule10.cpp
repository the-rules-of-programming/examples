
#include <vector>
#include <string>
#include <assert.h>
#include "geom.h"
#include "tests.h"

using namespace std;

#pragma warning(disable: 4244)

namespace rule10
{
	namespace x1
	{
		using namespace geom;

		const float pi = 3.1415927f;

		class Color
		{
		};

		void drawLine(Point fromPoint, Point toPoint, Color color)
		{
		}

		void drawCircle(Point center, float radius, Color color)
		{
			int count = int(ceil(pi / acos((radius - 1.0) / radius)));
			Point previousPoint = center + Vector(radius, 0.0, 0.0);
			for (int index = 1; index <= count; ++index)
			{
				float angle = 2.0 * pi * index / count;
				Point nextPoint = center + 
								  radius * Vector(cosf(angle), sinf(angle), 0.0);

				drawLine(previousPoint, nextPoint, color);

				previousPoint = nextPoint;
			}
		}
	};

	namespace x2
	{
		struct Date
		{
			int m_ordinal;
		};

		Date getCurrentDate()
		{
			return { 0 };
		}

		bool operator >= (Date date, Date otherDate)
		{
			return date.m_ordinal >= otherDate.m_ordinal;
		}

		bool operator <= (Date date, Date otherDate)
		{
			return date.m_ordinal <= otherDate.m_ordinal;
		}

		struct Customer
		{
			int m_customerID;
			string m_firstName;
			string m_lastName;
			Date m_lastPurchase;
			Date m_validFrom;
			Date m_validUntil;
			bool m_isClosed;
		};

	};

	namespace x3
	{
		using namespace x2;

		void findRecentPurchasers(
			const vector<Customer *> & customers,
			Date startingDate,
			vector<Customer *> * recentCustomers)
		{
			Date currentDate = getCurrentDate();

			for (Customer * customer : customers)
			{
				if (customer->m_validFrom >= currentDate &&
					customer->m_validUntil <= currentDate &&
					!customer->m_isClosed &&
					customer->m_lastPurchase >= startingDate)
				{
					recentCustomers->push_back(customer);
				}
			}
		}
	};

	namespace x4
	{
		using namespace x2;

		void findRecentPurchasers(
			const vector<Customer *> & validCustomers,
			Date startingDate,
			vector<Customer *> * recentCustomers)
		{
			Date currentDate = getCurrentDate();

			for (Customer * customer : validCustomers)
			{
				if (customer->m_lastPurchase >= startingDate)
				{
					recentCustomers->push_back(customer);
				}
			}
		}
	};

	namespace x5
	{
		class Character
		{
		};

		class AwarenessEvents
		{
		public:

			virtual void onSpotted(Character * otherCharacter);
			virtual void onLostSight(Character * otherCharacter);
		};

		class AwarenessManager
		{
		public:

			int getSpottedCount(Character * character);
			void subscribe(Character * character, AwarenessEvents * events);
			void unsubscribe(Character * character, AwarenessEvents * events);

			//

			AwarenessManager() :
				m_records()
				{ ; }

			struct Record
			{
				Character * m_character;
				vector<Character *> m_spottedCharacters;
				vector<AwarenessEvents *> m_subscribedEvents;
			};

			vector<Record> m_records;

			Record & ensureRecord(
				Character * character);
			void markVisibility(
				Character * character, 
				Character * otherCharacter, 
				bool isVisible);
		};

		class EyeIcon
		{
		public:

			bool isOpen() const;
			void open();
			void close();

			//

			EyeIcon() :
				m_isOpen(false)
				{ ; }

			bool m_isOpen;
		};

		//

		void AwarenessEvents::onSpotted(Character * otherCharacter)
		{
		}

		void AwarenessEvents::onLostSight(Character * otherCharacter)
		{
		}

		int AwarenessManager::getSpottedCount(Character * character)
		{
			return ensureRecord(character).m_spottedCharacters.size();
		}

		void AwarenessManager::subscribe(Character * character, AwarenessEvents * events)
		{
			ensureRecord(character).m_subscribedEvents.push_back(events);
		}

		void AwarenessManager::unsubscribe(Character * character, AwarenessEvents * events)
		{
			Record & record = ensureRecord(character);
			auto iter = find(
							record.m_subscribedEvents.begin(), 
							record.m_subscribedEvents.end(), 
							events);
			assert(iter != record.m_subscribedEvents.end());

			record.m_subscribedEvents.erase(iter, iter + 1);
		}

		AwarenessManager::Record & AwarenessManager::ensureRecord(
			Character * character)
		{
			auto iter = find_if(
							m_records.begin(),
							m_records.end(),
							[character](const Record & record) { return record.m_character == character; });
			if (iter != m_records.end())
				return *iter;

			Record record;
			record.m_character = character;

			m_records.push_back(record);
			return m_records.back();
		}

		void AwarenessManager::markVisibility(
			Character * character, 
			Character * otherCharacter, 
			bool isVisible)
		{
			for (int i = 0; i < 2; ++i)
			{
				Record & record = ensureRecord(character);
				auto iter = find(
								record.m_spottedCharacters.begin(), 
								record.m_spottedCharacters.end(), 
								otherCharacter);

				if (iter == record.m_spottedCharacters.end())
				{
					if (isVisible)
					{
						record.m_spottedCharacters.push_back(otherCharacter);

						for (AwarenessEvents * events : record.m_subscribedEvents)
						{
							events->onSpotted(otherCharacter);
						}
					}
				}
				else
				{
					if (!isVisible)
					{
						record.m_spottedCharacters.erase(iter, iter + 1);

						for (AwarenessEvents * events : record.m_subscribedEvents)
						{
							events->onLostSight(otherCharacter);
						}
					}
				}

				swap(character, otherCharacter);
			}
		}

		bool EyeIcon::isOpen() const
		{
			return m_isOpen;
		}

		void EyeIcon::open()
		{
			m_isOpen = true;
		}

		void EyeIcon::close()
		{
			m_isOpen = false;
		}

		AwarenessManager g_awarenessManager;
		EyeIcon g_eyeIcon;

		AwarenessManager * getAwarenessManager()
		{
			return &g_awarenessManager;
		}

		EyeIcon * getEyeIcon()
		{
			return &g_eyeIcon;
		}
	};

	namespace x6
	{
		using namespace x5;

		class Player : public Character, public AwarenessEvents
		{
		public:
    
			Player();

			void onSpotted(Character * otherCharacter) override;
			void onLostSight(Character * otherCharacter) override;

		protected:

			int m_spottedCount;
		};

		Player::Player() :
			m_spottedCount(getAwarenessManager()->getSpottedCount(this))
		{
			if (m_spottedCount == 0)
				getEyeIcon()->close();

			getAwarenessManager()->subscribe(this, this);
		}

		void Player::onSpotted(Character * otherCharacter)
		{
			if (m_spottedCount == 0)
				getEyeIcon()->open();

			++m_spottedCount;
		}

		void Player::onLostSight(Character * otherCharacter)
		{
			--m_spottedCount;

			if (m_spottedCount == 0)
				getEyeIcon()->close();
		}
	};

	namespace x7
	{
		using namespace x5;

		enum class STATUS
		{
			Normal,
			Blindfolded  
		};

		class Player : public Character, public AwarenessEvents
		{
		public:
    
			Player();

			void setStatus(STATUS status);

			void onSpotted(Character * otherCharacter) override;
			void onLostSight(Character * otherCharacter) override;

		protected:

			STATUS m_status;
			int m_spottedCount;
		};

		Player::Player() :
			m_status(STATUS::Normal),
			m_spottedCount(getAwarenessManager()->getSpottedCount(this))
		{
			if (m_spottedCount == 0)
				getEyeIcon()->close();

			getAwarenessManager()->subscribe(this, this);
		}

		void Player::setStatus(STATUS status)
		{
			if (status == m_status)
				return;

			if (m_spottedCount == 0)
			{
				if (status == STATUS::Normal)
					getEyeIcon()->close();
				else if (m_status == STATUS::Normal)
					getEyeIcon()->open();
			}

			m_status = status;
		}

		void Player::onSpotted(Character * otherCharacter)
		{
			if (m_spottedCount == 0 && m_status == STATUS::Normal)
				getEyeIcon()->open();

			++m_spottedCount;
		}

		void Player::onLostSight(Character * otherCharacter)
		{
			--m_spottedCount;

			if (m_spottedCount == 0 && m_status == STATUS::Normal)
				getEyeIcon()->close();
		}

	};

	namespace x8
	{
		using namespace x5;

		enum class WEATHER
		{
			Clear,
			Foggy
		};

		class WeatherEvents
		{
		public:

			virtual void onWeatherChanged(WEATHER oldWeather, WEATHER newWeather);
		};

		class WeatherManager
		{
		public:

			WEATHER getCurrentWeather() const;
			void subscribe(WeatherEvents * events);

			//

			WeatherManager() :
				m_currentWeather(WEATHER::Clear),
				m_subscribedEvents()
				{ ; }

			void setCurrentWeather(WEATHER weather);

			WEATHER m_currentWeather;
			vector<WeatherEvents *> m_subscribedEvents;
		};

		//

		void WeatherEvents::onWeatherChanged(WEATHER oldWeather, WEATHER newWeather)
		{
		}

		WEATHER WeatherManager::getCurrentWeather() const
		{
			return m_currentWeather;
		}

		void WeatherManager::setCurrentWeather(WEATHER weather)
		{
			if (weather == m_currentWeather)
				return;

			WEATHER oldWeather = m_currentWeather;
			m_currentWeather = weather;

			for (WeatherEvents * events : m_subscribedEvents)
			{
				events->onWeatherChanged(oldWeather, weather);
			}
		}

		void WeatherManager::subscribe(WeatherEvents * events)
		{
			m_subscribedEvents.push_back(events);
		}

		WeatherManager g_weatherManager;

		WeatherManager * getWeatherManager()
		{
			return &g_weatherManager;
		}
	};

	namespace x9
	{
		using namespace x8;

		enum class STATUS
		{
			Normal,
			Blindfolded  
		};

		//

		class Player : 
			public Character, 
			public AwarenessEvents, 
			public WeatherEvents
		{
		public:
    
			Player();

			void setStatus(STATUS status);

			void onSpotted(Character * otherCharacter) override;
			void onLostSight(Character * otherCharacter) override;

			void onWeatherChanged(WEATHER oldWeather, WEATHER newWeather) override;

		protected:

			STATUS m_status;
			int m_spottedCount;
		};

		Player::Player() :
			m_status(STATUS::Normal),
			m_spottedCount(getAwarenessManager()->getSpottedCount(this))
		{
			if (m_spottedCount == 0 && 
				getWeatherManager()->getCurrentWeather() != WEATHER::Foggy)
			{
				getEyeIcon()->close();
			}

			getAwarenessManager()->subscribe(this, this);
			getWeatherManager()->subscribe(this);
		}

		void Player::setStatus(STATUS status)
		{
			if (status == m_status)
				return;

			if (m_spottedCount == 0 &&
				getWeatherManager()->getCurrentWeather() != WEATHER::Foggy)
			{
				if (status == STATUS::Normal)
					getEyeIcon()->close();
				else if (m_status == STATUS::Normal)
					getEyeIcon()->open();
			}

			m_status = status;
		}

		void Player::onSpotted(Character * otherCharacter)
		{
			if (m_spottedCount == 0 && 
				m_status == STATUS::Normal &&
				getWeatherManager()->getCurrentWeather() != WEATHER::Foggy)
			{
				getEyeIcon()->open();
			}

			++m_spottedCount;
		}

		void Player::onLostSight(Character * otherCharacter)
		{
			--m_spottedCount;

			if (m_spottedCount == 0 && 
				m_status == STATUS::Normal &&
				getWeatherManager()->getCurrentWeather() != WEATHER::Foggy)
			{
				getEyeIcon()->close();
			}
		}

		void Player::onWeatherChanged(WEATHER oldWeather, WEATHER newWeather)
		{
			if (m_spottedCount == 0 &&
				m_status == STATUS::Normal)
			{
				if (oldWeather == WEATHER::Foggy)
					getEyeIcon()->close();
				else if (newWeather == WEATHER::Foggy)
					getEyeIcon()->open();
			}
		}
	};

	namespace x10
	{
		using namespace x8;

		enum class STATUS
		{
			Normal,
			Blindfolded
		};

		class Player : public Character, public AwarenessEvents, public WeatherEvents
		{
		public:
    
			Player();

			void setStatus(STATUS status);

			void onSpotted(Character * otherCharacter) override;
			void onLostSight(Character * otherCharacter) override;

			void onWeatherChanged(WEATHER oldWeather, WEATHER newWeather) override;

		protected:

			void refreshStealthIndicator();

			STATUS m_status;
		};

		Player::Player() :
			m_status(STATUS::Normal)
		{
			refreshStealthIndicator();

			getAwarenessManager()->subscribe(this, this);
			getWeatherManager()->subscribe(this);
		}

		void Player::setStatus(STATUS status)
		{
			m_status = status;

			refreshStealthIndicator();
		}

		void Player::onSpotted(Character * otherCharacter)
		{
			refreshStealthIndicator();
		}

		void Player::onLostSight(Character * otherCharacter)
		{
			refreshStealthIndicator();
		}

		void Player::onWeatherChanged(WEATHER oldWeather, WEATHER newWeather)
		{
			refreshStealthIndicator();
		}

		void Player::refreshStealthIndicator()
		{
			if (m_status == STATUS::Normal &&
				getAwarenessManager()->getSpottedCount(this) == 0 &&
				getWeatherManager()->getCurrentWeather() != WEATHER::Foggy)
			{
				getEyeIcon()->close();
			}
			else
			{
				getEyeIcon()->open();
			}
		}
	};

    void test(bool showDetails)
    {
		{
			using namespace x5;

			vector<Character *> characters;
			for (int i = 0; i < 5; ++i)
			{
				characters.push_back(new Character);
			}

			struct Event
			{
				int m_index;
				int m_otherIndex;
				int m_isVisible;
			};

			Event events[] =
			{
				{ 3, 0, true },
				{ 1, 2, false },
				{ 0, 4, true },
				{ 0, 4, false },
				{ 2, 4, true },
				{ 4, 2, true },
				{ 2, 3, true },
				{ 1, 0, true },
				{ 2, 0, true },
				{ 0, 3, false },
				{ 0, 4, false },
				{ 4, 0, true },
				{ 1, 2, false },
				{ 3, 2, false },
				{ 2, 1, false },
				{ 1, 0, false },
				{ 4, 0, true },
				{ 1, 3, true },
				{ 0, 4, true },
				{ 1, 2, false },
			};

			for (const Event & event : events)
			{
				getAwarenessManager()->markVisibility(
					characters[event.m_index], 
					characters[event.m_otherIndex], 
					event.m_isVisible);
			}

			vector<int> counts;
			vector<int> expectedCounts;

			for (int index = 0; index < 5; ++index)
			{
				counts.push_back(getAwarenessManager()->getSpottedCount(characters[index]));

				if (showDetails)
				{
					const AwarenessManager::Record & record = getAwarenessManager()->ensureRecord(characters[index]);
					printf("%d: spotted ", index);
					for (Character * spottedCharacter : record.m_spottedCharacters)
					{
						auto iter = find(characters.begin(), characters.end(), spottedCharacter);
						printf(" %d", static_cast<int>(iter - characters.begin()));
					}
					printf("\n");
				}
			}

			expectedCounts = { 2, 1, 2, 1, 2 };
			assert(counts == expectedCounts);
		}

		{
			using namespace x8;

			using namespace x10;

			vector<Character *> characters;

			Player * player = new Player;
			characters.push_back(player);

			for (int i = 0; i < 4; ++i)
			{
				characters.push_back(new Character);
			}

			struct Event
			{
				int m_index;
				int m_otherIndex;
				int m_isVisible;
			};

			//

			assert(!getEyeIcon()->isOpen());

			//

			Event events[] =
			{
				{ 0, 2, true },
				{ 0, 4, true },
				{ 2, 4, true },
				{ 1, 3, true },
			};

			for (const Event & event : events)
			{
				getAwarenessManager()->markVisibility(
					characters[event.m_index], 
					characters[event.m_otherIndex], 
					event.m_isVisible);
			}

			assert(getEyeIcon()->isOpen());

			//

			getWeatherManager()->setCurrentWeather(WEATHER::Foggy);

			assert(getEyeIcon()->isOpen());

			//

			player->setStatus(STATUS::Blindfolded);

			assert(getEyeIcon()->isOpen());

			//

			Event events2[] =
			{
				{ 0, 2, false },
				{ 0, 4, false },
			};

			for (const Event & event : events2)
			{
				getAwarenessManager()->markVisibility(
					characters[event.m_index], 
					characters[event.m_otherIndex], 
					event.m_isVisible);
			}

			assert(getEyeIcon()->isOpen());

			//

			getWeatherManager()->setCurrentWeather(WEATHER::Clear);
			player->setStatus(STATUS::Normal);

			assert(!getEyeIcon()->isOpen());
		}

		{
			using namespace x10;

			vector<Character *> characters;

			Player * player = new Player;
			characters.push_back(player);

			for (int i = 0; i < 4; ++i)
			{
				characters.push_back(new Character);
			}

			struct Event
			{
				int m_index;
				int m_otherIndex;
				int m_isVisible;
			};

			//

			assert(!getEyeIcon()->isOpen());

			//

			Event events[] =
			{
				{ 0, 2, true },
				{ 0, 4, true },
				{ 2, 4, true },
				{ 1, 3, true },
			};

			for (const Event & event : events)
			{
				getAwarenessManager()->markVisibility(
					characters[event.m_index], 
					characters[event.m_otherIndex], 
					event.m_isVisible);
			}

			assert(getEyeIcon()->isOpen());

			//

			getWeatherManager()->setCurrentWeather(WEATHER::Foggy);

			assert(getEyeIcon()->isOpen());

			//

			player->setStatus(STATUS::Blindfolded);

			assert(getEyeIcon()->isOpen());

			//

			Event events2[] =
			{
				{ 0, 2, false },
				{ 0, 4, false },
			};

			for (const Event & event : events2)
			{
				getAwarenessManager()->markVisibility(
					characters[event.m_index], 
					characters[event.m_otherIndex], 
					event.m_isVisible);
			}

			assert(getEyeIcon()->isOpen());

			//

			getWeatherManager()->setCurrentWeather(WEATHER::Clear);
			player->setStatus(STATUS::Normal);

			assert(!getEyeIcon()->isOpen());
		}
    }

	RegisterTest rt("Rule 10", test);
};
