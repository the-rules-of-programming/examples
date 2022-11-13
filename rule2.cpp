
#include <vector>
#include <algorithm>
#include <assert.h>
#include "tests.h"

using namespace std;

#pragma warning(disable: 4018)

namespace rule2
{
	namespace x1
	{
		int sumVector(const vector<int> & values)
		{
			int sum = 0;
			for (int value : values)
			{
				sum += value;
			}
			return sum;
		} 
	};

	namespace x2
	{
		int reduce(
			int initialValue,
			int (*reduceFunction)(int, int),
			const vector<int> & values)
		{
			int reducedValue = initialValue;
			for (int value : values)
			{
				reducedValue = reduceFunction(reducedValue, value);
			}
			return reducedValue;
		}

		int sum(int value, int otherValue)
		{
			return value + otherValue;
		}

		int example()
		{
			vector<int> values = { 1, 2, 3, 4 };
			int vectorSum = reduce(0, sum, values);
			return vectorSum;
		}
	};

	namespace x4
	{
		class Character
		{
		public:

			Character(int priority);
			~Character();

			void setPriority(int priority);
			int getPriority() const;

			static const vector<Character *> & getAllCharacters();

		protected:

			int m_priority;
			int m_index;

			static vector<Character *> s_allCharacters;
		};

		Character::Character(int priority) :
			m_priority(priority),
			m_index(0)
		{
			int index = 0;
			for (; index < s_allCharacters.size(); ++index)
			{
				if (priority <= s_allCharacters[index]->m_priority)
					break;
			}

			s_allCharacters.insert(s_allCharacters.begin() + index, this);

			for (; index < s_allCharacters.size(); ++index)
			{
				s_allCharacters[index]->m_index = index;
			}
		}

		Character::~Character()
		{
			int index = 0;
			for (; index < s_allCharacters.size(); ++index)
			{
				if (this == s_allCharacters[index])
					break;
			}

			s_allCharacters.erase(s_allCharacters.begin() + index);

			for (; index < s_allCharacters.size(); ++index)
			{
				s_allCharacters[index]->m_index = index;
			}
		}

		void Character::setPriority(int priority)
		{
			if (priority == m_priority)
				return;

			m_priority = priority;

			while (m_index > 0)
			{
				Character * character = s_allCharacters[m_index - 1];
				if (character->m_priority <= priority)
					break;

				s_allCharacters[m_index] = character;
				character->m_index = m_index;

				--m_index;
			}

			while (m_index + 1 < s_allCharacters.size())
			{
				Character * character = s_allCharacters[m_index + 1];
				if (character->m_priority >= priority)
					break;

				s_allCharacters[m_index] = character;
				character->m_index = m_index;

				++m_index;
			}

			s_allCharacters[m_index] = this;
		}

		//

		int Character::getPriority() const
		{
			return m_priority;
		}

		const vector<Character *> & Character::getAllCharacters()
		{
			return s_allCharacters;
		}

		vector<Character *> Character::s_allCharacters;
	};

	namespace x5
	{
		class Character
		{
		public:

			Character(int priority) :
				m_priority(priority)
			{
				s_allCharacters.push_back(this);
			}

			~Character()
			{
				auto iter = find(
								s_allCharacters.begin(), 
								s_allCharacters.end(), 
								this);
				s_allCharacters.erase(iter);
			}

			void setPriority(int priority)
			{
				m_priority = priority;
			}

			int getPriority() const
			{
				return m_priority;
			}

			static int sortByPriority(
				Character * left, 
				Character * right)
			{
				return left->m_priority < right->m_priority;
			}

			static vector<Character *> getAllCharacters()
			{
				vector<Character *> sortedCharacters = s_allCharacters;

				sort(
					sortedCharacters.begin(), 
					sortedCharacters.end(), 
					sortByPriority);

				return sortedCharacters;
			}

		protected:

			int m_priority;

			static vector<Character *> s_allCharacters;
		};

		//

		vector<Character *> Character::s_allCharacters;
	};

	namespace x6
	{
		class Character
		{
		public:

			void audit();
			static void auditAll();

		protected:

			int m_priority;
			int m_index;

			static vector<Character *> s_allCharacters;
		};

		void Character::audit()
		{
			assert(s_allCharacters[m_index] == this);
		}

		void Character::auditAll()
		{
			for (int index = 0; index < s_allCharacters.size(); ++index)
			{
				Character * character = s_allCharacters[index];

				if (index > 0)
				{
					Character * prevCharacter = s_allCharacters[index - 1];
					assert(character->m_priority >= prevCharacter->m_priority);
				}

				character->audit();
			}
		}

		//

		vector<Character *> Character::s_allCharacters;
	};

	namespace x7
	{
		struct RigidBodySimulator
		{
			struct InitializationParameters
			{
				void * (* m_allocationFunction)(size_t size);
				void (* m_freeFunction)(void * memory);
				float m_gravity;
			};

			void initialize(const InitializationParameters & params);
			void shutDown();
		};

		void RigidBodySimulator::initialize(const InitializationParameters & params)
		{
		}

		void RigidBodySimulator::shutDown()
		{
		}
	};

	namespace xMath
	{
		template <int ROWS, int COLUMNS>
		struct Matrix
		{
			float m_values[ROWS][COLUMNS];
		};

		struct Point
		{
			float m_x;
			float m_y;
			float m_z;
		};

		struct Vector
		{
			float m_x;
			float m_y;
			float m_z;
		};

		struct Quaternion
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;
		};

		struct Triangle
		{
		};
	};

	namespace x8
	{
		using namespace xMath;

		struct RigidBodySimulator
		{
			struct ObjectID
			{
				int m_index;
			};

			struct ObjectDefinition
			{
				float m_mass;
				Matrix<3, 3> m_momentOfInertia;
				vector<Triangle> m_triangles;
			};

			struct ObjectState
			{
				Point m_position;
				Quaternion m_orientation;
				Vector m_velocity;
				Vector m_angularVelocity;
			};

			ObjectID createObject(
				const ObjectDefinition & objectDefinition, 
				const ObjectState & objectState);
			void destroyObject(
				ObjectID objectID);
			ObjectState getObjectState(
				ObjectID objectID) const;
			void setObjectState(
				ObjectID objectID,
				const ObjectState & objectState);
		};
	};

	namespace x9
	{
		using namespace xMath;

		struct RigidBodySimulator
		{
			struct ObjectID
			{
				int m_index;
			};

			struct ObjectDefinition
			{
				float m_mass;
				Matrix<3, 3> m_momentOfInertia;
				vector<Triangle> m_triangles;
			};

			struct ObjectState
			{
				Point m_position;
				Quaternion m_orientation;
				Vector m_velocity;
				Vector m_angularVelocity;
			};

			ObjectID createObject(
				const ObjectDefinition & objectDefinition, 
				const ObjectState & objectState);
			void destroyObject(
				ObjectID objectID);

			ObjectState getObjectState(
				ObjectID objectID) const
			{
				return m_objectStates[objectID.m_index];
			}
                
			void setObjectState(
				ObjectID objectID,
				const ObjectState & objectState)
			{
				m_objectStates[objectID.m_index] = objectState;
			}

			vector<ObjectState> m_objectStates;
		};
	};

	namespace x10
	{
		using namespace xMath;

		struct RigidBodySimulator
		{
			class ObjectID
			{
				friend struct RigidBodySimulator;

			public:

				ObjectID() :
					m_index(-1), m_generation(-1)
					{ ; }

			protected:

				ObjectID(int index, int generation) :
					m_index(index), m_generation(generation)
					{ ; }

				int m_index;
				int m_generation;
			};

			struct ObjectDefinition
			{
				float m_mass;
				Matrix<3, 3> m_momentOfInertia;
				vector<Triangle> m_triangles;
			};

			struct ObjectState
			{
				Point m_position;
				Quaternion m_orientation;
				Vector m_velocity;
				Vector m_angularVelocity;
			};

			bool isObjectIDValid(ObjectID objectID) const
			{
				return objectID.m_index >= 0 &&
					objectID.m_index < m_indexGenerations.size() &&
					m_indexGenerations[objectID.m_index] == objectID.m_generation;
			}

			int findUnusedIndex()
			{
				return 0;
			}

			ObjectID createObject(
				const ObjectDefinition & objectDefinition, 
				const ObjectState & objectState)
			{
				int index = findUnusedIndex();

				++m_indexGenerations[index];
				m_objectDefinitions[index] = objectDefinition;
				m_objectStates[index] = objectState;

				return ObjectID(index, m_indexGenerations[index]);
			}

			void destroyObject(ObjectID objectID)
			{
				assert(isObjectIDValid(objectID));
				++m_indexGenerations[objectID.m_index];
			}

			ObjectState getObjectState(ObjectID objectID) const
			{
				assert(isObjectIDValid(objectID));
				return m_objectStates[objectID.m_index];
			}
                
			void setObjectState(
				ObjectID objectID,
				const ObjectState & objectState)
			{
				assert(isObjectIDValid(objectID));
				m_objectStates[objectID.m_index] = objectState;
			}

			vector<int> m_indexGenerations;
			vector<ObjectDefinition> m_objectDefinitions;
			vector<ObjectState> m_objectStates;
		};

	};

    void test(bool showDetails)
    {
		{
			using namespace x1;
			vector<int> values = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			assert(sumVector(values) == 55);
		}

		{
			using namespace x2;
			vector<int> values = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			assert(reduce(0, sum, values) == 55);
		}

		{
			using namespace x4;

			for (int i = 0; i < 10; ++i)
			{
				new Character(i);
			}

			int expectedSum = 45;
			
			for (int i = 0;; ++i)
			{
				int sum = 0;
				for (Character * character : Character::getAllCharacters())
				{
					sum += character->getPriority();
				}
				assert(sum == expectedSum);

				int lastPriority = INT_MIN;
				for (Character * character : Character::getAllCharacters())
				{
					assert(character->getPriority() >= lastPriority);
					lastPriority = character->getPriority();
				}

				if (i >= 10)
					break;

				const vector<Character *> & chars = Character::getAllCharacters();
				int index = rand() % chars.size();
				int oldPriority = chars[index]->getPriority();
				int newPriority = rand() % 10;
				expectedSum = expectedSum - oldPriority + newPriority;
				chars[index]->setPriority(newPriority);
			}
		}

		{
			using namespace x5;

			for (int i = 0; i < 10; ++i)
			{
				new Character(i);
			}

			int expectedSum = 45;
			
			for (int i = 0;; ++i)
			{
				int sum = 0;
				for (Character * character : Character::getAllCharacters())
				{
					sum += character->getPriority();
				}
				assert(sum == expectedSum);

				int lastPriority = INT_MIN;
				for (Character * character : Character::getAllCharacters())
				{
					assert(character->getPriority() >= lastPriority);
					lastPriority = character->getPriority();
				}

				if (i >= 10)
					break;

				const vector<Character *> & chars = Character::getAllCharacters();
				int index = rand() % chars.size();
				int oldPriority = chars[index]->getPriority();
				int newPriority = rand() % 10;
				expectedSum = expectedSum - oldPriority + newPriority;
				chars[index]->setPriority(newPriority);
			}
		}
	}

	RegisterTest rt("Rule 2", test);
};
