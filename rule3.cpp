
#include <vector>
#include <assert.h>
#include "tests.h"

using namespace std;

#pragma warning(disable: 4200)

namespace rule3
{
	namespace x1
	{
		void cp(
			int n, 
			float rr[], 
			float ii[], 
			float xr, 
			float xi, 
			float * yr, 
			float * yi)
		{
			float rn = 1.0f, in = 0.0f;
			*yr = 0.0f;
			*yi = 0.0f;
			for (int i = 0; i <= n; ++i)
			{
				*yr += rr[i] * rn - ii[i] * in;
				*yi += ii[i] * rn + rr[i] * in;
				float rn2 = rn * xr - in * xi;
				in = in * xr + rn * xi;
				rn = rn2;
			}
		}
	};

	namespace x2
	{
		void evaluateComplexPolynomial(
			int degree, 
			float realCoeffs[], 
			float imagCoeffs[], 
			float realX, 
			float imagX, 
			float * realY, 
			float * imagY)
		{
			float realXN = 1.0f, imagXN = 0.0f;
			*realY = 0.0f;
			*imagY = 0.0f;
			for (int n = 0; n <= degree; ++n)
			{
				*realY += realCoeffs[n] * realXN - imagCoeffs[n] * imagXN;
				*imagY += imagCoeffs[n] * realXN + realCoeffs[n] * imagXN;
				float realTemp = realXN * realX - imagXN * imagX;
				imagXN = imagXN * realX + realXN * imagX;
				realXN = realTemp;
			}
		}
	};

	namespace x3
	{
		template <class T>
		struct complex
		{
			T m_real;
			T m_imag;
		};

		template <class T>
		complex<T> operator + (complex<T> left, complex<T> right)
		{
			return { left.m_real + right.m_real, left.m_imag + right.m_imag };
		}

		template <class T>
		complex<T> operator * (complex<T> left, complex<T> right)
		{
			T real = left.m_real * right.m_real - left.m_imag * right.m_imag;
			T imag = left.m_real * right.m_imag + left.m_imag * right.m_real;

			return { real, imag };
		}

		template <class T>
		complex<T> & operator += (complex<T> & left, complex<T> right)
		{
			left = left + right;
			return left;
		}

		template <class T>
		complex<T> & operator *= (complex<T> & left, complex<T> right)
		{
			left = left * right;
			return left;
		}

		void evaluateComplexPolynomial(
			vector<complex<float>> & coeffs,
			complex<float> x,
			complex<float> * y)
		{
			complex<float> xN = { 1.0f, 0.0f };
			*y = { 0.0f, 0.0f };
			for (const complex<float> & coeff : coeffs)
			{
				*y += xN * coeff;
				xN *= x;
			}
		}
	};

	namespace x4
	{
		template <class ELEM, int MAX_COUNT = 8>
		class FixedVector
		{
		public:

			FixedVector() :
				m_count(0)
				{ ; }

			void append(const ELEM & elem)
				{
					assert(!isFull());
					(void) new (&m_elements[m_count++]) ELEM(elem);
				}
			void empty()
				{
					while (m_count > 0)
					{
						m_elements[--m_count].~ELEM();
					}
				}
			int getCount() const
				{ return m_count; }
			bool isEmpty() const
				{ return m_count == 0; }
			bool isFull() const
				{ return m_count >= MAX_COUNT; }

			ELEM & operator [] (int index)
				{ return m_elements[index]; }
			const ELEM & operator [] (int index) const
				{ return m_elements[index]; }

		protected:

			int m_count;
			union
			{
				ELEM m_elements[0];
				char m_storage[sizeof(ELEM) * MAX_COUNT];
			};
		};

		void reverseToFixedVector(
			vector<int> * source, 
			FixedVector<int, 8> * dest)
		{
			dest->empty();
			while (!source->empty())
			{
				if (dest->isFull())
					break;

				dest->append(source->back());
				source->pop_back();
			}
		}
	};

	namespace wnd
	{
		struct RECT
		{
			RECT() :
				left(0), top(0), right(0), bottom(0)
				{ ; }
			RECT(int left, int top, int right, int bottom) :
				left(left), top(top), right(right), bottom(bottom)
				{ ; }

			int left;
			int top;
			int right;
			int bottom;
		};

		struct Window
		{
			Window() :
				m_rect(), 
				m_prevWindow(nullptr), 
				m_nextWindow(nullptr)
				{
					m_prevWindow = s_bottomWindow;

					s_bottomWindow = this;
					if (!s_topWindow)
						s_topWindow = this;
				}

			RECT m_rect;
			Window * m_prevWindow;
			Window * m_nextWindow;

			static Window * s_topWindow;
			static Window * s_bottomWindow;
		};

		Window * Window::s_topWindow = nullptr;
		Window * Window::s_bottomWindow = nullptr;

		typedef Window * HWND;
		typedef RECT * LPRECT;
		typedef int SWP;

		#define HWND_TOP nullptr
		#define SWP_NOACTIVATE 1

		void SetWindowPos(HWND hwnd, HWND hwndPrev, int x, int y, int width, int height, SWP swp)
		{
			assert(swp == SWP_NOACTIVATE);

			//

			if (hwnd->m_prevWindow)
				hwnd->m_prevWindow->m_nextWindow = hwnd->m_nextWindow;
			else
				Window::s_topWindow = hwnd->m_nextWindow;

			if (hwnd->m_nextWindow)
				hwnd->m_nextWindow->m_prevWindow = hwnd->m_prevWindow;
			else
				Window::s_bottomWindow = hwnd->m_prevWindow;

			hwnd->m_nextWindow = nullptr;
			hwnd->m_prevWindow = nullptr;

			//

			if (hwndPrev == HWND_TOP)
			{
				hwnd->m_nextWindow = Window::s_topWindow;
				Window::s_topWindow = hwnd;

				if (!Window::s_bottomWindow)
					Window::s_bottomWindow = hwnd;
			}
			else
			{
				hwnd->m_nextWindow = hwndPrev->m_nextWindow;
				hwnd->m_prevWindow = hwndPrev;

				hwndPrev->m_nextWindow = hwnd;

				if (hwnd->m_nextWindow)
					hwnd->m_nextWindow->m_prevWindow = hwnd;
				else
					Window::s_bottomWindow = hwnd;
			}

			//

			hwnd->m_rect.left = x;
			hwnd->m_rect.top = y;
			hwnd->m_rect.right = x + width;
			hwnd->m_rect.bottom = y + height;
		}
	};

	namespace x5
	{
		using namespace wnd;

		int split(int min, int max, int index, int count)
		{
			return min + (max - min) * index / count;
		}

		void split(int x0, int x1, int y0, int y1, int & r0, int & r1)
		{
			r0 = split(x0, x1, y0, y1);
			r1 = split(x0, x1, y0 + 1, y1);
		}

		void layoutWindows(vector<HWND> ww, LPRECT rc)
		{
			int w = ww.size();
			int rowCount = int(sqrtf(float(w - 1))) + 1;
			int extra = rowCount * rowCount - w;
			int r = 0, c = 0;
			HWND hWndPrev = HWND_TOP;
			for (HWND theWindow : ww)
			{
				int cols = (r < extra) ? rowCount - 1 : rowCount;
				int x0, x1, y0, y1;
				split(rc->left, rc->right, c, cols, x0, x1);
				split(rc->top, rc->bottom, r, rowCount, y0, y1);
				SetWindowPos(
					theWindow, 
					hWndPrev, 
					x0, 
					y0, 
					x1 - x0, 
					y1 - y0, 
					SWP_NOACTIVATE);
				hWndPrev = theWindow;
				if (++c >= cols)
				{
					c = 0;
					++r;
				}
			}
		}
	};

	namespace x6
	{
		using namespace wnd;

		int divideRange(int min, int max, int index, int count)
		{
			return min + (max - min) * index / count;
		}

		void layoutWindows(vector<HWND> windows, LPRECT rect)
		{
			int windowCount = windows.size();
			int rowCount = int(sqrtf(float(windowCount - 1))) + 1;
			int shortRowCount = rowCount * rowCount - windowCount;
			int rowIndex = 0, colIndex = 0;
			HWND lastWindow = HWND_TOP;

			for (HWND window : windows)
			{
				int colCount = (rowIndex < shortRowCount) ? 
									rowCount - 1 : 
									rowCount;

				int left = divideRange(
								rect->left, 
								rect->right, 
								colIndex, 
								colCount);
				int right = divideRange(
								rect->left, 
								rect->right, 
								colIndex + 1, 
								colCount);
				int top = divideRange(
								rect->top, 
								rect->bottom, 
								rowIndex, 
								rowCount);
				int bottom = divideRange(
								rect->top, 
								rect->bottom, 
								rowIndex + 1, 
								rowCount);

				SetWindowPos(
					window,
					lastWindow,
					left,
					top,
					right - left,
					bottom - top,
					SWP_NOACTIVATE);

				lastWindow = window;
				if (++colIndex >= colCount)
				{
					colIndex = 0;
					++rowIndex;
				}
			}
		}
	};

    void test(bool showDetails)
    {
		/*
			x^0 = 1
			x^1 = 1 + i
			x^2 = (1 + i) * (1 + i) = 2i

			1 + (1 + 2i) * (1 + i) + (3 - i) * 2i
			= 1 + (1 + 3i - 2) + (6i + 2)
			= (1 + 1 - 2 + 2) + (3i + 6i)
			= 2 + 9i
		*/

		{
			using namespace x1;

			float rr[] = { 1.0f, 1.0f, 3.0f };
			float ir[] = { 0.0f, 2.0f, -1.0f };
			float yr;
			float yi;

			cp(2, rr, ir, 1.0f, 1.0f, &yr, &yi);

			assert(yr == 2.0f && yi == 9.0f);
		}

		{
			using namespace x2;

			float realCoeffs[] = { 1.0f, 1.0f, 3.0f };
			float imagCoeffs[] = { 0.0f, 2.0f, -1.0f };
			float realY;
			float imagY;

			evaluateComplexPolynomial(2, realCoeffs, imagCoeffs, 1.0f, 1.0f, &realY, &imagY);

			assert(realY == 2.0f && imagY == 9.0f);
		}

		{
			using namespace x3;

			vector<complex<float>> terms = { { 1.0f, 0.0f }, { 1.0f, 2.0f }, { 3.0f, -1.0f } };
			complex<float> y;
			evaluateComplexPolynomial(terms, { 1.0f, 1.0f }, &y);
			assert(y.m_real == 2.0f && y.m_imag == 9.0f);
		}

		{
			using namespace x4;

			vector<int> ints;
			for (int i = 0; i < 6; ++i)
				ints.push_back(i);

			FixedVector<int, 8> reversedInts;
			reverseToFixedVector(&ints, &reversedInts);

			for (int i = 0; i < 6; ++i)
				assert(reversedInts[i] == 5 - i);
		}

		{
			using namespace x5;

			vector<HWND> windows;

			for (int i = 0; i < 7; ++i)
				windows.push_back(new Window);

			RECT rect = { 0, 0, 10, 10 };
			layoutWindows(windows, &rect);

			int windowIndexAtPixel[10][10];

			for (int x = 0; x < 10; ++x)
			{
				for (int y = 0; y < 10; ++y)
				{
					windowIndexAtPixel[x][y] = -1;
				}
			}

			int windowIndex = 0;
			for (const Window * window = Window::s_topWindow;
				 window;
				 window = window->m_nextWindow)
			{
				assert(window->m_rect.left >= 0 && window->m_rect.left <= 10);
				assert(window->m_rect.top >= 0 && window->m_rect.top <= 10);
				assert(window->m_rect.right >= 0 && window->m_rect.right <= 10);
				assert(window->m_rect.bottom >= 0 && window->m_rect.bottom <= 10);

				for (int x = window->m_rect.left; x < window->m_rect.right; ++x)
				{
					for (int y = window->m_rect.top; y < window->m_rect.bottom; ++y)
					{
						assert(windowIndexAtPixel[x][y] < 0);
						windowIndexAtPixel[x][y] = windowIndex;
					}
				}

				++windowIndex;
			}

			if (showDetails)
			{
				for (int y = 0; y < 10; ++y)
				{
					for (int x = 0; x < 10; ++x)
					{
						printf("%-3d", windowIndexAtPixel[x][y]);
					}
					printf("\n");
				}
			}
		}

		{
			using namespace x5;

			vector<HWND> windows;

			for (int i = 0; i < 7; ++i)
				windows.push_back(new Window);

			RECT rect = { 0, 0, 10, 10 };
			layoutWindows(windows, &rect);

			int windowIndexAtPixel[10][10];

			for (int x = 0; x < 10; ++x)
			{
				for (int y = 0; y < 10; ++y)
				{
					windowIndexAtPixel[x][y] = -1;
				}
			}

			int windowIndex = 0;
			for (const Window * window = Window::s_topWindow;
				 window;
				 window = window->m_nextWindow)
			{
				assert(window->m_rect.left >= 0 && window->m_rect.left <= 10);
				assert(window->m_rect.top >= 0 && window->m_rect.top <= 10);
				assert(window->m_rect.right >= 0 && window->m_rect.right <= 10);
				assert(window->m_rect.bottom >= 0 && window->m_rect.bottom <= 10);

				for (int x = window->m_rect.left; x < window->m_rect.right; ++x)
				{
					for (int y = window->m_rect.top; y < window->m_rect.bottom; ++y)
					{
						assert(windowIndexAtPixel[x][y] < 0);
						windowIndexAtPixel[x][y] = windowIndex;
					}
				}

				++windowIndex;
			}

			if (showDetails)
			{
				for (int y = 0; y < 10; ++y)
				{
					for (int x = 0; x < 10; ++x)
					{
						printf("%-3d", windowIndexAtPixel[x][y]);
					}
					printf("\n");
				}
			}
		}
    }

	RegisterTest rt("Rule 3", test);
};
