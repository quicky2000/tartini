/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
double sq(const double & x)
{
  return x * x;
}

//------------------------------------------------------------------------------
#ifndef pow10
//rasises 10^x
double pow10(const double x)
{
  return pow(10.0, x);
}
#endif // pow10

//------------------------------------------------------------------------------
#ifndef pow2
//rasises 2^x
double pow2(const double & x)
{
  return pow(2.0, x);
}
#endif // POW2

//------------------------------------------------------------------------------
#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
double log2(const double & x)
{
  return log(x) / 0.69314718055994530941723212145818;
}
#endif // WINDOWS

//------------------------------------------------------------------------------
double logBaseN(const double & baseN, const double & x)
{
  return log(x) / log(baseN);
}

//------------------------------------------------------------------------------
double prand(void)
{
  return double(rand()) / double(RAND_MAX);
}

//------------------------------------------------------------------------------
double cycle(const double & a, const double & b)
{
    if(a >= 0.0)
      {
	return fmod(a, b);
      }
    else
      {
	return b + fmod(a, b);
      }
}

//------------------------------------------------------------------------------
int cycle(const int a, const int b)
{
    if(a >= 0)
      {
	return a % b;
      }
    else
      {
	return b + ((a+1) % b) - 1;
      }
}

//------------------------------------------------------------------------------
double myround(const double & x)
{
  return floor(x + 0.5);
}

//------------------------------------------------------------------------------
int toInt(const float x)
{
  return lrintf(x);
}

//------------------------------------------------------------------------------
int toInt(const double & x)
{
  return lrint(x);
}

//------------------------------------------------------------------------------
int intFloor(const float x)
{
  return lrintf(x-0.5);
}

//------------------------------------------------------------------------------
int intFloor(const double & x)
{
  return lrint(x-0.5);
}

//------------------------------------------------------------------------------
int roundUp(const int x, const int multiple)
{
  int remainder = x % multiple;
  return (remainder == 0) ? x : x - remainder + multiple;
}

//------------------------------------------------------------------------------
template<class T>
bool within(T plusMinus, T value1, T value2)
{
  return (value1 >= value2-plusMinus) ? (value1 <= value2+plusMinus) : false;
}

//------------------------------------------------------------------------------
template<class T>
bool between(T x, T lowerBound, T upperBound)
{
  return (x >= lowerBound) && (x <= upperBound);
}

//------------------------------------------------------------------------------
template<class T>
T bound(T var, T lowerBound, T upperBound)
{
  //this way will deal with NAN, setting it to lowerBound
  if(var >= lowerBound)
    {
      if(var <= upperBound)
	{
	  return var;
	}
      else
	{
	  return upperBound;
	}
    }
  else
    {
      return lowerBound;
    }
}

//------------------------------------------------------------------------------
double parabolaTurningPoint(const double & a, const double & b, const double & c)
{
  double bottom = (2 * ( c + a - 2 * b));
  return (bottom == 0.0) ? 0.0 : ((a - c) / bottom);
}

//------------------------------------------------------------------------------
template<class T>
inline void parabolaTurningPoint2(T y_1, T y0, T y1, T xOffset, T *x, T *y)
{
  T yTop = y_1 - y1;
  T yBottom = y1 + y_1 - 2 * y0;
  if(yBottom != 0.0)
    {
      *x = xOffset + yTop / (2 * yBottom);
      *y = y0 - (sq(yTop) / (8 * yBottom));
    }
  else
    {
      *x = xOffset;
      *y = y0;
    }
}

//------------------------------------------------------------------------------
MinMax::MinMax(void):
  min(0.0),
  max(0.0)
{
}

//------------------------------------------------------------------------------
MinMax::MinMax(float min_, float max_):
  min(min_),
  max(max_)
{
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
int maxIndex(ForwardIterator aFirst, int length)
{
  int bestIndex = 0;
  for(int j = 1; j < length; j++)
    {
      if(aFirst[j] > aFirst[bestIndex])
	{
	  bestIndex = j;
	}
    }
  return bestIndex;
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
int minIndex(ForwardIterator aFirst, int length)
{
  int bestIndex = 0;
  for(int j = 1; j < length; j++)
    {
      if(aFirst[j] < aFirst[bestIndex])
	{
	  bestIndex = j;
	}
    }
  return bestIndex;
}

//------------------------------------------------------------------------------
template<typename _ForwardIter, typename _Compare>
std::pair<_ForwardIter, _ForwardIter> minMaxElement(_ForwardIter __first, _ForwardIter __last, _Compare __lessComp)
{
  // concept requirements
  //__glibcpp_function_requires(_ForwardIteratorConcept<_ForwardIter>)
  //__glibcpp_function_requires(_BinaryPredicateConcept<_Compare,
  //typename iterator_traits<_ForwardIter>::value_type,
  //typename iterator_traits<_ForwardIter>::value_type>)

  std::pair<_ForwardIter, _ForwardIter> __result(__first, __first);
  if (__first == __last)
    {
      return __result;
    }
  while (++__first != __last)
    {
      if (__lessComp(*__first, *__result.first))
	{
	  __result.first = __first;
	}
      if (__lessComp(*__result.second, *__first))
	{
	  __result.second = __first;
	}
  }
  return __result;
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst)
{
  while(aFirst != aLast)
    {
      *aFirst += *bFirst;
      ++bFirst;
      ++aFirst;
    }
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler)
{
  //while(aFirst != aLast) *bFirst++ += *aFirst++;
  while(aFirst != aLast) { *aFirst += (*bFirst) * scaler; ++bFirst; ++aFirst; }
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
void copyElementsScale(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler)
{
  while(aFirst != aLast)
    {
      *bFirst++ = (*aFirst++) * scaler;
    }
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
void copyElementsDivide(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType div)
{
  while(aFirst != aLast)
    {
      *bFirst++ = (*aFirst++) / div;
    }
}

//------------------------------------------------------------------------------
template <class T>
T absolute(const T &x)
{
  return (x >= 0) ? x : -x;
}

//------------------------------------------------------------------------------
template <class T>
bool absoluteLess<T>::operator()(const T &x, const T &y) const
{
  return absolute(x) < absolute(y);
}

//------------------------------------------------------------------------------
template <class T>
bool absoluteGreater<T>::operator()(T &x, T &y) const
{
  return absolute(x) > absolute(y);
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
ForwardIterator binary_search_closest(ForwardIterator aFirst, ForwardIterator aLast, const ElementType &value)
{
  std::pair<ForwardIterator, ForwardIterator> range = std::equal_range(aFirst, aLast, value);
  if(range.first != aFirst)
    {
      --range.first;
    }
  if(range.second != aLast)
    {
      ++range.second;
    }
  ForwardIterator best = range.first;
  ForwardIterator it = best;
  while(++it != range.second)
    {
      if(absolute(*it - value) < absolute(*best - value))
	{
	  best = it;
	}
    }
  return best;
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
ForwardIterator print_elements(ForwardIterator aFirst, ForwardIterator aLast)
{
  std::cout << "[";
  if(aFirst != aLast)
    {
      std::cout << *aFirst++;
    }
  while(aFirst != aLast)
    {
      std::cout << " " << *aFirst++;
    }
  std::cout << "]" << std::endl;
}
//EOF

