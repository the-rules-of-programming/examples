
#include <vector>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include "geom.h"
#include "tests.h"

using namespace std;

#pragma warning(disable: 4018)

namespace rule8
{
	namespace misc
	{
		template <class CONT, class ELEM>
		void eraseByValue(CONT * container, const ELEM & element)
		{
			auto iter = find(container->begin(), container->end(), element);
			if (iter != container->end())
			{
				container->erase(iter, iter + 1);
			}
		}

		template <class CONT, class ELEM, class CMP>
		void insertSorted(CONT * container, const ELEM & element, CMP compare)
		{
			container->insert(
				upper_bound(container->begin(), container->end(), element),
				element);
		}

		enum class Faction
		{
			Red,
			Blue
		};
	};

	namespace x1
	{
		using namespace geom;
		using namespace misc;

		struct Person;

		bool isClearLineOfSight(const Person * person, const Person * otherPerson)
		{
			return true;
		}

		//

		struct Person
		{
			Person(Faction faction, const Point & position);
			~Person();

			bool isEnemy(const Person * otherPerson) const;
			const char * getName() const
				{ return ""; }

			void findNearbyEnemies(
				float maxDistance, 
				vector<Person *> * enemies);
			void findAllies(
				vector<Person *> * enemies);

			Faction m_faction;
			Point m_point;

			static vector<Person *> s_persons;
			static bool s_needsSort;
		};

		Person::Person(Faction faction, const Point & point) :
			m_faction(faction),
			m_point(point)
		{
			s_persons.push_back(this);
			s_needsSort = true;
		}

		Person::~Person()
		{
			eraseByValue(&s_persons, this);
		}

		bool Person::isEnemy(const Person * otherPerson) const
		{
			return m_faction != otherPerson->m_faction;
		}

		void Person::findNearbyEnemies(
			float maxDistance, 
			vector<Person *> * enemies)
		{
			for (Person * otherPerson : Person::s_persons)
			{
				float distance = getDistance(m_point, otherPerson->m_point);
				if (distance >= maxDistance)
					continue;

				if (!isEnemy(otherPerson))
					continue;

				enemies->push_back(otherPerson);
			}
		}

		bool compareFaction(Person * person, Person * otherPerson)
		{
			return person->m_faction < otherPerson->m_faction;
		}

		void Person::findAllies(vector<Person *> * allies)
		{
			if (s_needsSort)
			{
				s_needsSort = false;
				sort(s_persons.begin(), s_persons.end(), compareFaction);
			}

			int index = 0;

			for (; index < s_persons.size(); ++index)
			{
				if (!isEnemy(s_persons[index]))
					break;
			}

			for (; index < s_persons.size(); ++index)
			{
				Person * otherPerson = s_persons[index];
				if (isEnemy(otherPerson))
					break;

				if (otherPerson != this)
					allies->push_back(otherPerson);
			}
		}

		void example()
		{
			Person * player = nullptr;

			vector<Person *> allies;
			player->findAllies(&allies);

			vector<Person *> visibleAllies;
			for (Person * person : allies)
			{
				if (person != player && isClearLineOfSight(player, person))
					visibleAllies.push_back(person);
			}
		}

		//

		vector<Person *> Person::s_persons;
		bool Person::s_needsSort;
	};

	namespace x2
	{
		using namespace geom;
		using namespace misc;

		//

		struct Person
		{
			Person(Faction faction, const Point & position);
			~Person();

			bool isEnemy(const Person * otherPerson) const;

			template <class COND>
			void findPersons(COND condition, vector<Person *> * persons);

			Faction m_faction;
			Point m_point;

			static vector<Person *> s_persons;
			static bool s_needsSort;
		};

		template <class COND>
		void Person::findPersons(
			COND condition,
			vector<Person *> * persons)
		{
			for (Person * person : s_persons)
			{
				if (condition(person))
					persons->push_back(person);
			}
		}

		bool isClearLineOfSight(const Person * person, const Person * otherPerson)
		{
			return true;
		}

		void example()
		{
			Person * player = nullptr;
			vector<Person *> allies;

			struct IsVisibleAlly
			{
				IsVisibleAlly(Person * person) :
					m_person(person)
					{ ; }

				bool operator () (Person * otherPerson) const
				{
					return otherPerson != m_person &&
						   isClearLineOfSight(m_person, otherPerson) &&
						   !m_person->isEnemy(otherPerson);
				}

				Person * m_person;
			};

			player->findPersons(IsVisibleAlly(player), &allies);
		}

		//

		Person::Person(Faction faction, const Point & point) :
			m_faction(faction),
			m_point(point)
		{
			s_persons.push_back(this);
			s_needsSort = true;
		}

		Person::~Person()
		{
			eraseByValue(&s_persons, this);
		}

		bool Person::isEnemy(const Person * otherPerson) const
		{
			return m_faction != otherPerson->m_faction;
		}

		vector<Person *> Person::s_persons;
		bool Person::s_needsSort;
	};

	namespace x3
	{
		using namespace geom;
		using namespace misc;
		struct Person;

		enum class Hostility
		{
			Friendly,
			Neutral,
			Hostile
		};

		struct Disguise
		{
			virtual Hostility getHostility(const Person * otherPerson) const = 0;
		};

		struct Person
		{
			Person(Faction faction, const Point & position);
			~Person();

			bool isEnemy(const Person * otherPerson) const;
			void setDisguise(Disguise * disguise);

			template <class COND>
			void findPersons(COND condition, vector<Person *> * persons);

			Faction m_faction;
			Point m_point;
			Disguise * m_disguise;

			static vector<Person *> s_persons;
			static bool s_needsSort;
		};

		void Person::setDisguise(Disguise * disguise)
		{
			m_disguise = disguise;
		}

		bool Person::isEnemy(const Person * otherPerson) const
		{
			if (otherPerson == this)
				return false;

			if (m_disguise)
			{
				switch (m_disguise->getHostility(otherPerson))
				{
				case Hostility::Friendly:
					return false;

				case Hostility::Hostile:
					return true;

				case Hostility::Neutral:
					break;
				}
			}

			return m_faction != otherPerson->m_faction;
		}

		//

		vector<Person *> Person::s_persons;
		bool Person::s_needsSort;

		Person::Person(Faction faction, const Point & point) :
			m_faction(faction),
			m_point(point),
			m_disguise(nullptr)
		{
			s_persons.push_back(this);
			s_needsSort = true;
		}

		Person::~Person()
		{
			eraseByValue(&s_persons, this);
		}

		template <class COND>
		void Person::findPersons(COND condition, vector<Person *> * persons)
		{
			for (Person * person : s_persons)
			{
				if (condition(person))
					persons->push_back(person);
			}
		}
	};

	namespace x4
	{
		using namespace x1;

		void example()
		{
			Person * player = nullptr;

			vector<Person *> allies;
			player->findAllies(&allies);

			for (Person * ally : allies)
			{
				cout << ally->getName() << "\n";
			}
		}
	};

	namespace x5
	{
		using namespace x3;

		void example()
		{
			Person * player = nullptr;

			struct IsAlly
			{
				IsAlly(Person * person) :
					m_person(person)
					{ ; }

				bool operator () (Person * otherPerson) const
				{
					return otherPerson != m_person && 
						   !m_person->isEnemy(otherPerson);
				}

				Person * m_person;
			};

			vector<Person *> allies;
			player->findPersons(IsAlly(player), &allies);
		}
	};

    void test(bool showDetails)
    {
		{
			using namespace x1;

			struct Definition
			{
				Faction m_faction;
				Point m_position;
			};

			Definition definitions[] =
			{
				{ Faction::Red, { 0, 0, 0 } },
				{ Faction::Red, { 200, 0, 0 } },
				{ Faction::Red, { 400, 0, 0 } },
				{ Faction::Blue, { 600, 0, 0 } },
				{ Faction::Red, { 800, 0, 0 } },
				{ Faction::Blue, { 1000, 0, 0 } },
			};

			vector<Person *> persons;

			for (const Definition & definition : definitions)
			{
				Person * person = new Person(definition.m_faction, definition.m_position);
				persons.push_back(person);
			}

			vector<Person *> otherPersons;
			vector<Person *> expectedPersons;

			persons[0]->findAllies(&otherPersons);

			expectedPersons = { persons[1], persons[2], persons[4] };
			assert(otherPersons == expectedPersons);

			otherPersons.clear();
			persons[3]->findNearbyEnemies(50, &otherPersons);
			assert(otherPersons.empty());

			otherPersons.clear();
			persons[3]->findNearbyEnemies(250, &otherPersons);

			expectedPersons = { persons[2], persons[4] };
			assert(otherPersons == expectedPersons);
		};

 		{
			using namespace x2;

			struct Definition
			{
				Faction m_faction;
				Point m_position;
			};

			Definition definitions[] =
			{
				{ Faction::Red, { 0, 0, 0 } },
				{ Faction::Red, { 200, 0, 0 } },
				{ Faction::Red, { 400, 0, 0 } },
				{ Faction::Blue, { 600, 0, 0 } },
				{ Faction::Red, { 800, 0, 0 } },
				{ Faction::Blue, { 1000, 0, 0 } },
			};

			vector<Person *> persons;

			for (const Definition & definition : definitions)
			{
				Person * person = new Person(definition.m_faction, definition.m_position);
				persons.push_back(person);
			}

			struct IsVisibleAlly
			{
				IsVisibleAlly(Person * person) :
					m_person(person)
					{ ; }

				bool operator () (Person * otherPerson) const
				{
					return otherPerson != m_person &&
						   isClearLineOfSight(m_person, otherPerson) &&
						   !m_person->isEnemy(otherPerson);
				}

				Person * m_person;
			};

			Person * player = persons[0];
			vector<Person *> otherPersons;
			vector<Person *> expectedPersons;

			player->findPersons(IsVisibleAlly(player), &otherPersons);

			expectedPersons = { persons[1], persons[2], persons[4] };
			assert(otherPersons == expectedPersons);

			struct Fail
			{
				bool operator () (Person * otherPerson) const
				{
					return false;
				}
			};

			otherPersons.clear();
			player->findPersons(Fail(), &otherPersons);

			assert(otherPersons.empty());

			struct Pass
			{
				bool operator () (Person * otherPerson) const
				{
					return true;
				}
			};

			otherPersons.clear();
			player->findPersons(Pass(), &otherPersons);

			assert(otherPersons.size() == 6);
		};

		{
			using namespace x3;

			struct Definition
			{
				Faction m_faction;
				Point m_position;
			};

			Definition definitions[] =
			{
				{ Faction::Red, { 0, 0, 0 } },
				{ Faction::Red, { 200, 0, 0 } },
				{ Faction::Red, { 400, 0, 0 } },
				{ Faction::Blue, { 600, 0, 0 } },
				{ Faction::Red, { 800, 0, 0 } },
				{ Faction::Blue, { 1000, 0, 0 } },
			};

			vector<Person *> persons;

			for (const Definition & definition : definitions)
			{
				Person * person = new Person(definition.m_faction, definition.m_position);
				persons.push_back(person);
			}

			struct MakeEnemy : public Disguise
			{
				MakeEnemy(const Person * otherPerson) :
					m_otherPerson(otherPerson)
					{ ; }

				Hostility getHostility(const Person * otherPerson) const override
				{
					if (otherPerson == m_otherPerson)
						return Hostility::Hostile;

					return Hostility::Neutral;
				}

				const Person * m_otherPerson;
			};

			struct IsVisibleAlly
			{
				IsVisibleAlly(Person * person) :
					m_person(person)
					{ ; }

				bool operator () (Person * otherPerson) const
				{
					return otherPerson != m_person &&
						   !m_person->isEnemy(otherPerson);
				}

				Person * m_person;
			};

			persons[0]->setDisguise(new MakeEnemy(persons[1]));

			Person * player = persons[0];
			vector<Person *> otherPersons;
			vector<Person *> expectedPersons;

			player->findPersons(IsVisibleAlly(player), &otherPersons);

			expectedPersons = { persons[2], persons[4] };
			assert(otherPersons == expectedPersons);
		}
    }

	RegisterTest rt("Rule 8", test);
};
