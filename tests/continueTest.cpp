#include "BasicTestHelpers.hpp"

typedef std::string StringClass;
typedef AhoCorasick::Match<StringClass> StringMatch;

static const StringClass text[] = { "First word is hello, the secoind one is world", "And lets add something else"};
static std::vector<StringClass> strings = { "hello", "world", "bla-bla", "orld", "orl", "something" };

static std::vector<StringMatch> expected = {
	MakeMatch<StringMatch>(14, 0, strings),
	MakeMatch<StringMatch>(41, 4, strings),
	MakeMatch<StringMatch>(40, 1, strings),
	MakeMatch<StringMatch>(41, 3, strings),
	MakeMatch<StringMatch>(58, 5, strings),
};

class TestContinueHandler
{
public:
	bool operator()(StringClass::const_iterator& begin, StringClass::const_iterator& end)
	{
		if (++mIndex >= _countof(text))
			return false;

		begin = text[mIndex].begin();
		end = text[mIndex].end();

		return true;
	}

private:
	size_t mIndex = 0;
};

int main()
{
	return ContinueStrTest<AhoCorasick::PerformanceStrategy::Balanced>(text[0], expected, strings, TestContinueHandler()) ? 0 : 1;
}
