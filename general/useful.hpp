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
double sq(const double & p_x)
{
    return p_x * p_x;
}

//------------------------------------------------------------------------------
#ifndef pow10
//rasises 10^x
double pow10(const double p_x)
{
    return pow(10.0, p_x);
}
#endif // pow10

//------------------------------------------------------------------------------
#ifndef pow2
//rasises 2^x
double pow2(const double & p_x)
{
    return pow(2.0, p_x);
}
#endif // POW2

//------------------------------------------------------------------------------
#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
double log2(const double & p_x)
{
    return log(p_x) / 0.69314718055994530941723212145818;
}
#endif // WINDOWS

//------------------------------------------------------------------------------
double logBaseN( const double & p_base_N
               , const double & p_x
               )
{
    return log(p_x) / log(p_base_N);
}

//------------------------------------------------------------------------------
double prand(void)
{
    return double(rand()) / double(RAND_MAX);
}

//------------------------------------------------------------------------------
double cycle( const double & p_a
            , const double & p_b
            )
{
    if(p_a >= 0.0)
    {
        return fmod(p_a, p_b);
    }
    else
    {
        return p_b + fmod(p_a, p_b);
    }
}

//------------------------------------------------------------------------------
int cycle( const int p_a
         , const int p_b
         )
{
    if(p_a >= 0)
    {
        return p_a % p_b;
    }
    else
    {
        return p_b + ((p_a + 1) % p_b) - 1;
    }
}

//------------------------------------------------------------------------------
double myround(const double & p_x)
{
    return floor(p_x + 0.5);
}

//------------------------------------------------------------------------------
int toInt(const float p_x)
{
    return lrintf(p_x);
}

//------------------------------------------------------------------------------
int toInt(const double & p_x)
{
    return lrint(p_x);
}

//------------------------------------------------------------------------------
int intFloor(const float p_x)
{
    return lrintf(p_x - 0.5);
}

//------------------------------------------------------------------------------
int intFloor(const double & p_x)
{
    return lrint(p_x - 0.5);
}

//------------------------------------------------------------------------------
int roundUp( const int p_x
           , const int p_multiple
           )
{
    int l_remainder = p_x % p_multiple;
    return (l_remainder == 0) ? p_x : p_x - l_remainder + p_multiple;
}

//------------------------------------------------------------------------------
template<class T>
bool within( T p_plus_minus
           , T p_value_1
           , T p_value_2
           )
{
    return (p_value_1 >= p_value_2 - p_plus_minus) ? (p_value_1 <= p_value_2 + p_plus_minus) : false;
}

//------------------------------------------------------------------------------
template<class T>
bool between( T p_x
            , T p_lower_bound
            , T p_upper_bound
            )
{
    return (p_x >= p_lower_bound) && (p_x <= p_upper_bound);
}

//------------------------------------------------------------------------------
template<class T>
T bound( T p_var
       , T p_lower_bound
       , T p_upper_bound
       )
{
    //this way will deal with NAN, setting it to lowerBound
    if(p_var >= p_lower_bound)
    {
        if(p_var <= p_upper_bound)
        {
            return p_var;
        }
        else
        {
            return p_upper_bound;
        }
    }
    else
    {
        return p_lower_bound;
    }
}

//------------------------------------------------------------------------------
double parabolaTurningPoint( const double & p_a
                           , const double & p_b
                           , const double & p_c
                           )
{
    double l_bottom = (2 * (p_c + p_a - 2 * p_b));
    return (l_bottom == 0.0) ? 0.0 : ((p_a - p_c) / l_bottom);
}

//------------------------------------------------------------------------------
template<class T>
inline void parabolaTurningPoint2( T p_y_1
                                 , T p_y0
                                 , T p_y1
                                 , T p_x_offset
                                 , T * p_x
                                 , T * p_y
                                 )
{
    T l_y_top = p_y_1 - p_y1;
    T l_y_bottom = p_y1 + p_y_1 - 2 * p_y0;
    if(l_y_bottom != 0.0)
    {
        *p_x = p_x_offset + l_y_top / (2 * l_y_bottom);
        *p_y = p_y0 - (sq(l_y_top) / (8 * l_y_bottom));
    }
    else
    {
        *p_x = p_x_offset;
        *p_y = p_y0;
    }
}

//------------------------------------------------------------------------------
MinMax::MinMax(void)
: m_min(0.0)
, m_max(0.0)
{
}

//------------------------------------------------------------------------------
MinMax::MinMax( float p_min
              , float p_max
              )
: m_min(p_min)
, m_max(p_max)
{
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
int maxIndex( ForwardIterator p_attay_first
            , int p_length
            )
{
    int l_best_index = 0;
    for(int l_j = 1; l_j < p_length; l_j++)
    {
        if(p_attay_first[l_j] > p_attay_first[l_best_index])
        {
            l_best_index = l_j;
        }
    }
    return l_best_index;
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
int minIndex( ForwardIterator p_array_first
            , int p_length
            )
{
    int l_best_index = 0;
    for(int l_j = 1; l_j < p_length; l_j++)
    {
        if(p_array_first[l_j] < p_array_first[l_best_index])
        {
            l_best_index = l_j;
        }
    }
    return l_best_index;
}

//------------------------------------------------------------------------------
template<typename _ForwardIter, typename _Compare>
std::pair<_ForwardIter, _ForwardIter> minMaxElement( _ForwardIter p_first
                                                   , _ForwardIter p_last
                                                   , _Compare p_less_comp
                                                   )
{
    // concept requirements
    //__glibcpp_function_requires(_ForwardIteratorConcept<_ForwardIter>)
    //__glibcpp_function_requires(_BinaryPredicateConcept<_Compare,
    //typename iterator_traits<_ForwardIter>::value_type,
    //typename iterator_traits<_ForwardIter>::value_type>)

    std::pair<_ForwardIter, _ForwardIter> l_result(p_first, p_first);
    if(p_first == p_last)
    {
        return l_result;
    }
    while (++p_first != p_last)
    {
        if (p_less_comp(*p_first, *l_result.first))
        {
            l_result.first = p_first;
        }
        if (p_less_comp(*l_result.second, *p_first))
        {
            l_result.second = p_first;
        }
    }
    return l_result;
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
void addElements( ForwardIterator p_a_first
                , ForwardIterator p_a_last
                , ForwardIterator p_b_first
                )
{
    while(p_a_first != p_a_last)
    {
        *p_a_first += *p_b_first;
        ++p_b_first;
        ++p_a_first;
    }
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
void addElements( ForwardIterator p_a_first
                , ForwardIterator p_a_last
                , ForwardIterator p_b_first
                , ElementType p_scaler
                )
{
    //while(p_a_first != p_a_last) *p_b_first++ += *p_a_first++;
    while(p_a_first != p_a_last)
    {
        *p_a_first += (*p_b_first) * p_scaler;
        ++p_b_first; ++p_a_first;
    }
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
void copyElementsScale( ForwardIterator p_a_first
                      , ForwardIterator p_a_last
                      , ForwardIterator p_b_first
                      , ElementType p_scaler
                      )
{
    while(p_a_first != p_a_last)
    {
        *p_b_first++ = (*p_a_first++) * p_scaler;
    }
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
void copyElementsDivide( ForwardIterator p_a_first
                       , ForwardIterator p_a_last
                       , ForwardIterator p_b_first
                       , ElementType p_div
                       )
{
    while(p_a_first != p_a_last)
    {
        *p_b_first++ = (*p_a_first++) / p_div;
    }
}

//------------------------------------------------------------------------------
template <class T>
T absolute(const T & p_x)
{
    return (p_x >= 0) ? p_x : -p_x;
}

//------------------------------------------------------------------------------
template <class T>
bool absoluteLess<T>::operator()( const T & p_x
                                , const T & p_y
                                ) const
{
    return absolute(p_x) < absolute(p_y);
}

//------------------------------------------------------------------------------
template <class T>
bool absoluteGreater<T>::operator()( T & p_x
                                   , T & p_y
                                   ) const
{
    return absolute(p_x) > absolute(p_y);
}

//------------------------------------------------------------------------------
template <class ForwardIterator, class ElementType>
ForwardIterator binary_search_closest( ForwardIterator p_a_first
                                     , ForwardIterator p_a_last
                                     , const ElementType & p_value
                                     )
{
    std::pair<ForwardIterator, ForwardIterator> l_range = std::equal_range(p_a_first, p_a_last, p_value);
    if(l_range.first != p_a_first)
    {
        --l_range.first;
    }
    if(l_range.second != p_a_last)
    {
        ++l_range.second;
    }
    ForwardIterator l_best = l_range.first;
    ForwardIterator l_iter = l_best;
    while(++l_iter != l_range.second)
    {
        if(absolute(*l_iter - p_value) < absolute(*l_best - p_value))
        {
            l_best = l_iter;
        }
    }
    return l_best;
}

//------------------------------------------------------------------------------
template <class ForwardIterator>
ForwardIterator print_elements( ForwardIterator p_a_first
                              , ForwardIterator p_a_last
                              )
{
    std::cout << "[";
    if(p_a_first != p_a_last)
    {
        std::cout << *p_a_first++;
    }
    while(p_a_first != p_a_last)
    {
        std::cout << " " << *p_a_first++;
    }
    std::cout << "]" << std::endl;
}
//EOF

