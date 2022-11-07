
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <assert.h>
#include <stdarg.h>
#include "tests.h"

using namespace std;

#pragma warning (disable: 4996)

namespace rule12
{
	namespace x1
	{
		/// \struct TREE
		/// \brief Binary tree of integers
		/// \var TREE::l
		/// Left subtree
		/// \var TREE::r
		/// Right subtree
		/// \var TREE::n
		/// Data

		/// \fn sum(Tree * t)
		/// \brief Return sum of all integers in the tree
		/// \param t Root of tree (or subtree) to sum
		/// \returns Sum of integers in tree

		struct TREE { typedef TREE self; self * l; self * r; int n; };
		int sum(TREE * t) { return (!t) ? 0 : t->n + sum(t->l) + sum(t->r); }

		// Integer tree node

		struct STree
		{
			STree *     m_leftTree;
			STree *     m_rightTree;
			int         m_data;
		};

		// Return sum of all integers in tree

		int sumTree(STree * tree)
		{
			if (!tree)
				return 0;

			return tree->m_data + 
				   sumTree(tree->m_leftTree) + 
				   sumTree(tree->m_rightTree);
		}
	};

	namespace x2
	{
		struct Tree
		{
			Tree *		m_leftTree;
			Tree *		m_rightTree;
			int			m_data;
		};

		void visitInOrder(const Tree * tree, const function<void (const Tree *)> & pfn)
		{
			if (!tree)
				return;

			visitInOrder(tree->m_leftTree, pfn);
			pfn(tree);
			visitInOrder(tree->m_rightTree, pfn);
		}

		int sumTree(const Tree * tree)
		{
			int sum = 0;
			visitInOrder(tree, [&sum](const Tree * tree) { sum += tree->m_data; });
			return sum;
		}
	};

	namespace x3
	{
		string getFileAsString(string fileName)
		{
			errno = 0;
			string s;
			FILE * file = fopen(fileName.c_str(), "r");
			if (file)
			{
				while (true)
				{
					int c = getc(file);
					if (c == EOF)
					{
						if (ferror(file))
							s.clear();

						break;
					}

					s += c;
				}

				fclose(file);
			}

			return s;
		}
	};

	namespace x4
	{
		void log(const char * format, ...)
		{
		}

		bool tryGetFileAsString(string fileName, string * result)
		{
			ifstream file;
			file.open(fileName.c_str(), ifstream::in);
			if (!file.good())
			{
				log(
					"failed to open file %s: %s", 
					fileName.c_str(), 
					strerror(errno));

				return false;
			}

			string s;
			while (true)
			{
				char c = file.get();
				if (c == EOF)
				{
					if (file.bad())
					{
						log(
							"error reading file %s: %s", 
							fileName.c_str(), 
							strerror(errno));

						return false;
					}

					break;
				}

				s += c;
			}

			*result = s;
			return true;
		}
	};

	namespace x5
	{
		enum class ErrorCode
		{
			None,
			Custom
		};

		struct Result
		{
			Result(ErrorCode errorCode);
			Result(const char * format, ...);

			operator bool () const
				{ return m_errorCode == ErrorCode::None; }

			ErrorCode m_errorCode;
			string m_error;
		};

		Result tryGetFileAsString(string fileName, string * result)
		{
			result->clear();

			ifstream file;
			file.open(fileName.c_str(), ifstream::in);
			if (!file.good())
			{
				return Result(
							"failed to open file %s: %s", 
							fileName.c_str(), 
							strerror(errno));
			}

			string s;
			while (true)
			{
				int c = file.get();
				if (c == EOF)
					break;

				if (file.bad())
					return Result(
								"error reading file %s: %s", 
								fileName.c_str(), 
								strerror(errno));

				s += c;
			}

			*result = s;
			return ErrorCode::None;
		}

		//

		Result::Result(ErrorCode errorCode) :
			m_errorCode(errorCode),
			m_error()
		{
		}

		Result::Result(const char * format, ...)
		{
			va_list args;
			va_start(args, format);

			char error[1024];
			vsprintf_s(error, format, args);

			m_errorCode = ErrorCode::Custom;
			m_error = error;
		}
	};

	namespace x6
	{
		string getFileAsString(string fileName)
		{
			ifstream file;
			file.exceptions(ifstream::failbit | ifstream::badbit);
			file.open(fileName.c_str(), ifstream::in);

			string s;
			file.exceptions(ifstream::badbit);
			while (true)
			{
				int c = file.get();
				if (c == EOF)
					break;

				s += c;
			}

			return s;
		}
	};

    void test(bool showDetails)
    {
		{
			using namespace x3;

			string s = getFileAsString("testlines.txt");
			assert(s == "test line 1\nand line 2\nline 3, to conclude");
		}

		{
			using namespace x4;

			string s;
			bool f = tryGetFileAsString("testlines.txt", &s);
			assert(f);
			assert(s == "test line 1\nand line 2\nline 3, to conclude");

			f = tryGetFileAsString("nofile.txt", &s);
			assert(!f);
		}

		{
			using namespace x5;

			string s;
			Result result = tryGetFileAsString("testlines.txt", &s);
			assert(result);
			assert(s == "test line 1\nand line 2\nline 3, to conclude");

			result = tryGetFileAsString("nofile.txt", &s);
			assert(!result);
			if (showDetails)
			{
				printf("%s\n", result.m_error.c_str());
			}
		}

		{
			using namespace x6;

			try
			{
				string s = getFileAsString("testlines.txt");
				assert(s == "test line 1\nand line 2\nline 3, to conclude");
			}
			catch (...)
			{
				assert(false);
			}

			try
			{
				string s = getFileAsString("nofile.txt");
				assert(false);
			}
			catch (...)
			{
			}
		}
    }

	RegisterTest rt("Rule 12", test);
};
