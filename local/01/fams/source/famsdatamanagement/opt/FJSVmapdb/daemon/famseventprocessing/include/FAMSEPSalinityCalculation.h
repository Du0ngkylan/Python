/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPSalinityCalculation.h
 * @brief       Header file of class Salinity.
 * @author      PhongLe
 * @date        2019/03/13
 */

#ifndef FAMS_EVENT_PROCESSING_MANAGEMENT_H_
#define FAMS_EVENT_PROCESSING_MANAGEMENT_H_

#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <valarray>
namespace famseventprocessing {

  template<typename TYPE, std::size_t SIZE> std::valarray<TYPE> toValarray(const TYPE (&array)[SIZE]) {
    std::valarray<TYPE> va(array, SIZE);
    return va;
  }

  template<class Container, class Transform> Container map(const Container& c, Transform t) {
    Container mapped(c.size());
    std::transform(std::begin(c), std::end(c), std::begin(mapped), t);
    return mapped;
  }

  template<class Container, class T, class BinaryOp> T reduce(const Container& c, T value, BinaryOp op) {
    return std::accumulate(std::begin(c), std::end(c), value, op);
  }

  class Salinity {
      private:
          static const double k;
          static const double ax[];
          static const double bx[];
          static const double cx[];
          static const double dx[];
          static const double ex[];
      public:
          static double getPractical(double conductivity, double temperature, double pressure);
          Salinity() {
          }
  };
}
#endif