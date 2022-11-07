
#include <vector>
#include <unordered_map>
#include <assert.h>
#include "tests.h"

using namespace std;

#pragma warning(disable:4018)

namespace rule1
{
	namespace x1
	{
		int countSetBits(int value)
        {
            int count = 0;

            while (value)
            {
                ++count;
                value = value & (value - 1);
            }

            return count;
        }
	};

    namespace x2
    {
        int countSetBits(int value)
        {
            value = ((value & 0xaaaaaaaa) >> 1) + (value & 0x55555555);
            value = ((value & 0xcccccccc) >> 2) + (value & 0x33333333);
            value = ((value & 0xf0f0f0f0) >> 4) + (value & 0x0f0f0f0f);
            value = ((value & 0xff00ff00) >> 8) + (value & 0x00ff00ff);
            value = ((value & 0xffff0000) >> 16) + (value & 0x000ffff);

            return value;
        }
    };

    namespace x3
    {
        int countSetBits(int value)
        {
            int count = 0;

            for (int bit = 0; bit < 32; ++bit)
            {
                if (value & (1 << bit))
                    ++count;
            }

            return count;
        }
    };

    namespace x4
    {
        int countStepPatterns(int stepCount)
        {
            if (stepCount < 0)
                return 0;
    
            if (stepCount == 0)
                return 1;

            return countStepPatterns(stepCount - 3) + 
                   countStepPatterns(stepCount - 2) + 
                   countStepPatterns(stepCount - 1);
        }
    };

    namespace x5
    {
        int countStepPatterns(unordered_map<int, int> * memo, int rungCount)
        {
            if (rungCount < 0)
                return 0;
    
            if (rungCount == 0)
                return 1;

            auto iter = memo->find(rungCount);
            if (iter != memo->end())
                return iter->second;

            int stepPatternCount = countStepPatterns(memo, rungCount - 3) + 
                                   countStepPatterns(memo, rungCount - 2) + 
                                   countStepPatterns(memo, rungCount - 1);

            memo->insert({ rungCount, stepPatternCount });
            return stepPatternCount;
        }

        int countStepPatterns(int rungCount)
        {
            unordered_map<int, int> memo;
            return countStepPatterns(&memo, rungCount);
        }
    };

    namespace x6
    {
        int countStepPatterns(int rungCount)
        {
            vector<int> stepPatternCounts = { 0, 0, 1 };

            for (int rungIndex = 0; rungIndex < rungCount; ++rungIndex)
            {
                stepPatternCounts.push_back(
                    stepPatternCounts[rungIndex + 0] +
                    stepPatternCounts[rungIndex + 1] +
                    stepPatternCounts[rungIndex + 2]);
            }

            return stepPatternCounts.back();
        }
    };

    namespace x7
    {
        struct Ordinal
        {
        public:

            Ordinal() :
                m_words()
                { ; }
            Ordinal(unsigned int value) :
                m_words({ value })
                { ; }
            explicit Ordinal(unsigned long long value) :
                m_words()
                {
                    m_words.push_back(static_cast<unsigned int>(value) & 0xffffffff);
                    m_words.push_back(static_cast<unsigned int>(value >> 32));
                }

            typedef unsigned int Word;

            Ordinal operator + (const Ordinal & value) const
            {
                int wordCount = max(m_words.size(), value.m_words.size());

                Ordinal result;
                long long carry = 0;

                for (int wordIndex = 0; wordIndex < wordCount; ++wordIndex)
                {
                    long long sum = carry + 
                                    getWord(wordIndex) + 
                                    value.getWord(wordIndex);

                    result.m_words.push_back(Word(sum));
                    carry = sum >> 32;
                }

                if (carry > 0)
                    result.m_words.push_back(Word(carry));

                return result;
            }

            bool operator == (const Ordinal & value) const
            {
                int count = m_words.size();
                int otherCount = value.m_words.size();

                for (int index = 0; index < max(count, otherCount); ++index)
                {
                    Word word = (index < count) ? m_words[index] : 0;
                    Word otherWord = (index < otherCount) ? value.m_words[index] : 0;

                    if (word != otherWord)
                        return false;
                }

                return true;
            }

        protected:

            Word getWord(int wordIndex) const
            {
                return (wordIndex < m_words.size()) ? m_words[wordIndex] : 0;
            }

            vector<Word> m_words;
        };
    };

    namespace x8
    {
        using namespace x7;

        Ordinal countStepPatterns(int rungCount)
        {
            vector<Ordinal> stepPatternCounts = { 0, 0, 1 };

            for (int rungIndex = 0; rungIndex < rungCount; ++rungIndex)
            {
                stepPatternCounts.push_back(
                    stepPatternCounts[rungIndex + 0] +
                    stepPatternCounts[rungIndex + 1] +
                    stepPatternCounts[rungIndex + 2]);
            }

            return stepPatternCounts.back();
        }
    };

    namespace x9
    {
        int countStepPatterns(int rungCount)
        {
            // NOTE (chris) can't represent the pattern count in an int
            //  once we get past 36 rungs...

            assert(rungCount <= 36);

            vector<int> stepPatternCounts = { 0, 0, 1 };

            for (int rungIndex = 0; rungIndex < rungCount; ++rungIndex)
            {
                stepPatternCounts.push_back(
                    stepPatternCounts[rungIndex + 0] +
                    stepPatternCounts[rungIndex + 1] +
                    stepPatternCounts[rungIndex + 2]);
            }

            return stepPatternCounts.back();
        }
    };

    namespace crd
    {
        struct Card
        {
            int m_ordinal;
        };
    };

    namespace x10
    {
        using namespace crd;

        vector<Card> shuffleOnce(const vector<Card> & cards)
        {
            vector<Card> shuffledCards;

            int splitIndex = cards.size() / 2;
            int leftIndex = 0;
            int rightIndex = splitIndex;

            while (true)
            {
                if (leftIndex >= splitIndex)
                {
                    for (; rightIndex < cards.size(); ++rightIndex)
                        shuffledCards.push_back(cards[rightIndex]);

                    break;
                }
                else if (rightIndex >= cards.size())
                {
                    for (; leftIndex < splitIndex; ++leftIndex)
                        shuffledCards.push_back(cards[leftIndex]);

                    break;
                }
                else if (rand() & 1)
                {
                    shuffledCards.push_back(cards[rightIndex]);
                    ++rightIndex;
                }
                else
                {
                    shuffledCards.push_back(cards[leftIndex]);
                    ++leftIndex;
                }
            }

            return shuffledCards;
        }

        vector<Card> shuffle(const vector<Card> & cards)
        {
            vector<Card> shuffledCards = cards;

            for (int i = 0; i < 7; ++i)
            {
                shuffledCards = shuffleOnce(shuffledCards);
            }

            return shuffledCards;
        }
    };

    namespace x11
    {
        using namespace crd;

        vector<Card> shuffle(const vector<Card> & cards)
        {
            vector<Card> shuffledCards = cards;

            for (int cardIndex = shuffledCards.size(); --cardIndex >= 0; )
            {
                int swapIndex = rand() % (cardIndex + 1);
                swap(shuffledCards[swapIndex], shuffledCards[cardIndex]);
            }

            return shuffledCards;
        }
    };

    namespace x12
    {
        using namespace crd;

        void copyCard(
            vector<Card> * destinationCards, 
            const vector<Card> & sourceCards,
            int * sourceIndex)
        {
            destinationCards->push_back(sourceCards[*sourceIndex]);
            ++(*sourceIndex);
        }

        void copyCards(
            vector<Card> * destinationCards, 
            const vector<Card> & sourceCards,
            int * sourceIndex,
            int endIndex)
        {
            while (*sourceIndex < endIndex)
            {
                copyCard(destinationCards, sourceCards, sourceIndex);
            }
        }

        vector<Card> shuffleOnce(const vector<Card> & cards)
        {
            vector<Card> shuffledCards;

            int splitIndex = cards.size() / 2;
            int leftIndex = 0;
            int rightIndex = splitIndex;

            while (true)
            {
                if (leftIndex >= splitIndex)
                {
                    copyCards(&shuffledCards, cards, &rightIndex, cards.size());
                    break;
                }
                else if (rightIndex >= cards.size())
                {
                    copyCards(&shuffledCards, cards, &leftIndex, splitIndex);
                    break;
                }
                else if (rand() & 1)
                {
                    copyCard(&shuffledCards, cards, &rightIndex);
                }
                else
                {
                    copyCard(&shuffledCards, cards, &leftIndex);
                }
            }

            return shuffledCards;
        }

        vector<Card> shuffle(const vector<Card> & cards)
        {
            vector<Card> shuffledCards = cards;

            for (int i = 0; i < 7; ++i)
            {
                shuffledCards = shuffleOnce(shuffledCards);
            }

            return shuffledCards;
        }
    };

    void test(bool showDetails)
    {
        {
            using namespace x1;
            assert(countSetBits(0) == 0);
            assert(countSetBits(0x378) == 6);
            assert(countSetBits(0x3780f00) == 10);
        }

        {
            using namespace x2;
            assert(countSetBits(0) == 0);
            assert(countSetBits(0x378) == 6);
            assert(countSetBits(0x3780f00) == 10);
        }

        {
            using namespace x3;
            assert(countSetBits(0) == 0);
            assert(countSetBits(0x378) == 6);
            assert(countSetBits(0x3780f00) == 10);
        }

        {
            using namespace x4;
            assert(countStepPatterns(1) == 1);
            assert(countStepPatterns(4) == 7);
            assert(countStepPatterns(5) == 13);
            assert(countStepPatterns(20) == 121415);
        }

        {
            using namespace x5;
            assert(countStepPatterns(1) == 1);
            assert(countStepPatterns(4) == 7);
            assert(countStepPatterns(5) == 13);
            assert(countStepPatterns(20) == 121415);
        }

        {
            using namespace x6;
            assert(countStepPatterns(1) == 1);
            assert(countStepPatterns(4) == 7);
            assert(countStepPatterns(5) == 13);
            assert(countStepPatterns(20) == 121415);
        }

        {
            using namespace x8;
            assert(countStepPatterns(1) == 1);
            assert(countStepPatterns(4) == 7);
            assert(countStepPatterns(5) == 13);
            assert(countStepPatterns(20) == 121415);
            assert(countStepPatterns(36) == Ordinal(2082876103ULL));
            assert(countStepPatterns(37) == Ordinal(3831006429ULL));
        }

        struct Deck
        {
            Deck(int count) :
                m_cards()
            {
                for (int i = 0; i < count; ++i)
                {
                    m_cards.push_back({ i });
                }
            }

            int getSum() const
            {
                int sum = 0;
                for (const crd::Card & card : m_cards)
                    sum += card.m_ordinal;
                return sum;
            }

            vector<crd::Card> m_cards;
        };

        {
            using namespace x10;

            for (int i = 0; i < 5; ++i)
            {
                Deck deck(20);
                shuffle(deck.m_cards);
                assert(deck.getSum() == 190);
            }
        }

        {
            using namespace x11;

            for (int i = 0; i < 5; ++i)
            {
                Deck deck(20);
                shuffle(deck.m_cards);
                assert(deck.getSum() == 190);
            }
        }

        {
            using namespace x12;

            for (int i = 0; i < 5; ++i)
            {
                Deck deck(20);
                shuffle(deck.m_cards);
                assert(deck.getSum() == 190);
            }
        }
    }

    RegisterTest rt("Rule 1", test);
};
