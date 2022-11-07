
#include <vector>
#include <assert.h>
#include "tests.h"

using namespace std;

#pragma warning (disable: 4018)

namespace rule19
{
	namespace OldStack
	{
        class StackAlloc
        {
            friend class StackContext;

        public:

            static void * alloc(int byteCount);

            template <class T>
            static T * alloc(int count)
                { return static_cast<T *>(alloc(sizeof(T) * count)); }

        protected:

            struct Index
            {
                int m_chunkIndex;
                int m_byteIndex;
            };

            static Index s_index;
            static vector<char *> s_chunks;
        };

        StackAlloc::Index StackAlloc::s_index;
        vector<char *> StackAlloc::s_chunks;

        const int c_chunkSize = 1024 * 1024;

        void * StackAlloc::alloc(int byteCount)
        {
            assert(byteCount <= c_chunkSize);

            while (true)
            {
                int chunkIndex = s_index.m_chunkIndex;
                int byteIndex = s_index.m_byteIndex;

                if (chunkIndex >= s_chunks.size())
                {
                    s_chunks.push_back(new char[c_chunkSize]);
                }

                if (s_index.m_byteIndex + byteCount <= c_chunkSize)
                {
                    s_index.m_byteIndex += byteCount;
                    return &s_chunks[chunkIndex][byteIndex];
                }

                s_index = { chunkIndex + 1, 0 };
            }
        }

        class StackContext 
        {
        public:

            StackContext()
            : m_index(StackAlloc::s_index)
                { ; }
            ~StackContext()
                { StackAlloc::s_index = m_index; }

        protected:

            StackAlloc::Index m_index;
        };
    };

	namespace x1
	{
        using namespace OldStack;

        template <class ELEMENT>
        class StackVector
        {
        public:

            StackVector();
            ~StackVector();

            void reserve(int capacity);
            int size() const;
            void push_back(const ELEMENT & element);
            void pop_back();
            ELEMENT & back();
            ELEMENT & operator [](int index);

			struct Iterator
			{
				ELEMENT & operator * () const
					{ return *m_element; }
				void operator ++ ()
					{ ++m_element; }
				bool operator != (const Iterator & iterator)
					{ return m_element != iterator.m_element; }

				ELEMENT * m_element;
			};

			Iterator begin()
				{ return { &m_elements[0] }; }
			Iterator end()
				{ return { &m_elements[m_count] }; }

        protected:

            int m_count;
            int m_capacity;
            ELEMENT * m_elements;
        };

        template <class ELEMENT>
        StackVector<ELEMENT>::StackVector() :
            m_count(0), 
            m_capacity(0), 
            m_elements(nullptr)
        {
        }

        template <class ELEMENT>
        StackVector<ELEMENT>::~StackVector()
        {
            for (int index = 0; index < m_count; ++index)
            {
                m_elements[index].~ELEMENT();
            } 
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::reserve(int capacity)
        {
            if (capacity <= m_capacity)
                return;

            ELEMENT * newElements = StackAlloc::alloc<ELEMENT>(capacity);

            for (int index = 0; index < m_count; ++index)
            {
                newElements[index] = move(m_elements[index]);
            }

            m_capacity = capacity;
            m_elements = newElements;
        }

        template <class ELEMENT>
        int StackVector<ELEMENT>::size() const
        {
            return m_count;
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::push_back(const ELEMENT & element)
        {
            if (m_count >= m_capacity)
            {
                reserve(max(8, m_capacity * 2));
            }

            new (&m_elements[m_count++]) ELEMENT(element);
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::pop_back()
        {
            m_elements[--m_count].~ELEMENT();
        }

        template <class ELEMENT>
        ELEMENT & StackVector<ELEMENT>::back()
        {
            return m_elements[m_count - 1];
        }

        template <class ELEMENT>
        ELEMENT & StackVector<ELEMENT>::operator [](int index)
        {
            return m_elements[index];
        }

        void getPrimeFactors(
            int number,
            StackVector<int> * factors)
        {
            for (int factor = 2; factor * factor <= number; )
            {
                if (number % factor == 0)
                {
                    factors->push_back(factor);
                    number /= factor;
                }
                else
                {
                    ++factor;
                }
            }

            factors->push_back(number);
        }
    };

	namespace x2
	{
        using namespace x1;

        struct Character
        {
            bool isEnemy() const
                { return false; }
        };

        struct CharacterData
        {
            Character * m_character;
        };

        void findNearbyCharacters(
            float maxDistance, 
            StackVector<CharacterData> * datas)
        {
        }

        void findNearbyEnemies(
            float maxDistance, 
            StackVector<Character *> * enemies)
        {
            StackContext context;
            StackVector<CharacterData> datas;
            findNearbyCharacters(maxDistance, &datas);

            for (const CharacterData & data : datas)
            {
                if (data.m_character->isEnemy())
                {
                    enemies->push_back(data.m_character);
                }
            }
        }
	};

	namespace NewStack
	{
        class StackContext
        {
        public:
    
            StackContext();
            ~StackContext();

            void * alloc(int byteCount);

            template <class T>
            T * alloc(int count)
                { return static_cast<T *>(alloc(sizeof(T) * count)); }

            static StackContext * currentContext();

        protected:

            struct Index
            {
                int m_chunkIndex;
                int m_byteIndex;
            };

            static char * ensureChunk();
            static void recoverChunk(char * chunk);

            struct Sequence
            {
                Sequence() :
                    m_index({ 0, 0 }), m_chunks()
                    { ; }

                void * alloc(int byteCount);

                Index m_index;
                vector<char *> m_chunks;
            };

            Index m_initialIndex;
            Sequence m_extraSequence;

            static const int c_chunkSize = 1024 * 1024;

            static Sequence s_mainSequence;
            static vector<char *> s_unusedChunks;
            static vector<StackContext *> s_contexts;
        };

        char * StackContext::ensureChunk()
        {
            char * chunk = nullptr;

            if (!s_unusedChunks.empty())
            {
                chunk = s_unusedChunks.back();
                s_unusedChunks.pop_back();
            }
            else
            {
                chunk = new char[c_chunkSize];
            }

            return chunk;
        }

        void StackContext::recoverChunk(char * chunk)
        {
            s_unusedChunks.push_back(chunk);
        }

        void * StackContext::Sequence::alloc(int byteCount)
        {
            assert(byteCount <= c_chunkSize);

            while (true)
            {
                int chunkIndex = m_index.m_chunkIndex;
                int byteIndex = m_index.m_byteIndex;

                if (chunkIndex >= m_chunks.size())
                {
                    m_chunks.push_back(new char[c_chunkSize]);
                }

                if (m_index.m_byteIndex + byteCount <= c_chunkSize)
                {
                    m_index.m_byteIndex += byteCount;
                    return &m_chunks[chunkIndex][byteIndex];
                }

                m_index = { chunkIndex + 1, 0 };
            }
        }

        StackContext::StackContext() :
            m_initialIndex(s_mainSequence.m_index), 
            m_extraSequence()
        {
            s_contexts.push_back(this);
        }

        StackContext::~StackContext()
        {
            assert(s_contexts.back() == this);

            for (char * chunk : m_extraSequence.m_chunks)
            {
                recoverChunk(chunk);
            }

            s_mainSequence.m_index = m_initialIndex;
            s_contexts.pop_back();
        }

        void * StackContext::alloc(int byteCount)
        {
            return (s_contexts.back() == this) ? 
                        s_mainSequence.alloc(byteCount) : 
                        m_extraSequence.alloc(byteCount);
        }

        StackContext * StackContext::currentContext()
        {
            return (s_contexts.empty()) ? nullptr : s_contexts.back();
        }

        StackContext::Sequence StackContext::s_mainSequence;
        vector<char *> StackContext::s_unusedChunks;
        vector<StackContext *> StackContext::s_contexts;
    };

    namespace x3
    {
        using namespace NewStack;

        template <class ELEMENT>
        class StackVector
        {
        public:
    
            StackVector(StackContext * context);

            void reserve(int capacity);
            void push_back(const ELEMENT & element);

        protected:

            StackContext * m_context;
            int m_count;
            int m_capacity;
            ELEMENT * m_elements;
        };

        template <class ELEMENT>
        StackVector<ELEMENT>::StackVector(StackContext * context) :
            m_context(context), 
            m_count(0), 
            m_capacity(0), 
            m_elements(nullptr)
        {
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::reserve(int capacity)
        {
            if (capacity <= m_capacity)
                return;

            ELEMENT * newElements = m_context->alloc<ELEMENT>(capacity);

            for (int index = 0; index < m_count; ++index)
            {

                newElements[index] = move(m_elements[index]);
            }

            m_capacity = capacity;
            m_elements = newElements;
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::push_back(const ELEMENT & element)
        {
            if (m_count >= m_capacity)
            {
                reserve(max(8, m_capacity * 2));
            }

            new (&m_elements[m_count++]) ELEMENT(element);
        }
    };

	namespace DualStack
	{
        bool g_useNewStackAlloc = false;

        class StackAlloc
        {
        public:

            static void * alloc(int byteCount);
        };

        void * StackAlloc::alloc(int byteCount)
        {
            return (g_useNewStackAlloc) ?
                NewStack::StackContext::currentContext()->alloc(byteCount) :
                OldStack::StackAlloc::alloc(byteCount);
        }

        class StackContext
        {
        public:
    
            StackContext() :
                m_oldContext(),
                m_newContext()
                { s_contexts.push_back(this); }
            ~StackContext()
                { s_contexts.pop_back(); }

            void * alloc(int byteCount);

            template <class T>
            T * alloc(int count)
                { return static_cast<T *>(alloc(sizeof(T) * count)); }

            template <class T>
            T * allocNew(int count)
                { return m_newContext.alloc<T>(count); }

            static StackContext * currentContext()
                { return (s_contexts.empty()) ? nullptr : s_contexts.back(); }

        protected:

            OldStack::StackContext m_oldContext;
            NewStack::StackContext m_newContext;

            static vector<StackContext *> s_contexts;
        };

        vector<StackContext *> StackContext::s_contexts;

        void * StackContext::alloc(int byteCount)
        {
            return m_newContext.alloc(byteCount);
        }
    };

	namespace x4
	{
        using namespace DualStack;

        template <class ELEMENT>
        class StackVector
        {
        public:
    
            StackVector();
            StackVector(StackContext * context);

            void reserve(int capacity);
            void push_back(const ELEMENT & element);

        protected:
    
            bool m_isExplicitContext;
            StackContext * m_context;
            int m_count;
            int m_capacity;
            ELEMENT * m_elements;
        };

        template <class ELEMENT>
        StackVector<ELEMENT>::StackVector()
        : m_isExplicitContext(false),
            m_context(StackContext::currentContext()), 
            m_count(0), 
            m_capacity(0), 
            m_elements(nullptr)
        {
        }

        template <class ELEMENT>
        StackVector<ELEMENT>::StackVector(StackContext * context)
        : m_isExplicitContext(true),
            m_context(context), 
            m_count(0), 
            m_capacity(0), 
            m_elements(nullptr)
        {
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::reserve(int capacity)
        {
            if (capacity <= m_capacity)
                return;

            assert(
                m_isExplicitContext || 
                m_context == StackContext::currentContext());

            ELEMENT * newElements = (m_isExplicitContext) ?
                m_context->allocNew<ELEMENT>(capacity) :
                m_context->alloc<ELEMENT>(capacity);

            for (int index = 0; index < m_count; ++index)
            {
                newElements[index] = move(m_elements[index]);
            }

            m_elements = newElements;
        }

        template <class ELEMENT>
        void StackVector<ELEMENT>::push_back(const ELEMENT & element)
        {
            if (m_count >= m_capacity)
            {
                reserve(max(8, m_capacity * 2));
            }

            new (&m_elements[m_count++]) ELEMENT(element);
        }
	};

    void test(bool showDetails)
    {

        {
            using namespace OldStack;

            struct Block
            {
                int m_size;
                int m_fill;
                unsigned char * m_bytes;
            };

            struct Checker
            {
                Checker(const vector<Block> & blocks) :
                    m_blocks(blocks)
                    { ; }

                void allocateBlocks()
                {
                    for (Block & block : m_blocks)
                    {
                        block.m_bytes = static_cast<unsigned char *>(StackAlloc::alloc(block.m_size));
                        memset(block.m_bytes, block.m_fill, block.m_size);
                    }
                }

                void checkBlocks()
                {
                    for (const Block & block : m_blocks)
                    {
                        for (int index = 0; index < block.m_size; ++index)
                        {
                            assert(block.m_bytes[index] == block.m_fill);
                        }
                    }
                }

                vector<Block> m_blocks;
            };

            vector<Block> blocks =
            {
                { 55, 0 },
                { 550, 1 },
                { 55, 2 },
                { 5500, 3 },
                { 55, 4 },
                { 55000, 5 },
                { 55, 6 },
                { 550000, 7 },
                { 20, 8 },
                { 1400, 9 },
            };

            StackContext sc;
            Checker checker(blocks);

            checker.allocateBlocks();

            {
                StackContext sc;
                Checker checker(blocks);

                checker.allocateBlocks();

                {
                    StackContext sc;
                    Checker checker(blocks);

                    checker.allocateBlocks();
                    checker.checkBlocks();
                }

                checker.checkBlocks();
            }

            checker.checkBlocks();
        }

        {
            using namespace NewStack;

            struct Block
            {
                int m_size;
                int m_fill;
                unsigned char * m_bytes;
            };

            struct Checker
            {
                Checker(const vector<Block> & blocks) :
                    m_blocks(blocks)
                    { ; }

                void allocateBlocks(
                    int minIndex = -1,
                    int maxIndex = -1,
                    StackContext * sc = nullptr)
                {
                    if (minIndex < 0)
                        minIndex = 0;
                    if (maxIndex < 0)
                        maxIndex = m_blocks.size();
                    if (!sc)
                        sc = StackContext::currentContext();

                    for (int index = minIndex; index < maxIndex; ++index)
                    {
                        Block & block = m_blocks[index];
                        block.m_bytes = static_cast<unsigned char *>(sc->alloc(block.m_size));
                        memset(block.m_bytes, block.m_fill, block.m_size);
                    }
                }

                void checkBlocks()
                {
                    for (const Block & block : m_blocks)
                    {
                        for (int index = 0; index < block.m_size; ++index)
                        {
                            assert(block.m_bytes[index] == block.m_fill);
                        }
                    }
                }

                vector<Block> m_blocks;
            };

            vector<Block> blocks =
            {
                { 55, 0 },
                { 550, 1 },
                { 55, 2 },
                { 5500, 3 },
                { 55, 4 },
                { 55000, 5 },
                { 55, 6 },
                { 550000, 7 },
                { 20, 8 },
                { 1400, 9 },
            };

            StackContext sc1;
            Checker checker1(blocks);

            checker1.allocateBlocks();

            {
                StackContext sc2;
                Checker checker2(blocks);

                checker2.allocateBlocks(0, 4);

                {
                    StackContext sc3;
                    Checker checker3(blocks);

                    checker2.allocateBlocks(4, 6, &sc2);
                    checker3.allocateBlocks();
                    checker2.allocateBlocks(6, 9, &sc2);
                    checker3.checkBlocks();
                }

                checker2.allocateBlocks(9, 10);
                checker2.checkBlocks();
            }

            checker1.checkBlocks();
        }

        {
            using namespace x1;

            StackContext ctx;
            StackVector<int> ints;
            ints.push_back(1);
            ints.push_back(2);
            ints.push_back(4);
        }

        {
            using namespace x3;

            StackContext ctx;
            StackVector<int> ints(&ctx);
            ints.push_back(1);
            ints.push_back(2);
            ints.push_back(4);
        }

        {
            using namespace x4;

            StackContext ctx;
            StackVector<int> ints;
            ints.push_back(1);
            ints.push_back(2);
            ints.push_back(4);
        }
    };

    RegisterTest rt("Rule 19", test);
};
