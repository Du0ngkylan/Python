/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPSalinityCalculation.h
 * @brief       Header file of class Salinity.
 * @author      PhongLe
 * @date        2019/03/13
 */

 #include "FAMSEPSalinityCalculation.h"


namespace famseventprocessing {
    const double Salinity::k = 0.0162;
    const double Salinity::ax[] = {
        0.0080,
        -0.1692,
        25.3851,
        14.0941,
        -7.0261,
        2.7081 };
    const double Salinity::bx[] = {
        0.0005,
        -0.0056,
        -0.0066,
        -0.0375,
        0.00636,
        -0.0144 };
    const double Salinity::cx[] = {
        0.6766097,
        2.00564 * powf(10, -2),
        1.104259 * powf(10, -4),
        -6.9698 * powf(10, -7),
        1.0031 * powf(10, -9) };
    const double Salinity::dx[] = {
        3.426 * powf(10, -2),
        4.464 * powf(10, -4),
        4.215 * powf(10, -1),
        -3.107 * powf(10, -3) };
    const double Salinity::ex[] = {
            2.070 * powf(10, -5),
            -6.370 * powf(10, -10),
            3.989 * powf(10, -15) };


    double Salinity::getPractical(double conductivity, double temperature, double pressure) {
        static const auto sum = [](double p, double c) {return p + c;};
        static const auto a = toValarray(ax);
        static const auto b = toValarray(bx);
        static const auto c = toValarray(cx);
        static const auto d = toValarray(dx);
        static const auto e = toValarray(ex);
        double t = temperature * 1.00024;
        double R = conductivity / 42.914;

        int i = 0;
        auto rtx = map(c, [&](double x) {return x * powf(t, i++);});
        double rt = reduce(rtx, 0.f, sum);

        i = 1;
        auto Rpx = map(e, [&](double x) {return x * powf(pressure, i++);});
        double Rp = 1 + (reduce(Rpx, 0.f, sum) / (1 + (d[0] * t) + (d[1] * powf(t, 2)) + (d[2] + (d[3] * t)) * R));
        double Rt = R / (Rp * rt);

        std::valarray<double> n {
            0,
            1,
            2,
            3,
            4,
            5 };
        auto Sax = map(n, [&](double f) {return a[f] * powf(Rt, (f / 2.f));});
        auto Sbx = map(n, [&](double f) {return b[f] * powf(Rt, (f / 2.f));});
        auto Sa = reduce(Sax, 0.f, sum);
        auto Sb = reduce(Sbx, 0.f, sum);
        //  std::cout << "Sa: " << Sa << std::endl;
        //  std::cout << "Sb: " << Sb << std::endl;
        double S = Sa + ((t - 15) / (1 + (k * (t - 15))) * Sb);

        return S;
    }
}