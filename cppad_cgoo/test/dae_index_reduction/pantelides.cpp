/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2012 Ciengis

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

#include <cppad_cgoo/cg.hpp>

#include "gcc_load_dynamic.hpp"
#include "pendulum.hpp"

inline bool PantelidesPendulum2D() {
    using namespace CppAD;
    using namespace std;
    typedef CG<double> CGD;

    // create f: U -> Z and vectors used for derivative calculations
    ADFun<CGD>* fun = Pendulum2D<CGD > ();

    std::vector<bool> eqDifferentialInfo(5, true);
    eqDifferentialInfo[4] = false;
    std::vector<bool> varInfo(6, true);
    varInfo[5] = false;

    Plantelides<double> pantelides(fun, eqDifferentialInfo, varInfo);

    pantelides.reduceIndex();

    delete fun;

    return false;
}

inline bool PantelidesPendulum3D() {
    using namespace CppAD;
    using namespace std;
    typedef CG<double> CGD;

    // create f: U -> Z and vectors used for derivative calculations
    ADFun<CGD>* fun = Pendulum3D<CGD > ();

    std::vector<bool> eqDifferentialInfo(7, true);
    eqDifferentialInfo[6] = false;
    std::vector<bool> varInfo(7, true);

    Plantelides<double> pantelides(fun, eqDifferentialInfo, varInfo);

    pantelides.reduceIndex();

    delete fun;

    return false;
}

bool Pantelides() {
    bool ok = true;
    ok &= PantelidesPendulum2D();
    ok &= PantelidesPendulum3D();
    return ok;
}
